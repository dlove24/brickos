
/*! \file   loader.c
    \brief  legOS task downloading
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
 *  The Original Code is legOS code, released October 2, 1999.
 *
 *  The Initial Developer of the Original Code is Markus L. Noga.
 *  Portions created by Markus L. Noga are Copyright (C) 1999
 *  Markus L. Noga. All Rights Reserved.
 *
 *  Contributor(s): everyone discussing LNP at LUGNET
 */

/*  2000.05.01
 *
 *  Modifications to the original loader.c file for LegOS 0.2.2 include:
 *
 *  Paolo Masetti's patches to eliminate "Invalid Argument" error and
 *  to get rid of several compiler warnings:
 *     <paolo.masetti@itlug.org>
 *     http://www.lugnet.com/robotics/rcx/legos/?n=619
 *
 *  Markus L. Noga's solution to Cygwin's failure to define the O_ASYNC symbol:
 *     <markus@noga.de>
 *     http://www.lugnet.com/robotics/rcx/legos/?n=439
 *
 *  Paolo Masetti's adaptation for definitive porting to Win32. No more errors in
 *  serial communication and conditional compile for Winnt (cygnwin).
 *  Several addition to dll option to better support user. Execute dll without
 *  arguments to get help.
 *     <paolo.masetti@itlug.org>
 */

/*  2002.04.01
 *
 *  Modifications to the original loader.c file in LegOS 0.2.4 include:
 *
 *  Hary D. Mahesan's update to support USB IR firmware downloading 
 *  using RCX 2.0's USB tower under WIN32 on Cygwin.
 *	<hdmahesa@engmail.uwaterloo.ca>
 *	<hmahesan@hotmail.com>
 *
 *  CVS inclusion, revision and modification by Paolo Masetti.
 *	<paolo.masetti@itlug.org>
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <signal.h>
#include <errno.h>

#if defined(_WIN32)
  #include <windows.h>
#endif

#include <sys/lnp.h>
#include <sys/lnp-logical.h>

#include "rcxtty.h"
#include "keepalive.h"
#include <lx.h>

#define MAX_DATA_CHUNK 0xf8   	  //!< maximum data bytes/packet for boot protocol
#define XMIT_RETRIES   5      	  //!< number of packet transmit retries
#define REPLY_TIMEOUT  750000 	  //!< timeout for reply
#define BYTE_TIME      (1000*LNP_BYTE_TIME) //!< time to transmit a byte.

#define DEFAULT_DEST  	0
#define DEFAULT_PROGRAM	0
#define DEFAULT_SRCPORT 0
#define DEFAULT_PRIORITY 10

typedef enum {
  CMDacknowledge,     		//!< 1:
  CMDdelete, 	      	//!< 1+ 1: b[nr]
  CMDcreate, 	      	//!< 1+12: b[nr] s[textsize] s[datasize] s[bsssize] s[stacksize] s[start] b[prio]
  CMDoffsets, 	      	//!< 1+ 7: b[nr] s[text] s[data] s[bss]
  CMDdata,   	      	//!< 1+>3: b[nr] s[offset] array[data]
  CMDrun,     	      	//!< 1+ 1: b[nr]
  CMDirmode,			//!< 1+ 1: b[0=near/1=far]
  CMDlast     	      	//!< ?
} packet_cmd_t;

#if defined(__sun__) && defined(__svr4__)	// Solaris
#undef HAVE_GETOPT_LONG
#else
#define HAVE_GETOPT_LONG 1
#endif

#ifdef HAVE_GETOPT_LONG
#include <getopt.h>

static const struct option long_options[]={
  {"rcxaddr",required_argument,0,'r'},
  {"program",required_argument,0,'p'},
  {"srcport",required_argument,0,'s'},
  {"tty",    required_argument,0,'t'},
  {"irmode", required_argument,0,'i'},
  {"execute",no_argument      ,0,'e'},
  {"verbose",no_argument      ,0,'v'},
  {0        ,0                ,0,0  }
};

#else // HAVE_GETOPT_LONG

#define getopt_long(ac, av, opt, lopt, lidx) (getopt((ac), (av), (opt)))

#endif // HAVE_GETOPT_LONG

volatile int receivedAck=0;

volatile unsigned short relocate_to=0;

unsigned int  rcxaddr = DEFAULT_DEST,
              prog    = DEFAULT_PROGRAM,
              srcport = DEFAULT_SRCPORT,
		  irmode  = -1;
  
int run_flag=0;
int verbose_flag=0;
int tty_usb=0;

void io_handler(void);

/*! blocking I/R write.
 *! return number of bytes written, or negative for error.
 */
