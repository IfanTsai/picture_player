/*
 * date: 2017��3��20��14:58:56
 * author: Ifan Tsai
 * describe: png��ʽͼƬ�Ľ����Լ���ʾ��ز���
 *          
 */

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <png.h>
#include <zlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include "fb_png.h"
#include "debug.h"

#define PNG_BYTES_TO_CHECK   8


// �ж��Ƿ���png��ʽ��ͼƬ������libpng����ĺ�����ʵ��
// �ο��Կ�����Ŀ¼��example.c
int is_png(int fd)
{
	char buf[PNG_BYTES_TO_CHECK] = { 0 };
	read(fd, buf, sizeof(buf));
	return(!png_sig_cmp((png_const_bytep)buf, (png_size_t)0, PNG_BYTES_TO_CHECK));
}

/************** use libjpeg start *************************/
/*
 * ʹ��libpng��Դ������pngͼƬ����
 * �ο���CSDN���ͺͿ�����Ŀ¼��example.c
 * ���͵�ַ:http://blog.csdn.net/liuchao35758600/article/details/7288822
 */
static int read_png_file(pic_info_t *pinfo)
{
	FILE *fp;
	png_structp png_ptr;
	png_infop info_ptr;
	png_bytep* row_pointers;
	char buf[PNG_BYTES_TO_CHECK];

	fp = fopen(pinfo->pathname, "rb");
	if( fp == NULL )
	{
		return -1;
	}

	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0 );
	info_ptr = png_create_info_struct(png_ptr);

	setjmp(png_jmpbuf(png_ptr));

	/* ��ȡPNG_BYTES_TO_CHECK���ֽڵ����� */
	int temp = fread(buf, 1, PNG_BYTES_TO_CHECK, fp);
	/* �����������ݲ�û��PNG_BYTES_TO_CHECK���ֽ� */
	if(temp < PNG_BYTES_TO_CHECK)
	{
		fclose(fp);
		png_destroy_read_struct(&png_ptr, &info_ptr, 0);
				return -1;
	}
	/* ��������Ƿ�ΪPNG��ǩ�� */
	temp = png_sig_cmp((png_bytep)buf, (png_size_t)0, PNG_BYTES_TO_CHECK);
	/* �������PNG��ǩ������˵�����ļ�����PNG�ļ� */
	if(temp != 0)
	{
		fclose(fp);
		png_destroy_read_struct(&png_ptr, &info_ptr, 0);
		return -1;
	}

	/* ��λ�ļ�ָ�� */
	rewind(fp);
	/* ��ʼ���ļ� */
	png_init_io(png_ptr, fp);
	/* ��ȡPNGͼƬ��Ϣ */
	png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_EXPAND, 0);
	
	/* ��ȡͼ���ɫ������ */
	int color_type = png_get_color_type(png_ptr, info_ptr);
	
	/* ��ȡͼ��Ŀ�� */
	long width  = png_get_image_width(png_ptr, info_ptr);
	long height = png_get_image_height(png_ptr, info_ptr);

	// ע��png_get_bit_depth����������ÿ����ɫ��ռ��λ��
	// ����rgbɫ��Ҫ����3
	int  bpp    = png_get_bit_depth(png_ptr, info_ptr);
	bpp *= 3;
	
	/* ��ȡ��ͼƬ�ֱ��ʺ�bpp��Ϊpinfo->buf�����ڴ� */
	long len	= height * width * bpp / 8;
	pinfo->buf	= (unsigned char *)malloc(len);

	/* ��ȡͼ����������������ݣ�row_pointers��߾���rgba���� */
	row_pointers = png_get_rows(png_ptr, info_ptr);

	// ��䴫�ݹ�����pinfo�ṹ��
	pinfo->bpp	  = bpp;
	pinfo->height = height;
	pinfo->width  = width;

	debug("height = %ld\n", height);
	debug("width = %ld\n", width);
	debug("bpp = %d\n", bpp);
	debug("color_type = %d\n", color_type);

	/* ���ݲ�ͬ��ɫ�����ͽ�����Ӧ���� */
	int pos = 0;
	switch(color_type)
	{
		case PNG_COLOR_TYPE_RGB_ALPHA:
			for (int y = 0; y < height; y++)
			{
				for (int x = 0; x < width * 4; x += 4)
				{
					/* ������RGBA���ݣ�����RGBA���� */
					// ����x210�������fb������û���ṩ͸���ȣ�������������͸����
					pinfo->buf[pos++] = row_pointers[y][x+0];	  // red
					pinfo->buf[pos++] = row_pointers[y][x+1];	  // green
					pinfo->buf[pos++] = row_pointers[y][x+2];	  // blue
					//pinfo->buf[pos++] = row_pointers[y][x++];	  // alpha
				}
			}
			break;

		case PNG_COLOR_TYPE_RGB:
			for (int y = 0; y < height; y++)
			{
				for (int x = 0; x < width * 3; x += 3)
				{
					pinfo->buf[pos++] = row_pointers[y][x+0];	  // red
					pinfo->buf[pos++] = row_pointers[y][x+1];	  // green
					pinfo->buf[pos++] = row_pointers[y][x+2];	  // blue
				}
			}
			break;
		/* ����ɫ�����͵�ͼ��Ͳ����� */
		default:
			fclose(fp);
			png_destroy_read_struct(&png_ptr, &info_ptr, 0);
			return -1;
	}
	png_destroy_read_struct(&png_ptr, &info_ptr, 0);
	fclose(fp);
	
	return 0;
}
/************** use libjpeg end *************************/



void show_png(unsigned int *pfb, pic_info_t *pinfo)
{
	#if 0
	int fd = open(pinfo->pathname, O_RDONLY);
	if (fd < 0)
	{
		perror("open jpg");
		return;
	}
	if (is_png(fd))
	{
		debug("the picture is png\n");
	}
	else
	{
		debug("the picture is not png\n");
		close(fd);
		return;
	}
	close(fd);
	#endif
	
	/* ʹ��libpng��������png��ȥ��ʾ */
	read_png_file(pinfo);
	fb_show_picture_png(pfb, pinfo);
	free(pinfo->buf);                 // ��read_png_file(pinfo);�з�����ڴ�
}

