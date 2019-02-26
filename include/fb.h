#ifndef __FB_H__
#define __FB_H__

#define FB_PATH     ("/dev/fb0")

#define WIDTH       (1024)
#define HEIGHT      (600)

#define WHITE       (0xffffff)
#define RED         (0xff0000)
#define GREEN       (0x00ff00)
#define BLUE        (0x0000ff)
#define BLACK       (0x000000)

#define PATHNMAE_MAX_LEN   128

// 图片信息
typedef struct pic_info
{
	char pathname[PATHNMAE_MAX_LEN];       // 路径名
	unsigned int width;                    // 水平像素   
	unsigned int height;                   // 垂直像素
	unsigned int bpp;                      // 位深度
	unsigned char *buf;                    // 图片有效数据
}pic_info_t;


int  fb_open(const char *fb_path, unsigned int **pfb);
void fb_close(unsigned int fd, unsigned int *pfb);
void fb_set_bgc(unsigned int *pfb, unsigned int pixel);
void fb_show_picture_bmp(unsigned int *pfb, const pic_info_t *pic_info);
void fb_show_picture_jpg(unsigned int *pfb, const pic_info_t *pic_info);
inline void fb_show_picture_png(unsigned int *pfb, const pic_info_t *pic_info)
{
	return fb_show_picture_jpg(pfb, pic_info);
}




#endif
