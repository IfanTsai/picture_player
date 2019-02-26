#ifndef __FB_BMP_H__
#define __FB_BMP_H__

#include "fb.h"

// BMP 文件头
typedef struct
{
//    unsigned short    bfType;  			
    unsigned long    	bfSize; 			
    unsigned short    	bfReserved1;
    unsigned short    	bfReserved2;
    unsigned long    	bfOffBits;
} ClBitMapFileHeader;

// BMP 信息头
typedef struct
{
    unsigned long  		biSize;
    long   				biWidth;
    long   				biHeight;
    unsigned short   	biPlanes;
    unsigned short   	biBitCount;
    unsigned long 	 	biCompression;
    unsigned long  		biSizeImage;
    long   				biXPelsPerMeter;
    long   				biYPelsPerMeter;
    unsigned long  	 	biClrUsed;
    unsigned long   	biClrImportant;
} ClBitMapInfoHeader;


int is_bmp(int fd);
void show_bmp(unsigned int *pfb, pic_info_t *pinfo);








#endif
