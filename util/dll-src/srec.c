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

/* srec.c */

#include <stdio.h>

#include "srec.h"

static signed char ctab[256] = {
    -1,-1,-1,-1,-1,-1,-1,-1,  -1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,  -1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,  -1,-1,-1,-1,-1,-1,-1,-1,
     0, 1, 2, 3, 4, 5, 6, 7,   8, 9,-1,-1,-1,-1,-1,-1,
     0,10,11,12,13,14,15,-1,  -1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,  -1,-1,-1,-1,-1,-1,-1,-1,
     0,10,11,12,13,14,15,-1,  -1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,  -1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,  -1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,  -1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,  -1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,  -1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,  -1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,  -1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,  -1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,  -1,-1,-1,-1,-1,-1,-1,-1,
};

static int ltab[10] = {4,4,6,8,0,4,0,8,6,4};

#define C1(l,p)  (ctab[l[p]])
#define C2(l,p)  ((C1(l,p)<<4)|C1(l,p+1))

int
srec_decode(srec_t *srec, char *_line)
{
    int len, pos = 0, count, alen, sum = 0;
    unsigned char *line = (unsigned char *)_line;

    if (!srec || !line)
	return S_NULL;

    for (len = 0; line[len]; len++)
	if (line[len] == '\n' || line[len] == '\r')
	    break;

    if (len < 4)
	return S_INVALID_HDR;

    if (line[0] != 'S')
	return S_INVALID_HDR;

    for (pos = 1; pos < len; pos++) {
	if (C1(line, pos) < 0)
	    return S_INVALID_CHAR;
    }

    srec->type = C1(line, 1);
    count = C2(line, 2);

    if (srec->type > 9)
	return S_INVALID_TYPE;
    alen = ltab[srec->type];
    if (alen == 0)
	return S_INVALID_TYPE;
    if (len < alen + 6 || len < count * 2 + 4)
	return S_TOO_SHORT;
    if (count > 37 || len > count * 2 + 4)
	return S_TOO_LONG;

    sum += count;

    len -= 4;
    line += 4;

    srec->addr = 0;
    for (pos = 0; pos < alen; pos += 2) {
	unsigned char value = C2(line, pos);
	srec->addr = (srec->addr << 8) | value;
	sum += value;
    }

    len -= alen;
    line += alen;

    for (pos = 0; pos < len - 2; pos += 2) {
	unsigned char value = C2(line, pos);
	srec->data[pos / 2] = value;
	sum += value;
    }

    srec->count = count - (alen / 2) - 1;

    sum += C2(line, pos);

    if ((sum & 0xff) != 0xff)
	return S_INVALID_CKSUM;

    return S_OK;
}

int
srec_encode(srec_t *srec, char *line)
{
    int alen, count, sum = 0, pos;

    if (srec->type > 9)
	return S_INVALID_TYPE;
    alen = ltab[srec->type];
    if (alen == 0)
	return S_INVALID_TYPE;

    line += sprintf(line, "S%d", srec->type);

    if (srec->count > 32)
	return S_TOO_LONG; 
    count = srec->count + (alen / 2) + 1;
    line += sprintf(line, "%02X", count);
    sum += count;

    while (alen) {
	int value;
	alen -= 2;
	value = (srec->addr >> (alen * 4)) & 0xff;
	line += sprintf(line, "%02X", value);
	sum += value;
    }

    for (pos = 0; pos < srec->count; pos++) {
	line += sprintf(line, "%02X", srec->data[pos]);
	sum += srec->data[pos];
    }

    sprintf(line, "%02X\n", (~sum) & 0xff);

    return S_OK;
}
