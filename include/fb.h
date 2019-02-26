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

// ͼƬ��Ϣ
typedef struct pic_info
{
	char pathname[PATHNMAE_MAX_LEN];       // ·����
	unsigned int width;                    // ˮƽ����   
	unsigned int height;                   // ��ֱ����
	unsigned int bpp;                      // λ���
	unsigned char *buf;                    // ͼƬ��Ч����
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
