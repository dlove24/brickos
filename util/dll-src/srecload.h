#ifndef __srecload_h__
#define __srecload_h__


//! wrapper for an s-record image
typedef struct {
  unsigned short base;        	  //!< base address
  unsigned short text_size;	  //!< length from base address
  unsigned short data_size;
  unsigned short bss_size;
  unsigned short offset;      	  //!< start offset from base address
  
  unsigned char *text;	      	  //!< text at base address
} image_t;


//! load an RCX s-record file and return its image
/*! \param filename file to load
    \param img      image to store to
*/
void image_load(image_t *img,const char *filename);

#endif
