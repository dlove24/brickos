/*! \file   rcxtty.h
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
 *  Contributor(s):	Paolo Masetti <paolo.masetti@itlug.org>
 *
 */

#ifndef __rcxtty_h__
#define __rcxtty_h__

#if defined(_WIN32)
  #define FILEDESCR	HANDLE
  #define BADFILE	NULL
#else
  #define FILEDESCR	int
  #define BADFILE	-1
#endif

// port settings
//
#define TTY_VARIABLE	"RCXTTY"	//!< environment variable to override defaults

#if defined(linux) || defined(LINUX)
#define DEFAULTTTY   "/dev/ttyS0"	//!< Linux  - COM1
#elif defined(_WIN32)
#define DEFAULTTTY   "com1"	      	//!< Cygwin - COM1
#elif defined(sparc)
#define DEFAULTTTY   "/dev/ttyb"	//!< Sun - serial port B
#else
#define DEFAULTTTY   "/dev/ttyd2"	//!< IRIX - second serial port
#endif

//! initialize RCX communications port
int rcxInit(const char *tty, int highspeed);

//! shutdown RCX communications port
void rcxShutdown();

//! get RCX file descriptor
FILEDESCR rcxFD(void);

//! perror() wrapper
void myperror(char *str);

//! write() wrapper
int mywrite(FILEDESCR fd, const void *buf, size_t len);

#endif
