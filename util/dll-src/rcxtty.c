/*! \file   rcxtty.c
    \brief  Basic RCX communication
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
 *  Contributor(s): Kekoa Proudfoot  <kekoa@graphics.stanford.edu>
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
#include <fcntl.h>
#include <unistd.h>

#if defined(_WIN32)
  #include <windows.h>
#else
  #include <termios.h>
  #include <string.h>
#endif

#include "rcxtty.h"

extern int tty_usb;

//! the one and only RCX file descriptor
static FILEDESCR rcxFd = BADFILE;

//! get RCX file descriptor
FILEDESCR rcxFD(void) {
  return rcxFd;
}

void myperror(char *str) {
#if defined(_WIN32)
    fprintf(stderr, "Error %lu: %s\n", (unsigned long) GetLastError(), str);
#else
    perror(str);
#endif
}

int mywrite(FILEDESCR fd, const void *buf, size_t len) {
#if defined(_WIN32)
    DWORD nBytesWritten=0;
    if (WriteFile(fd, buf, len, &nBytesWritten, NULL))
      return nBytesWritten;
    else
      return -1;
#else
    return write(fd, buf, len);
#endif
}

//! initialize RCX communications port
int rcxInit(const char *tty, int highspeed)
{
/*
 *  Copyright (C) 1998, 1999, Kekoa Proudfoot.  All Rights Reserved.
 *
 *  License to copy, use, and modify this software is granted provided that
 *  this notice is retained in any copies of any part of this software.
 *
 *  The author makes no guarantee that this software will compile or
 *  function correctly.  Also, if you use this software, you do so at your
 *  own risk.
 *
 *  Kekoa Proudfoot
 *  kekoa@graphics.stanford.edu
 *  10/3/98
 */

//  char		*tty;
  FILEDESCR	fd;

  #if defined(_WIN32)
    DCB dcb;
    COMMTIMEOUTS CommTimeouts;
  #else
    struct termios ios;
  #endif

  if (*tty == '-' && !*(tty + 1))  // read from standard input if tty="-"
    fd = 0;
#if defined(_WIN32)
  else if ((fd = CreateFile(tty, GENERIC_READ | GENERIC_WRITE,
                                 0, NULL, OPEN_EXISTING,
                                 0, NULL)) == INVALID_HANDLE_VALUE) {
    fprintf(stderr, "Error %lu: Opening %s\n", (unsigned long) GetLastError(), tty);
#else
  else if ((fd = open(tty, O_RDWR | O_EXCL)) < 0) {
    perror("open");
#endif
    return -1;
  }

#if !defined(_WIN32)
  if (!isatty(fd)) {
    close(fd);
    fprintf(stderr, "%s: not a tty\n", tty);
    return -1;
  }
#endif

#if defined(_WIN32)
  // These settings below definitely do not apply to the USB IR Tower, so if() them out.
  if(tty_usb==0) {
    // Serial settings
    FillMemory(&dcb, sizeof(dcb), 0);
    if (!GetCommState(fd, &dcb)) {	// get current DCB
      // Error in GetCommState
      myperror("GetCommState");
      return -1;
    } else {
      dcb.ByteSize = 8;
      dcb.Parity   = (highspeed ? 0 : 1);		// 0-4=no,odd,even,mark,space
      dcb.StopBits = 0;					// 0,1,2 = 1, 1.5, 2
      dcb.fBinary  = TRUE ;
      dcb.fParity  = FALSE ;
      dcb.fAbortOnError = FALSE ;
      dcb.BaudRate = (highspeed ? CBR_4800 : CBR_2400);	// Update DCB rate.

      // Set new state.
      if (!SetCommState(fd, &dcb)) {
        // Error in SetCommState. Possibly a problem with the communications
        // port handle or a problem with the DCB structure itself.
        myperror("SetCommState");
        return -1;
      }
      if (!GetCommTimeouts (fd, &CommTimeouts)) myperror("GetCommTimeouts");

      // Change the COMMTIMEOUTS structure settings.
      CommTimeouts.ReadIntervalTimeout = MAXDWORD;
      CommTimeouts.ReadTotalTimeoutMultiplier = 0;
      CommTimeouts.ReadTotalTimeoutConstant = 0;
      CommTimeouts.WriteTotalTimeoutMultiplier = 10;
      CommTimeouts.WriteTotalTimeoutConstant = 1000;

      // Set the time-out parameters for all read and write operations
      // on the port.
      if (!SetCommTimeouts(fd, &CommTimeouts)) myperror("SetCommTimeouts");
    }
  }
  rcxFd=fd;
#else
  memset(&ios, 0, sizeof(ios));
  ios.c_cflag = CREAD | CLOCAL | CS8 | (highspeed ? 0 : PARENB | PARODD);

  cfsetispeed(&ios, highspeed ? B4800 : B2400);
  cfsetospeed(&ios, highspeed ? B4800 : B2400);

  if (tcsetattr(fd, TCSANOW, &ios) == -1) {
    perror("tcsetattr");
    exit(1);
  }
  rcxFd=fd;
#endif

  return 0;
}

//! shutdown RCX communications port
void rcxShutdown()
{
  #if defined(_WIN32)
    CloseHandle(rcxFd);
  #else
    close(rcxFd);
  #endif

  rcxFd=BADFILE;
}
