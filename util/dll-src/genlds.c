/*! \file   genlds.c
    \brief  BrickOS linker script generation
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
 *  Portions created by Markus L. Noga are Copyright (C) 2000
 *  Markus L. Noga. All Rights Reserved.
 *
 *  Contributor(s): Markus L. Noga <markus@noga.de>
 */
 
#include <stdio.h>
#include <time.h>
#include <string.h>

#define MAX_SYMBOLS 65536		//!< max symbols, enough for the RCX
#define MAX_SYMLEN  256			//!< max symbol length. 

//! symbol type.
typedef struct {
  unsigned addr;
  char text[MAX_SYMLEN];
} symbol_t;

//! the list of symbols
symbol_t symbols[MAX_SYMBOLS];

//! read the kernel symbols from a file
static unsigned read_symbols(FILE *f,symbol_t *symbols,unsigned max,unsigned *ram) {
  char buffer[MAX_SYMLEN];
  unsigned i=0;
  
  for(; i<max; ) {
    char symtype;
    
    if(!fgets(buffer,MAX_SYMLEN,f))
      break;
    
    buffer[MAX_SYMLEN-1]=0;
    sscanf(buffer,"%x %c %s",&(symbols[i].addr),&symtype,symbols[i].text);

    // keep global symbols
    //
    if(symtype=='T' || symtype=='D' || symtype=='B') {
      // check for start of user RAM
      //
      if(!strcmp(symbols[i].text,"_mm_start"))
        *ram=symbols[i].addr+2;
      
      // skip special symbols
      //
      if(!strcmp(symbols[i].text,"_main"))
	continue;
      if(!strncmp(symbols[i].text,"___text",7))
	continue;
      if(!strncmp(symbols[i].text,"___data",7))
	continue;
      if(!strncmp(symbols[i].text,"___bss" ,6))
	continue;
      
      i++;
    }
  }
  
  return i;
}


//! print the kernel symbols in linker script format.
static void print_symbols(FILE *f,symbol_t *symbols,unsigned num_symbols) {
  unsigned i;
  for(i=0; i<num_symbols; i++)
    fprintf(f,"    %s = 0x%04x ;\n",symbols[i].text,symbols[i].addr-0x8000);
}


