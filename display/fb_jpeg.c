/*
 * date: 2017年3月19日17:51:49
 * author: Ifan Tsai
 * describe: jpg格式图片的解码以及显示相关操作
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


// 判断是否是jpg格式的图片，jpg格式图片以0xffd8开头，0xffd9结尾
int is_jpg(int fd)
{
	unsigned char buf[2] = { 0 };
	read(fd, buf, sizeof(buf));
	if ( !(buf[0] == 0xff && buf[1] == 0xd8) )
	{
		return 0;
	}
	lseek(fd, -2, SEEK_END);      // 将读写指针移动到倒数第二个字节处
	read(fd, buf, sizeof(buf));
	if ( !(buf[0] == 0xff && buf[1] == 0xd9) )
	{
		return 0;
	}

	return 1;
}

/************** use libjpeg start *************************/
/*
 * 使用libjpeg开源库来对jpg图片解码
 * 参考自库代码根目录下example.c
 */
struct my_error_mgr {
  struct jpeg_error_mgr pub;	
  jmp_buf setjmp_buffer;	    
};

typedef struct my_error_mgr * my_error_ptr;

// 错误处理的回调函数
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
	struct jpeg_decompress_struct cinfo;   // 创建解码器
	struct my_error_mgr jerr;
	FILE * infile;		                   // 需要解码的jpg文件指针    
  	//JSAMPARRAY buffer;		          
  	char *buffer;		                   // 存放解码后的一行数据
  	int row_stride;		                   // 存放图像一行的字节数
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
	
	jpeg_create_decompress(&cinfo);            // 初始化解码器
	
	jpeg_stdio_src(&cinfo, infile);            // 将解码器与jpg文件绑定

    jpeg_read_header(&cinfo, TRUE);            // 读取头信息

    jpeg_start_decompress(&cinfo);             // 开始解码
    
    // 计算图像一行的字节数
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

	// 一行一行解码图像，并将一行的数据拷贝到传递过来的pic_info_t结构体中
    while (cinfo.output_scanline < cinfo.output_height) 
	{
	    //jpeg_read_scanlines(&cinfo, buffer, 1);  
	    jpeg_read_scanlines(&cinfo, &buffer, 1);
		memcpy(pinfo->buf + (cinfo.output_scanline-1) * row_stride,  \
			buffer, row_stride);
    }

	// 填充传递过来的pinfo结构体
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

	/* 使用libjpeg库来解码jpg后去显示 */
	read_JPEG_file(pinfo);
	fb_show_picture_jpg(pfb, pinfo);
	free(pinfo->buf);               // 在read_JPEG_file(pinfo);中分配的内存
}




















