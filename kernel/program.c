/*! \file program.c
    \brief  Implementation: program data structures and functions
    \author Markus L. Noga <markus@noga.de>
*/

/*
 *  The contents of this file are subject to the Mozilla Public License
 *  Version 1.0 (the "License"); you may not use this file except in
 *  compliance with the License. You may obtain a copy of the License at
 *  http://www.mozilla.org/MPL/
 *
 *  Software distributed under the License is distributed on an "AS IS"
 *  basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
 *  License for the specific language governing rights and limitations
 *  under the License.
 *
 *  The Original Code is legOS code, released October 17, 1999.
 *
 *  The Initial Developer of the Original Code is Markus L. Noga.
 *  Portions created by Markus L. Noga are Copyright (C) 1999
 *  Markus L. Noga. All Rights Reserved.
 *
 *  Contributor(s): Markus L. Noga <markus@noga.de>
 *                  Frank Cremer <frank@demon.holly.nl>
 */

/*
 *  2000.05.01 - Paolo Masetti <paolo.masetti@itlug.org>
 *
 *  - Added "CMDirmode" for changing via LNP IR mode
 *
 *  2001.05.10 - Matt Ahrens <mahrens@acm.org>
 *
 *    - Added free memory and batter life display
 *      Press "view" repeatedly while no programs are running to see
 *
 *  2002.4.23 - Ted Hess <thess@kitschensync.net>
 *
 *  - Added Remote key handler
 */

#include <sys/program.h>

#ifdef CONF_PROGRAM

#include <sys/tm.h>
#include <string.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdlib.h>

#include <sys/lnp.h>
#include <sys/lnp-logical.h>
#include <sys/dmotor.h>
#include <sys/dsensor.h>
#include <sys/mm.h>
#include <sys/battery.h>
#include <dsound.h>
#include <remote.h>

#include <conio.h>

///////////////////////////////////////////////////////////////////////////////
//
// Global Variables
//
///////////////////////////////////////////////////////////////////////////////

volatile unsigned cprog;                //!< the current program

///////////////////////////////////////////////////////////////////////////////
//
// Internal Variables
//
///////////////////////////////////////////////////////////////////////////////

const unsigned char min_length[]={
   1, // CMDacknowledge
   2, // CMDdelete
  13, // CMDcreate
   8, // CMDoffsets
   4, // CMDdata
   2, // CMDrun
   2, // CMDirmode
   2  // CMDsethost
};

static program_t programs[PROG_MAX];      //!< the programs

static unsigned char* buffer_ptr;         //!< packet receive buffer
volatile unsigned char packet_len;        //!< packet length
volatile unsigned char packet_src;        //!< packet sender

static sem_t packet_sem;                  //!< synchronization semaphore

#if 0
#define debugs(a) { cputs(a); msleep(500); }
#define debugw(a) { cputw(a); msleep(500); }
#else
#define debugs(a)
#define debugw(a)
#endif

// Forward ref
int lrkey_handler(unsigned int etype, unsigned int key);

///////////////////////////////////////////////////////////////////////////////
//
// Functions
//
///////////////////////////////////////////////////////////////////////////////

//! check if a given program is valid.
/*! \return 0 if invalid */
int program_valid(unsigned nr) {
  program_t *prog=programs+nr;

  return (nr < PROG_MAX) &&
         (prog->text_size>0) &&
         (prog->text_size+prog->data_size==prog->downloaded);
}

//! run the given program
static void program_run(unsigned nr) {
  if(program_valid(nr)) {
    program_t *prog=programs+nr;

    // initialize data segments
    //
    memcpy(prog->data,prog->data_orig,prog->data_size);
    memset(prog->bss,0,prog->bss_size);

    execi((void*) (((char*)prog->text)
            + prog->start  ),
    0,0,prog->prio,prog->stack_size);
  }
}

//! packet handler, called from interrupt
/*! allocates buffer, copies data and wakes parser task.
*/
static void packet_producer(const unsigned char *data,
                                unsigned char length,
                                unsigned char src) {
  // old packet still unhandled or empty packet?
  //
  if(packet_len>0 || length==0)
    return;

  if (buffer_ptr != 0)
    return;

  buffer_ptr = malloc(length);
  memcpy(buffer_ptr,data,length);
  packet_len=length;
  packet_src=src;
  sem_post(&packet_sem);
}