//! print the linker script header.
static void print_header(FILE *f,
		         const char *now,const char *kernel_name,
			 unsigned ram,unsigned kernlen, unsigned ramlen) {
    fprintf(f,
"/*
 * 
 *  dynamic linker command file
 *  generated: %s
 *  kernel   : %s
 *  app start: 0x%04x
 * 
 *  BrickOS for LEGO(R) Mindstorms(TM)
 *  Originally: legOS - the independent LEGO Mindstorms OS
 *              (c) 1999 by Markus L. Noga <markus@noga.de>    
 * 
 */

OUTPUT_FORMAT(\"symbolsrec\")
OUTPUT_ARCH(h8300)
ENTRY(\"_main\")

MEMORY {
  rom   : o = 0x0000, l = 0x8000
  kern  : o = 0x8000, l = 0x%04x
  ram   : o = 0x%04x, l = 0x%04x
  stack : o = 0xfefc, l = 0x0004
  eight : o = 0xff00, l = 0x0100
}

SECTIONS {

  .rom : {
    /* used rom functions */
    
    _rom_reset_vector = 0x0000;
        
    _reset        = 0x03ae ;
    lcd_show      = 0x1b62 ;
    lcd_hide      = 0x1e4a ;
    lcd_number    = 0x1ff2 ;
    lcd_clear     = 0x27ac ;
    power_init    = 0x2964 ;
    sound_system  = 0x299a ;
    power_off     = 0x2a62 ;
    sound_playing = 0x3ccc ;

    _rom_dummy_handler   = 0x046a ;
    _rom_ocia_handler    = 0x04cc ;
    _rom_ocia_return     = 0x04d4 ;
    
  } > rom

  .kernel :	{
    /* kernel symbols (relative to 0x8000) */
",
    now,kernel_name,ram,
    kernlen,ram,ramlen);
}


//! print the linker script footer.
static void print_footer(FILE *f) {
  fprintf(f,
"    /* end of kernel symbols */
  }  > kern
      
  .text BLOCK(2) :	{
    ___text = . ;
    *(.text) 	      /* must start with text for clean entry */			
    *(.rodata)
    *(.strings)
    *(.vectors)       /* vectors region (dummy) */
    *(.persist)

    ___text_end = . ;
  }  > ram

  .tors BLOCK(2) : {
    ___ctors = . ;
    *(.ctors)
    ___ctors_end = . ;
    ___dtors = . ;
    *(.dtors)
    ___dtors_end = . ;
  }  > ram

  .data BLOCK(2) : {
    ___data = . ;
    *(.data)
    *(.tiny)
    ___data_end = . ;
  }  > ram

  .bss BLOCK(2) : {
    ___bss = . ;
    *(.bss)
    *(COMMON)
    ___bss_end = ALIGN(2) ;
  }  >ram

  .stack : {
    _stack = . ; 
    *(.stack)
  }  > topram

  .eight 0xff00: {
    *(.eight)

    /* on-chip module registers (relative to 0xff00) */

    _T_IER = 0x90 ;
    _T_CSR = 0x91 ;
    _T_CNT = 0x92 ;
    _T_OCRA = 0x94 ;
    _T_OCRB = 0x94 ;
    _T_CR = 0x96 ;
    _T_OCR = 0x97 ;
    _T_ICRA = 0x98 ;
    _T_ICRB = 0x9a ;
    _T_ICRC = 0x9c ;
    _T_ICRD = 0x9e ;
    _WDT_CSR = 0xa8 ;
    _WDT_CNT = 0xa9 ;
    _PORT1_PCR = 0xac ;
    _PORT2_PCR = 0xad ;
    _PORT3_PCR = 0xae ;
    _PORT1_DDR = 0xb0 ;
    _PORT2_DDR = 0xb1 ;
    _PORT1 = 0xb2 ;
    _PORT2 = 0xb3 ;
    _PORT3_DDR = 0xb4 ;
    _PORT4_DDR = 0xb5 ;
    _PORT3 = 0xb6 ;
    _PORT4 = 0xb7 ;
    _PORT5_DDR = 0xb8 ;
    _PORT6_DDR = 0xb9 ;
    _PORT5 = 0xba ;
    _PORT6 = 0xbb ;
    _PORT7 = 0xbe ;
    _STCR = 0xc3 ;
    _SYSCR = 0xc4 ;
    _T0_CR = 0xc8 ;
    _T0_CSR = 0xc9 ;
    _T0_CORA = 0xca ;
    _T0_CORB = 0xcb ;
    _T0_CNT = 0xcc ;
    _T1_CR = 0xd0 ;
    _T1_CSR = 0xd1 ;
    _T1_CORA = 0xd2 ;
    _T1_CORB = 0xd3 ;
    _T1_CNT = 0xd4 ;
    _S_MR = 0xd8 ;
    _S_BRR = 0xd9 ;
    _S_CR = 0xda ;
    _S_TDR = 0xdb ;
    _S_SR = 0xdc ;
    _S_RDR = 0xdd ;
    _AD_A = 0xe0 ;
    _AD_A_H = 0xe0 ;
    _AD_A_L = 0xe1 ;
    _AD_B = 0xe2 ;
    _AD_B_H = 0xe2 ;
    _AD_B_L = 0xe3 ;
    _AD_C = 0xe4 ;
    _AD_C_H = 0xe4 ;
    _AD_C_L = 0xe5 ;
    _AD_D = 0xe6 ;
    _AD_D_H = 0xe6 ;
    _AD_D_L = 0xe7 ;
    _AD_CSR = 0xe8 ;
    _AD_CR = 0xe9 ;
    
    /* end of on-chip module registers */
    
  }  > eight

  .stab 0 (NOLOAD) : {
    [ .stab ]
  }

  .stabstr 0 (NOLOAD) : {
    [ .stabstr ]
  }

} /* SECTIONS */
"
    ); 
}


int main(int argc, char *argv[]) {
  const char *kernel_name;
  unsigned num_symbols;
  unsigned ram,kernlen,ramlen;
  time_t now_time;
  char *now;
  
  // determine kernel name
  //
  if(argc<2) {
    fprintf(stderr,"usage: %s kernelname < kernel.map\n",argv[0]);
    return -1;
  }
  kernel_name=argv[1];

  // parse kernel symbols
  //   
  num_symbols=read_symbols(stdin,symbols,MAX_SYMBOLS,&ram);
  
  // calculate kernel and ram size
  //
  kernlen=ram    - 0x8000;
  ramlen =0xfefc - ram;
 
  // create timestamp
  //
  now_time=time(NULL); 
  now     =ctime(&now_time);

  // print linker script
  //
  print_header (stdout,now,kernel_name,ram,kernlen,ramlen);
  print_symbols(stdout,symbols,num_symbols);
  print_footer (stdout);
  
  return 0;
}