int lnp_logical_write(const void *data, size_t length) {

// With Win32 we are using Blocking Write by default
#if !defined(_WIN32)
  fd_set fds;

  // wait for transmission
  //
  do {
    FD_ZERO(&fds);
    FD_SET(rcxFD(),&fds);
  } while(select(rcxFD()+1,NULL,&fds,NULL,NULL)<1);
#endif

  // transmit
  //
  keepaliveRenew();

  return mywrite(rcxFD(), data, length)!=length;
}

//! send a LNP layer 0 packet of given length
/*! \return 0 on success.
*/
int lnp_assured_write(const unsigned char *data, unsigned char length,
                      unsigned char dest, unsigned char srcport) {
  int i;
  struct timeval timeout,now;
  unsigned long total,elapsed;
  
  for(i=0; i<XMIT_RETRIES; i++) {
    receivedAck=0;
    
    lnp_addressing_write(data,length,dest,srcport);
    
    gettimeofday(&timeout,0);
    total=REPLY_TIMEOUT+length*BYTE_TIME;
    elapsed=0;

    do {
#if defined(_WIN32)
      io_handler();
#else
      struct timeval tv;
      fd_set fds;

      FD_ZERO(&fds);
      FD_SET(rcxFD(), &fds);

      tv.tv_sec = (total - elapsed) / 1000000;
      tv.tv_usec = (total - elapsed) % 1000000;
      select(rcxFD() + 1, &fds, NULL, NULL, &tv);
      if (FD_ISSET(rcxFD(), &fds))
		io_handler();
#endif
 
      gettimeofday(&now,0);
      elapsed=1000000*(now.tv_sec  - timeout.tv_sec ) +
	       	       now.tv_usec - timeout.tv_usec;
      
    } while((!receivedAck) && (elapsed < total));
    
    if(i || !receivedAck)
      if(verbose_flag)
        fprintf(stderr,"try %d: ack:%d\n",i,receivedAck);
    
    if(receivedAck)
      return 0;    
   }
  
  return -1;
}

void io_handler(void) {
  
    static struct timeval last={0,0};
    struct timeval now;
    unsigned long diff;
    
    unsigned char buffer[256];
#if defined(_WIN32)
    DWORD len=0;
    int i;
#else
    int len,i;
#endif
    
    gettimeofday(&now,0);
    diff= 1000000*now .tv_sec + now .tv_usec - 
	 (1000000*last.tv_sec + last.tv_usec);
    
    if(diff> 10000*LNP_BYTE_TIMEOUT) {
      if(verbose_flag)
        fprintf(stderr,"\n#time %lu ",diff);
      lnp_integrity_reset();
    }
#if defined(_WIN32)
    // Remember, USB support only in WIN32 environments.
    if (tty_usb == 0) {
	ReadFile(rcxFD(), buffer, sizeof(buffer), &len, NULL);
    } else {
	struct timeval timeout, timenow;
	unsigned long total, elapsed;
	
	gettimeofday(&timeout,0);
	total = REPLY_TIMEOUT+(long)sizeof(buffer)*BYTE_TIME;
	while(len == 0) {
		ReadFile(rcxFD(), buffer, sizeof(buffer), &len, NULL);
		gettimeofday(&timenow, 0);
		// calculate elapsed time as usual
		elapsed=1000000*(timenow.tv_sec - timeout.tv_sec ) + (timenow.tv_usec - timeout.tv_usec);
		if(elapsed > total)
			break;
	}
    }
#else
    len=read(rcxFD(),buffer,sizeof(buffer));
#endif
    for(i=0; i<len; i++) {
      if(verbose_flag)
        fprintf(stderr,"%02x ",buffer[i]);
      lnp_integrity_byte(buffer[i]);
    }
    gettimeofday(&last,0);
}

