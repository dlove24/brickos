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

/* S-record routines */

/* srec.h */

typedef struct {
    unsigned char type;
    unsigned long addr;
    unsigned char count;
    unsigned char data[32];
} srec_t;

#define S_OK               0
#define S_NULL            -1
#define S_INVALID_HDR     -2
#define S_INVALID_CHAR    -3
#define S_INVALID_TYPE    -4
#define S_TOO_SHORT       -5
#define S_TOO_LONG        -6
#define S_INVALID_CKSUM   -7

extern int srec_decode(srec_t *srec, char *line);
extern int srec_encode(srec_t *srec, char *line);

