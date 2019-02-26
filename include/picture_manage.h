#ifndef __PICTURE_MANAGE__
#define __PICTURE_MANAGE__

#include "fb.h"

enum pic_type               
{
	PIC_TYPE_BMP,
	PIC_TYPE_JPG,
	PIC_TYPE_PNG
};

// 管理文件夹上的图片的链表
typedef struct picture_info_list
{
	enum pic_type type;
	pic_info_t pic_info;
	void (*show)(unsigned int *pfb, pic_info_t *pinfo);
	struct picture_info_list *pnext;
	struct picture_info_list *prev;
}pic_info_list_t;


void scan_picture(pic_info_list_t *pic_list, const char *path);
void be_pic_info_loop_list(pic_info_list_t *pictures);


















#endif