void LNPinit(const char *tty) {
  struct timeval timeout,now;
  long diff;

#if !defined(_WIN32)
  unsigned char buffer[256];
#endif
    
  // initialize RCX communications
  //
  if (verbose_flag) fputs("opening tty...\n", stderr);
#ifdef CONF_LNP_FAST
  rcxInit(tty, 1);
#else
  rcxInit(tty, 0);
#endif

  if (rcxFD() == BADFILE) {
    myperror("opening tty");
    exit(1);
  }
  keepaliveInit();
   
  // wait for IR to settle
  // 
  gettimeofday(&timeout,0);
  do {    
    usleep(100000);
    gettimeofday(&now,0);
    diff=1000000*(now.tv_sec  - timeout.tv_sec ) +
         	  now.tv_usec - timeout.tv_usec;
      
  } while(diff < 100000);
#if defined(_WIN32)
  PurgeComm(rcxFD(), PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);
#else
  read(rcxFD(),buffer,256);
#endif
}
    
void ahandler(const unsigned char *data,unsigned char len,unsigned char src) {
  if(*data==CMDacknowledge) {
    receivedAck=1;
    if(len==8) {
      // offset packet
      //
      relocate_to=(data[2]<<8)|data[3];
    }
  }
}
    
void lnp_download(const lx_t *lx) {
  unsigned char buffer[256+3];
  
  size_t i,chunkSize,totalSize=lx->text_size+lx->data_size;

  if(verbose_flag)
    fputs("\ndata ",stderr);

  buffer[0]=CMDdata;
  buffer[1]=prog;

  for(i=0; i<totalSize; i+=chunkSize) {
    chunkSize=totalSize-i;
    if(chunkSize>MAX_DATA_CHUNK)
      chunkSize=MAX_DATA_CHUNK;

    buffer[2]= i >> 8;
    buffer[3]= i &  0xff;
    memcpy(buffer+4,lx->text + i,chunkSize);
    if(lnp_assured_write(buffer,chunkSize+4,rcxaddr,srcport)) {
      fputs("error downloading program\n",stderr);
      exit(-1);
    }
  }
}