//! packet command parser task
static int packet_consumer(int argc, char *argv[]) {
  packet_cmd_t cmd;
  unsigned char nr=0;
  program_t *prog=programs; // to avoid a silly warning
  const static unsigned char acknowledge=CMDacknowledge;
  char msg[8];

  while (!shutdown_requested()) {
    // wait for new packet
    //
    packet_len=0;
    free(buffer_ptr);
    buffer_ptr = 0;
    if (sem_wait(&packet_sem) != -1) {
      if (buffer_ptr == 0)
        continue;

      debugw(*(size_t*)buffer_ptr);

      // handle trivial errors
      //
      cmd=buffer_ptr[0];
      if (cmd>=CMDlast || packet_len<min_length[cmd])
        continue;

      // handle IR CMDs
      if (cmd==CMDirmode) {
        if (buffer_ptr[1]==0) {
          debugs("nearmodeIR");
          lnp_logical_range(0);
          debugs("OK");
          lnp_addressing_write(&acknowledge,1,packet_src,0);
        } else {
          debugs("farmodeIR");
          lnp_logical_range(1);
          debugs("OK");
          lnp_addressing_write(&acknowledge,1,packet_src,0);
        }
        continue;
      }

      // Is this a request to change host address
      if (cmd == CMDsethost) {
        // ACK before we change our address
        lnp_addressing_write(&acknowledge,1,packet_src,0);
        lnp_set_hostaddr(buffer_ptr[1]);
        continue;
      }
  
      // Get program number, validate value
      if((cmd > CMDacknowledge) && (cmd <= CMDrun)) {
        nr = buffer_ptr[1];
        if(nr > PROG_MAX)
          continue;
#ifndef CONF_VIS
        cputc_hex_0(nr+1);
#endif
      }

      debugw(nr);
      prog = programs+nr;

      switch( cmd ) {
      case CMDdelete:
        debugs("dele");

        if(nb_tasks <= nb_system_tasks) {
          if(prog->text)
            free(prog->text);
          memset(prog,0,sizeof(program_t));

#ifndef CONF_VIS
          if(nr == cprog)
            cputc_0('-');
#endif
          debugs("OK");
  
          lnp_addressing_write(&acknowledge,1,packet_src,0);
        }
        break;

      case CMDcreate:
        debugs("crea");
        if(!prog->text) {
          memcpy(&(prog->text_size),buffer_ptr+2,11);

          if((prog->text=malloc(prog->text_size+
                2*prog->data_size+
                prog->bss_size  ))) {
            prog->data=prog->text+prog->text_size;
            prog->bss=prog->data+prog->data_size;
            prog->data_orig=prog->bss +prog->bss_size;
            prog->downloaded=0;

            debugs("OK");

            cputw(0);
            cprog = nr;

            msg[0]=CMDacknowledge;
            msg[1]=nr;
            memcpy(msg+2,prog,6);
            lnp_addressing_write(msg,8,packet_src,0);
          } else
            memset(prog,0,sizeof(program_t));
        }
        break;

      case CMDdata:
        debugs("data");
        if(prog->text && !program_valid(nr)) {
          size_t offset=*(size_t*)(buffer_ptr+2);
          if(offset<=prog->downloaded) {
            if(offset==prog->downloaded) {
              memcpy(prog->text+offset,buffer_ptr+4,packet_len-4);
              prog->downloaded+=packet_len-4;

              if(program_valid(nr)) {
                // copy original data segment and we're done.
                //
                memcpy(prog->data_orig,prog->data,prog->data_size);
                cls();
              } else
                cputw(prog->downloaded);
              debugs("OK");
            } else
              debugs("OLD");

            lnp_addressing_write(&acknowledge,1,packet_src,0);
          }
        }
        break;

      case CMDrun:
        debugs("run");
        if(program_valid(nr)) {
          cprog = nr;
          program_stop(0);
          program_run(nr);

          debugs("OK");
          lnp_addressing_write(&acknowledge,1,packet_src,0);
        }
        break;

      default:
        debugs("error");
      }
    }
  }
  free(buffer_ptr);
  return 0;
}

