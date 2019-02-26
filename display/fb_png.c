/*
 * date: 2017年3月20日14:58:56
 * author: Ifan Tsai
 * describe: png格式图片的解码以及显示相关操作
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


// 判断是否是png格式的图片，调用libpng库里的函数来实现
// 参考自库代码根目录下example.c
int is_png(int fd)
{
	char buf[PNG_BYTES_TO_CHECK] = { 0 };
	read(fd, buf, sizeof(buf));
	return(!png_sig_cmp((png_const_bytep)buf, (png_size_t)0, PNG_BYTES_TO_CHECK));
}

/************** use libjpeg start *************************/
/*
 * 使用libpng开源库来对png图片解码
 * 参考自CSDN博客和库代码根目录下example.c
 * 博客地址:http://blog.csdn.net/liuchao35758600/article/details/7288822
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

	/* 读取PNG_BYTES_TO_CHECK个字节的数据 */
	int temp = fread(buf, 1, PNG_BYTES_TO_CHECK, fp);
	/* 若读到的数据并没有PNG_BYTES_TO_CHECK个字节 */
	if(temp < PNG_BYTES_TO_CHECK)
	{
		fclose(fp);
		png_destroy_read_struct(&png_ptr, &info_ptr, 0);
				return -1;
	}
	/* 检测数据是否为PNG的签名 */
	temp = png_sig_cmp((png_bytep)buf, (png_size_t)0, PNG_BYTES_TO_CHECK);
	/* 如果不是PNG的签名，则说明该文件不是PNG文件 */
	if(temp != 0)
	{
		fclose(fp);
		png_destroy_read_struct(&png_ptr, &info_ptr, 0);
		return -1;
	}

	/* 复位文件指针 */
	rewind(fp);
	/* 开始读文件 */
	png_init_io(png_ptr, fp);
	/* 读取PNG图片信息 */
	png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_EXPAND, 0);
	
	/* 获取图像的色彩类型 */
	int color_type = png_get_color_type(png_ptr, info_ptr);
	
	/* 获取图像的宽高 */
	long width  = png_get_image_width(png_ptr, info_ptr);
	long height = png_get_image_height(png_ptr, info_ptr);

	// 注意png_get_bit_depth读出来的是每个颜色所占的位数
	// 所以rgb色需要乘以3
	int  bpp    = png_get_bit_depth(png_ptr, info_ptr);
	bpp *= 3;
	
	/* 获取完图片分辨率和bpp后，为pinfo->buf分配内存 */
	long len	= height * width * bpp / 8;
	pinfo->buf	= (unsigned char *)malloc(len);

	/* 获取图像的所有行像素数据，row_pointers里边就是rgba数据 */
	row_pointers = png_get_rows(png_ptr, info_ptr);

	// 填充传递过来的pinfo结构体
	pinfo->bpp	  = bpp;
	pinfo->height = height;
	pinfo->width  = width;

	debug("height = %ld\n", height);
	debug("width = %ld\n", width);
	debug("bpp = %d\n", bpp);
	debug("color_type = %d\n", color_type);

	/* 根据不同的色彩类型进行相应处理 */
	int pos = 0;
	switch(color_type)
	{
		case PNG_COLOR_TYPE_RGB_ALPHA:
			for (int y = 0; y < height; y++)
			{
				for (int x = 0; x < width * 4; x += 4)
				{
					/* 以下是RGBA数据，保存RGBA数据 */
					// 由于x210开发板的fb驱动并没有提供透明度，所以这里舍弃透明度
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
		/* 其它色彩类型的图像就不读了 */
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
	
	/* 使用libpng库来解码png后去显示 */
	read_png_file(pinfo);
	fb_show_picture_png(pfb, pinfo);
	free(pinfo->buf);                 // 在read_png_file(pinfo);中分配的内存
}