int main(int argc, char **argv) {
  lx_t lx;    	    // the legOS executable
  char *filename;
  int opt;
#ifdef HAVE_GETOPT_LONG
  int option_index;
#endif
  unsigned char buffer[256+3]="";
  char *tty=NULL;

  while((opt=getopt_long(argc, argv, "r:p:s:t:i:ev",
                        (struct option *)long_options, &option_index) )!=-1) {
    switch(opt) {
      case 'e':
	  run_flag=1;
        break;
      case 'r':
	  sscanf(optarg,"%x",&rcxaddr);
        break;
      case 'p':
	  sscanf(optarg,"%x",&prog);
        break;
      case 's':
	  sscanf(optarg,"%x",&srcport);
        break;
      case 't':
	  sscanf(optarg,"%s",buffer);
        break;
      case 'i':
	  sscanf(optarg,"%x",&irmode);
        break;
      case 'v':
	  verbose_flag=1;
	  break;
    }
  }           
  
  // load executable
  //      
  if(argc-optind<1) {
    char *usage_string =
	"  -p<prognum>  , --program=<prognum>   set destination program to <prognum>\n"
	"  -r<rcxadress>, --rcxaddr=<rcxadress> set RCX address to <rcxaddress>\n"
	"  -s<srcadress>, --srcport=<srcadress> set RCX sourceport to <srcaddress>\n"
	"  -t<comport>  , --tty=<comport>       set IR Tower com port <comport>\n"
#if defined(_WIN32)
	"  -t<usb>      , --tty=<usb>           set IR Tower USB mode \n"
#endif
	"  -i<0/1>      , --irmode=<0/1>        set IR mode near(0)/far(1) on RCX\n"
	"  -e           , --execute             execute program after download\n"
	"  -v           , --verbose             verbose mode\n"
	"\n"
	"Default COM port or USB support can be set using environment variable RCXTTY.\n"
	"Eg:\tset RCXTTY=COM2\n"
	   "\tset RCXTTY=USB\n"
	"\n"
	;

	fprintf(stderr, "usage: %s [options] file.lx\n", argv[0]);
	fprintf(stderr, usage_string);

    return -1;
  }
  filename=argv[optind++];
  if(lx_read(&lx,filename)) {
    fprintf(stderr,"unable to load legOS executable from %s.\n",filename);
    return -1;
  }

  // Moved tty device setting for a new option on command line
  if (buffer[0]) tty=buffer;
  if (!tty) tty = getenv(TTY_VARIABLE);
  if (!tty) tty = DEFAULTTTY;

  // Check if USB IR tower is selected.
#if defined(_WIN32)
  if (stricmp(tty, "usb")==0) {
	tty_usb = 1;
	if(verbose_flag)
		fputs("\n\n Hary Mahesan - LEGO USB IR Tower Mode\n\n",stderr);
	tty="//./legotower1"; // Set the correct usb tower if you have more than one (unlikely).
  }
#endif

  LNPinit(tty);

  if (verbose_flag)
    fputs("\nLNP Initialized...\n",stderr);

  lnp_addressing_set_handler(0,ahandler);

  if(verbose_flag)  
    fprintf(stderr,"loader hostaddr=0x%02x hostmask=0x%02x portmask=0x%02x\n",
            rcxaddr & 0x00ff, LNP_HOSTMASK & 0x00ff, srcport & 0x00ff);

  // Set IR mode
  if (irmode != -1) {
    buffer[0]=CMDirmode;
    buffer[1]=irmode;
    if(lnp_assured_write(buffer,2,rcxaddr,srcport)) {
      fputs("error setting IR mode to far\n",stderr);
      return -1;
    }
  }
  
  if(verbose_flag)
    fputs("\ndelete",stderr);
  buffer[0]=CMDdelete;
  buffer[1]=prog; //       prog 0
  if(lnp_assured_write(buffer,2,rcxaddr,srcport)) {
    fputs("error deleting program\n",stderr);
    return -1;
  }

  if(verbose_flag)
    fputs("\ncreate ",stderr);
  buffer[ 0]=CMDcreate;
  buffer[ 1]=prog; //       prog 0
  buffer[ 2]=lx.text_size>>8;
  buffer[ 3]=lx.text_size & 0xff;
  buffer[ 4]=lx.data_size>>8;
  buffer[ 5]=lx.data_size & 0xff;
  buffer[ 6]=lx.bss_size>>8;
  buffer[ 7]=lx.bss_size & 0xff;
  buffer[ 8]=lx.stack_size>>8;
  buffer[ 9]=lx.stack_size & 0xff;
  buffer[10]=lx.offset >> 8;  	// start offset from text segment
  buffer[11]=lx.offset & 0xff; 
  buffer[12]=DEFAULT_PRIORITY;
  if(lnp_assured_write(buffer,13,rcxaddr,srcport)) {
    fputs("error creating program\n",stderr);
    return -1;
  }

  // relocation target address in relocate_to
  //
  lx_relocate(&lx,relocate_to);

  lnp_download(&lx);

  if (run_flag) {
    if(verbose_flag)
      fputs("\nrun ",stderr);
    buffer[0]=CMDrun;
    buffer[1]=prog; //       prog 0
    if(lnp_assured_write(buffer,2,rcxaddr,srcport)) {
      fputs("error running program\n",stderr);
      return -1;
    }
  }
      
  return 0;
}