//! stop program
void program_stop(int flag) {
  int count_down = 40;

  // Kindly request that all user tasks shutdown
  shutdown_tasks(T_USER);
  // Wait a bit
  while (--count_down && (nb_tasks > nb_system_tasks)) {
    if (flag)
      cputs("STOP");
    msleep(100);
  }
    
  if (nb_tasks > nb_system_tasks) {
    // Wait no longer.
    if (flag) {
      cputs("KILL");
      // display immediately
      lcd_refresh();
    }
    killall(PRIO_HIGHEST-1);
  }

  // Reset motors, sensors, sound & LNP as
  // programs may have motors running,
  // sensors active or handlers set.
  //
  // Programs that exit on their own
  // are assumed to take the necessary
  // actions themselves.
  //
#ifdef CONF_DSOUND
  dsound_stop();
#endif
#ifdef CONF_DMOTOR
  dm_init();
#endif
#ifdef CONF_DSENSOR
  ds_init();
#endif
  lnp_init();
#ifdef CONF_LR_HANDLER
  // Reset remote button handler
  lr_init();
  lr_set_handler(lrkey_handler);
#endif
}

//! handle key input (on/off, run, program)
int key_handler(int argc, char *argv[]) {
  int c;

#ifndef CONF_VIS
  cputc_0('-');
#endif

  while (!shutdown_requested()) {
    int clear=0;
    c=getchar();

gotkey:

    debugs("key "); debugw(c);
    debugs("task"); debugw(nb_tasks);

    switch(c) {
      case KEY_ONOFF:
        cputs("OFF");
        // Kindly request all tasks shutdown
        shutdown_tasks(T_USER | T_KERNEL);
        // Except for key_handler
        ctid->tflags &= (~T_SHUTDOWN);
        // Wait a bit
        clear = 50;
        while (--clear && (nb_tasks > 2))
         msleep(100);
        // Wait no longer.
        if (nb_tasks > 2)
          killall(PRIO_HIGHEST);
        // Now key_handler should shutdown
        ctid->tflags |= T_SHUTDOWN;
        break;

      case KEY_RUN:
        // toggle: start/stop program
        if(nb_tasks > nb_system_tasks) {
          // if program running, stop it
          clear=1;
          program_stop(1);
        } else if(program_valid(cprog)) {
          program_stop(0);
          program_run(cprog);
        } else {
          cputs("NONE");
          clear=1;
        }
        break;

      case KEY_PRGM:
        // works only if no programs are running.
        if(nb_tasks <= nb_system_tasks) {
          int i;
          for(i=0; i<PROG_MAX; i++) {
            if( (++cprog)>=PROG_MAX)
              cprog=0;
            if(program_valid(cprog))
              break;
          }
          if(i==PROG_MAX) {
            cputs("NONE");
            clear=1;
#ifndef CONF_VIS
            cputc_0('-');
          }
          else
            cputc_hex_0(cprog+1);
#else
          }
#endif

        }
        break;
      case KEY_VIEW:
         // works only if no programs are running.
        if (nb_tasks > nb_system_tasks)
          break;
        /*
         * pressing the "view" button cycles through a display of the
         * amount of the amount of free memory (in decimal and
         * hexadecimal) and battery power. If a button other than "view"
         * is pressed while cycling through, we handle that button
         * ("goto gotkey").
         */
        cputs("addr");
        if ((c = getchar()) != KEY_VIEW) goto gotkey;
        cputw(lnp_hostaddr);
        while ((c = getchar()) == KEY_PRGM) {
          lnp_hostaddr += 0x10;
          lnp_hostaddr &= CONF_LNP_HOSTMASK;
          cputw(lnp_hostaddr);
        }
        if (c != KEY_VIEW) goto gotkey;
        if (program_valid(cprog)) {
          cputs("dele");
          if ((c = getchar()) != KEY_VIEW && c != KEY_PRGM) goto gotkey;
          if (c == KEY_PRGM) {
            program_t *prog=programs+cprog;
            if (prog->text)
              free(prog->text);
            memset(prog,0,sizeof(program_t));
            cputc_0('-');
          }
        }
        
        cputs("free");
        if ((c = getchar()) != KEY_VIEW) goto gotkey;
        lcd_int(mm_free_mem());
        if ((c = getchar()) != KEY_VIEW) goto gotkey;
        cputw(mm_free_mem());
        if ((c = getchar()) != KEY_VIEW) goto gotkey;

#if defined(CONF_DSENSOR)
        cputs("batt");
        if ((c = getchar()) != KEY_VIEW) goto gotkey;
        lcd_int(get_battery_mv());
        if ((c = getchar()) != KEY_VIEW) goto gotkey;
#endif // CONF_DSENSOR

        clear=1;
        break;
    }

    if(clear) {
      wait_event(dkey_released,KEY_ANY);
      cls();
    }
  }
  return 0;
}

