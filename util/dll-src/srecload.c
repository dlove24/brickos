/*! \file   srecload.c
    \brief  load symbol-srec files
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
 *  Contributor(s): Markus L. Noga
 */

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
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "srec.h"
#include "srecload.h"

#define IMG_LOWEST    0x0000
#define IMG_HIGHEST   0xffff
#define IMG_MAXSIZE   0x10000


//! load symbols from symbolsrec file
/*! \return current line in file

    the symbol section is enclosed in $$ 
    symbol line format is SYMBOL $ADDR
*/
static int symbols_load(image_t *img,FILE *file) {
  char buf[256];
  int doubledollar=0;
  unsigned short text=0,
                 text_end=0,
      	      	 data=0,
      	      	 data_end=0,
                 bss=0,
                 bss_end=0,
                 ctors=0,
                 ctors_end=0,
                 dtors=0,
                 dtors_end=0,
                 _main=0;
  int line=0;
    
  // read in symbols
  //
  while(doubledollar<2 && fgets(buf, 80, file)) {
    int i;
    line++;

    // skip empty lines
    //
    for(i=0; buf[i] && isspace(buf[i]); i++)
      ;
    if (!buf[i])
	continue;
    
    if(buf[i]=='$' && buf[i+1]=='$') {
      // delimiter?
      //
      doubledollar++;
    } else {
      // symbol / address pair
      //
      char *symbol;
      unsigned short address;
      
      if(doubledollar<1) {
	fprintf(stderr,"malformed symbolsrec file at line %d\n",line);
	exit(-1);
      }
      
      // read symbol
      //
      symbol=buf+i;
      while(buf[i] && buf[i]!=' ')
	i++;
      if(buf[i]!=' ' || buf[i+1]!='$') {
	fprintf(stderr,"malformed symbolsrec file at line %d\n",line);
	exit(-1);
      }
      buf[i]=0;
      
      address=(unsigned short) strtoul(buf+i+2,NULL,16);
  
      // retain relevant offsets
      //
      if(!strcmp(symbol,"___text"))
	text=address; 
      else if(!strcmp(symbol,"___text_end"))
	text_end=address; 
      else if(!strcmp(symbol,"___data"))
	data=address; 
      else if(!strcmp(symbol,"___data_end"))
	data_end=address; 
      else if(!strcmp(symbol,"___bss"))
	bss=address; 
      else if(!strcmp(symbol,"___bss_end"))
	bss_end=address; 
      else if(!strcmp(symbol,"___ctors"))
	ctors=address; 
      else if(!strcmp(symbol,"___ctors_end"))
	ctors_end=address; 
      else if(!strcmp(symbol,"___dtors"))
	dtors=address; 
      else if(!strcmp(symbol,"___dtors_end"))
	dtors_end=address; 
      else if(!strcmp(symbol,"_main"))
	_main=address; 
    }   
  }

  // save general file information
  //
  img->base     =text;  
  //  added ctor/dtor sections for C++.  They reside between text and data
  //   so they've been added to text for simplicity.  -stephen 14Jan01
  img->text_size=(text_end - text) + (ctors_end - ctors) + (dtors_end - dtors);
  img->data_size=data_end - data;
  img->bss_size = bss_end - bss;
  img->offset   =_main-text;
  
  return line;
}


void image_load(image_t *img,const char *filename)
{
  FILE *file;
  char buf[256];
  srec_t srec;
  int line;
  unsigned short size,start=0;

  if ((file = fopen(filename, "r")) == NULL) {
    fprintf(stderr, "%s: failed to open\n", filename);
    exit(1);
  }

  // read symbols from file
  //
  line=symbols_load(img,file);
  size=img->text_size+img->data_size;
  
  if((img->text=calloc(size,1))== NULL) {
    fprintf(stderr, "out of memory\n");
    exit(1);
  }
        
  // Build an image of the srecord data 
  //
  while (fgets(buf, 80, file)) {
    int i,error;
    line++;

    // skip empty lines
    //
    for(i=0; buf[i] && isspace(buf[i]); i++)
      ;
    if (!buf[i])
	continue;

    // decode line
    //
    if ((error = srec_decode(&srec, buf)) < 0) {
	char *errstr = NULL;
	switch (error) {
	  case S_NULL:         errstr = "null string error"; break;
	  case S_INVALID_HDR:  errstr = "invalid header";    break;
	  case S_INVALID_CHAR: errstr = "invalid character"; break;
	  case S_INVALID_TYPE: errstr = "invalid type";      break;
	  case S_TOO_SHORT:    errstr = "line to short";     break;
	  case S_TOO_LONG:     errstr = "line too line";     break;
	  case S_INVALID_CKSUM:       /* ignored */          break;
	  default:             errstr = "unknown error";     break;
	}
	if (errstr) {
	  fprintf(stderr, "%s: %s on line %d\n", filename, errstr, line);
	  exit(1);
	}
    }
    
    // handle lines
    //
    if (srec.type == 1) {
      if (srec.addr < img->base || srec.addr + srec.count > img->base + size) {
	fprintf(stderr, "%s: address [0x%4.4lX, 0x%4.4lX] out of bounds [0x%4.4X-0x%4.4X] on line %d\n",filename, srec.addr, (srec.addr + srec.count), img->base, (img->base + size), line);
	exit(1);
      }

      memcpy(img->text + srec.addr - img->base, srec.data, srec.count);
    }
    else if (srec.type == 9) {
      start = srec.addr;
    }
  }

  // trivial verification
  //
  if(start != img->base + img->offset) {
    fprintf(stderr, "%s: main isn't entry point\n",filename);
    exit(1);
  }
}
