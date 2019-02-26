/*
 * date: 2017��3��19��17:51:49
 * author: Ifan Tsai
 * describe: jpg��ʽͼƬ�Ľ����Լ���ʾ��ز���
 *          
 */

#include <stdio.h>
#include <setjmp.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include "jpeglib.h"
#include "fb_jpeg.h"
#include "fb.h"
#include "debug.h"


// �ж��Ƿ���jpg��ʽ��ͼƬ��jpg��ʽͼƬ��0xffd8��ͷ��0xffd9��β
int is_jpg(int fd)
{
	unsigned char buf[2] = { 0 };
	read(fd, buf, sizeof(buf));
	if ( !(buf[0] == 0xff && buf[1] == 0xd8) )
	{
		return 0;
	}
	lseek(fd, -2, SEEK_END);      // ����дָ���ƶ��������ڶ����ֽڴ�
	read(fd, buf, sizeof(buf));
	if ( !(buf[0] == 0xff && buf[1] == 0xd9) )
	{
		return 0;
	}

	return 1;
}

/************** use libjpeg start *************************/
/*
 * ʹ��libjpeg��Դ������jpgͼƬ����
 * �ο��Կ�����Ŀ¼��example.c
 */
struct my_error_mgr {
  struct jpeg_error_mgr pub;	
  jmp_buf setjmp_buffer;	    
};

typedef struct my_error_mgr * my_error_ptr;

// ������Ļص�����
METHODDEF(void)
my_error_exit (j_common_ptr cinfo)
{
	my_error_ptr myerr = (my_error_ptr) cinfo->err;

	(*cinfo->err->output_message) (cinfo);

	longjmp(myerr->setjmp_buffer, 1);
}


static GLOBAL(int)
read_JPEG_file (pic_info_t *pinfo)
{
	struct jpeg_decompress_struct cinfo;   // ����������
	struct my_error_mgr jerr;
	FILE * infile;		                   // ��Ҫ�����jpg�ļ�ָ��    
  	//JSAMPARRAY buffer;		          
  	char *buffer;		                   // ��Ž�����һ������
  	int row_stride;		                   // ���ͼ��һ�е��ֽ���
	if ((infile = fopen(pinfo->pathname, "rb")) == NULL) 
	{
	    fprintf(stderr, "can't open %s\n", pinfo->pathname);
	    return -1;
    }
	cinfo.err = jpeg_std_error(&jerr.pub);  
    jerr.pub.error_exit = my_error_exit;
	if (setjmp(jerr.setjmp_buffer)) 
	{
	    jpeg_destroy_decompress(&cinfo);
	    fclose(infile);
	    return -1;
    }
	
	jpeg_create_decompress(&cinfo);            // ��ʼ��������
	
	jpeg_stdio_src(&cinfo, infile);            // ����������jpg�ļ���

    jpeg_read_header(&cinfo, TRUE);            // ��ȡͷ��Ϣ

    jpeg_start_decompress(&cinfo);             // ��ʼ����
    
    // ����ͼ��һ�е��ֽ���
    row_stride = cinfo.output_width * cinfo.output_components;

	debug("the picture width is %d\n", cinfo.output_width);
	debug("the picture height is %d\n", cinfo.output_height);
	debug("the picture bpp is %d\n", cinfo.output_components * 8);
  
    //buffer = (*cinfo.mem->alloc_sarray)
	//	((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);  
	buffer = (char *)malloc(row_stride);
	
	long height = cinfo.output_height;
	long width  = cinfo.output_width;
	int  bpp    = cinfo.output_components * 8;
	long len    = height * width * bpp / 8;
	pinfo->buf  = (unsigned char *)malloc(len);

	// һ��һ�н���ͼ�񣬲���һ�е����ݿ��������ݹ�����pic_info_t�ṹ����
    while (cinfo.output_scanline < cinfo.output_height) 
	{
	    //jpeg_read_scanlines(&cinfo, buffer, 1);  
	    jpeg_read_scanlines(&cinfo, &buffer, 1);
		memcpy(pinfo->buf + (cinfo.output_scanline-1) * row_stride,  \
			buffer, row_stride);
    }

	// ��䴫�ݹ�����pinfo�ṹ��
	pinfo->bpp    = bpp;
	pinfo->height = height;
	pinfo->width  = width;
 
    jpeg_finish_decompress(&cinfo);
  
    jpeg_destroy_decompress(&cinfo);

	fclose(infile);

	return 0;
}
/************** use libjpeg end *************************/


void show_jpg(unsigned int *pfb, pic_info_t *pinfo)
{
	#if 0
	int fd = open(pinfo->pathname, O_RDONLY);
	if (fd < 0)
	{
		perror("open jpg");
		return;
	}
	if (is_jpg(fd))
	{
		debug("the picture is jpg\n");
	}
	else
	{
		debug("the picture is not jpg\n");
		close(fd);
		return;
	}
	close(fd);
	#endif

	/* ʹ��libjpeg��������jpg��ȥ��ʾ */
	read_JPEG_file(pinfo);
	fb_show_picture_jpg(pfb, pinfo);
	free(pinfo->buf);               // ��read_JPEG_file(pinfo);�з�����ڴ�
}




