#if defined(CONF_LR_HANDLER)
//! handle remote key input (P1-P5, A1-C1, A2-C2, stop, beep)
int lrkey_handler(unsigned int etype, unsigned int key) {
  unsigned char pnr = 0;

  // If a program is running, stop it
  //  NOTE: this LRKEY is allowed while a program is running!
  if(key == LRKEY_STOP && etype == LREVT_KEYON && nb_tasks > nb_system_tasks) {
    program_stop(1);
    return 1; // we consumed key
  }
  
  // Only interested if no program is running
  if(nb_tasks <= nb_system_tasks) {
    // Keydown events dispatched here
    if (etype == LREVT_KEYON) {
      switch (key) {
#ifdef CONF_DSOUND
        case LRKEY_BEEP:
          // Need high pitched beep-beep
          dsound_system(0);
          break;
#endif // CONF_DSOUND
        
        case LRKEY_P5:
          pnr++;
          // ... Fallthru
        case LRKEY_P4:
          pnr++;
          // ... Fallthru
        case LRKEY_P3:
          pnr++;
          // ... Fallthru
        case LRKEY_P2:
          pnr++;
          // ... Fallthru
        case LRKEY_P1:
          // Start something?
          if(program_valid(pnr)) 
          {
            cprog = pnr;
            // Reset things
            program_stop(0);
#ifdef CONF_VIS
            cputc_hex_0(pnr+1);
#ifndef CONF_LCD_REFRESH
            lcd_refresh();
#endif
#endif
            // launch Program(n)
            program_run(pnr);
          } else {
            //  no such program downloaded
           cputs("NONE");
          }
          break;

#if defined(CONF_DMOTOR)        
        // Motor on commands
        case LRKEY_A1:
          // A Motor fwd
          motor_a_dir(fwd);
          break;
        case LRKEY_A2:
          // A Motor rev
          motor_a_dir(rev);
          break;
        case LRKEY_B1:
          // B Motor fwd
          motor_b_dir(fwd);
          break;
        case LRKEY_B2:
          // B Motor rev
          motor_b_dir(rev);
          break;
        case LRKEY_C1:
          // C Motor fwd
          motor_c_dir(fwd);
          break;
        case LRKEY_C2:
          // C Motor rev
          motor_c_dir(rev);
          break;
#endif // CONF_DMOTOR
        default:
          // Not consumed
          return 0;
      }
#ifndef CONF_LCD_REFRESH
      lcd_refresh();
#endif
      // Key consumed
      return 1;
    }
  
    // Keyup events dispatched here
    if (etype == LREVT_KEYOFF) {
      switch (key) {
#if defined(CONF_DMOTOR)
        case LRKEY_A1:
        case LRKEY_A2:
          // Shut off A motor
          motor_a_dir(brake);
          break;
        case LRKEY_B1:
        case LRKEY_B2:
          // Shut off B motor
          motor_b_dir(brake);
          break;
        case LRKEY_C1:
        case LRKEY_C2:
          // Shut off C motor
          motor_c_dir(brake);
          break;
#endif // CONF_DMOTOR
        case LRKEY_P1:
        case LRKEY_P2:
        case LRKEY_P3:
        case LRKEY_P4:
        case LRKEY_P5:
        case LRKEY_STOP:
          // remove the STOP (or NONE) message
          cls();
          break;
        default:
          return 0;
      }
      // Used the key
      return 1;
    }
  }

#ifndef CONF_LCD_REFRESH
  lcd_refresh();
#endif
  // Didn't eat the key
  return 0;
}
#endif

//! initialize program support
/*! run in single tasking mode
*/
void program_init() {
  packet_len=0;
  sem_init(&packet_sem,0,0);
  execi(&packet_consumer,0,0,PRIO_HIGHEST,DEFAULT_STACK_SIZE);
  execi(&key_handler,0,0,PRIO_HIGHEST,DEFAULT_STACK_SIZE);

#ifdef CONF_LR_HANDLER
  // Setup kernel remote callback handler and dispatch thread
  lr_startup();
  lr_set_handler(lrkey_handler);
#endif

  lnp_addressing_set_handler(0,&packet_producer);
  buffer_ptr = 0;
}

//! shutdown program support
/*! run in single tasking mode
*/
void program_shutdown() {
  lnp_addressing_set_handler(0,LNP_DUMMY_ADDRESSING);
  sem_destroy(&packet_sem);

#ifdef CONF_LR_HANDLER
  lr_shutdown();
#endif
}

#endif // CONF_PROGRAM
