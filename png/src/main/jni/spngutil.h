
#ifndef _SPNGUTIL_H
#define _SPNGUTIL_H

void* spng_encode(size_t* size,FILE* fp,void *buff,const liq_palette *pal,unsigned int w ,unsigned int h,unsigned int color);
void *spng_decode(const char *file,unsigned int *width, unsigned int *height);

#endif
