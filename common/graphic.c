#include "common.h"
#include <sys/ioctl.h>
#include <linux/fb.h>
#include <linux/kd.h>
#include <linux/vt.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/mman.h>
#include <string.h>
#include <math.h>
#include <string.h>

static int LCD_FB_FD;
static int *LCD_FB_BUF = NULL;
static int DRAW_BUF[SCREEN_WIDTH * SCREEN_HEIGHT];

static struct area
{
	int x1, x2, y1, y2;
} update_area = {0, 0, 0, 0};

#define AREA_SET_EMPTY(pa)        \
	do                            \
	{                             \
		(pa)->x1 = SCREEN_WIDTH;  \
		(pa)->x2 = 0;             \
		(pa)->y1 = SCREEN_HEIGHT; \
		(pa)->y2 = 0;             \
	} while (0)

void fb_init(char *dev)
{
	int fd;
	struct fb_fix_screeninfo fb_fix;
	struct fb_var_screeninfo fb_var;

	if (LCD_FB_BUF != NULL)
		return; /*already done*/

	// 进入终端图形模式
	fd = open("/dev/tty0", O_RDWR, 0);
	ioctl(fd, KDSETMODE, KD_GRAPHICS);
	close(fd);

	// First: Open the device
	if ((fd = open(dev, O_RDWR)) < 0)
	{
		printf("Unable to open framebuffer %s, errno = %d\n", dev, errno);
		return;
	}
	if (ioctl(fd, FBIOGET_FSCREENINFO, &fb_fix) < 0)
	{
		printf("Unable to FBIOGET_FSCREENINFO %s\n", dev);
		return;
	}
	if (ioctl(fd, FBIOGET_VSCREENINFO, &fb_var) < 0)
	{
		printf("Unable to FBIOGET_VSCREENINFO %s\n", dev);
		return;
	}

	printf("framebuffer info: bits_per_pixel=%u,size=(%d,%d),virtual_pos_size=(%d,%d)(%d,%d),line_length=%u,smem_len=%u\n",
		   fb_var.bits_per_pixel, fb_var.xres, fb_var.yres, fb_var.xoffset, fb_var.yoffset,
		   fb_var.xres_virtual, fb_var.yres_virtual, fb_fix.line_length, fb_fix.smem_len);

	// Second: mmap
	void *addr = mmap(NULL, fb_fix.smem_len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (addr == (void *)-1)
	{
		printf("failed to mmap memory for framebuffer.\n");
		return;
	}

	if ((fb_var.xoffset != 0) || (fb_var.yoffset != 0))
	{
		fb_var.xoffset = 0;
		fb_var.yoffset = 0;
		if (ioctl(fd, FBIOPAN_DISPLAY, &fb_var) < 0)
		{
			printf("FBIOPAN_DISPLAY framebuffer failed\n");
		}
	}

	LCD_FB_FD = fd;
	LCD_FB_BUF = addr;

	// set empty
	AREA_SET_EMPTY(&update_area);
	return;
}

static void _copy_area(int *dst, int *src, struct area *pa)
{
	int x, y, w, h;
	x = pa->x1;
	w = pa->x2 - x;
	y = pa->y1;
	h = pa->y2 - y;
	src += y * SCREEN_WIDTH + x;
	dst += y * SCREEN_WIDTH + x;
	while (h-- > 0)
	{
		memcpy(dst, src, w * 4);
		src += SCREEN_WIDTH;
		dst += SCREEN_WIDTH;
	}
}

static int _check_area(struct area *pa)
{
	if (pa->x2 == 0)
		return 0; // is empty

	if (pa->x1 < 0)
		pa->x1 = 0;
	if (pa->x2 > SCREEN_WIDTH)
		pa->x2 = SCREEN_WIDTH;
	if (pa->y1 < 0)
		pa->y1 = 0;
	if (pa->y2 > SCREEN_HEIGHT)
		pa->y2 = SCREEN_HEIGHT;

	if ((pa->x2 > pa->x1) && (pa->y2 > pa->y1))
		return 1; // no empty

	// set empty
	AREA_SET_EMPTY(pa);
	return 0;
}

void fb_update(void)
{
	if (_check_area(&update_area) == 0)
		return; // is empty
	_copy_area(LCD_FB_BUF, DRAW_BUF, &update_area);
	AREA_SET_EMPTY(&update_area); // set empty
	return;
}

/*======================================================================*/

static void *_begin_draw(int x, int y, int w, int h)
{
	int x2 = x + w;
	int y2 = y + h;
	if (update_area.x1 > x)
		update_area.x1 = x;
	if (update_area.y1 > y)
		update_area.y1 = y;
	if (update_area.x2 < x2)
		update_area.x2 = x2;
	if (update_area.y2 < y2)
		update_area.y2 = y2;
	return DRAW_BUF;
}

void fb_draw_buffer(int *src)
{
	int *buf = _begin_draw(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	memcpy(buf,src,SCREEN_WIDTH*SCREEN_HEIGHT*sizeof(int));
}

void fb_draw_pixel(int x, int y, int color)
{
	if (x < 0 || y < 0 || x >= SCREEN_WIDTH || y >= SCREEN_HEIGHT)
		return;
	int *buf = _begin_draw(x, y, 1, 1);
	/*---------------------------------------------------*/
	*(buf + y * SCREEN_WIDTH + x) = color;
	/*---------------------------------------------------*/
	return;
}

void fb_draw_bold_line(int x1, int y1, int x2, int y2, int r, int color, int fillet)
{
	/*---------------------------------------------------*/
	int dx = x2 - x1;
	int dy = y2 - y1;
	int ux = ((dx > 0) << 1) - 1;
	int uy = ((dy > 0) << 1) - 1;
	int x = x1, y = y1, eps;
	eps = 0;
	dx = abs(dx);
	dy = abs(dy);
	if (dx > dy)
	{
		for (x = x1; x != x2; x += ux)
		{
			fb_draw_circle(x, y,r, color);
			eps += dy;
			if ((eps << 1) >= dx)
			{
				y += uy;
				eps -= dx;
			}
		}
	}
	else
	{
		for (y = y1; y != y2; y += uy)
		{
			fb_draw_circle(x, y,r, color);
			eps += dx;
			if ((eps << 1) >= dy)
			{
				x += ux;
				eps -= dy;
			}
		}
	}
	fb_draw_circle(x, y,r, color);

	/*---------------------------------------------------*/
	return;
	// double dx, dy;
	// int usx = 0; //是否以x为步长
	// int len;
	// if (y1 == y2)
	// {
	// 	len = r;
	// 	dx = 0;
	// 	//dy = y1 < y2 ? 1 : -1;
	// }
	// else
	// {
	// 	double k = (y2 - y1) / (1.0 * x2 - x1);
	// 	k = -1 / k;
	// 	if (abs(k) > 1)
	// 	{
	// 		len = abs(r*sin(atan(k)));
	// 		//dy = y1 < y2 ? 1 : -1;
	// 		dx = 1 / k;
	// 	}
	// 	else
	// 	{
	// 		len = abs(r*cos(atan(k)));
	// 		//dx = x1 < x2 ? 1 : -1;
	// 		dy = k;
	// 		usx = 1;
	// 	}
	// }
	// if(usx) {
	// 	for(int i=-len;i<len;i++) {
	// 		int xx1 = x1+i,xx2 = x2+i;
	// 		int yy1 = y1+i*dy,yy2 = y2+i*dy;
	// 		fb_draw_line(xx1,yy1,xx2,yy2,color);
	// 		fb_draw_line(xx1,yy1+1,xx2,yy2+1,color);
	// 		fb_draw_line(xx1,yy1-1,xx2,yy2+1,color);
	// 		fb_draw_line(xx1,yy1+1,xx2,yy2-1,color);
	// 	}
	// } else {
	// 	for(int i=-len;i<len;i++) {
	// 		int yy1 = y1+i,yy2 = y2+i;
	// 		int xx1 = x1+i*dx,xx2 = x2+i*dx;
	// 		fb_draw_line(xx1,yy1,xx2,yy2,color);
	// 		fb_draw_line(xx1+1,yy1,xx2+1,yy2,color);
	// 		fb_draw_line(xx1+1,yy1,xx2-1,yy2,color);
	// 		fb_draw_line(xx1-1,yy1,xx2+1,yy2,color);
	// 	}
	// }
	

	// for(int yy = y-r;yy<(int)fmin(y+r,SCREEN_HEIGHT);yy++) {
	// 	if(yy<0) continue;
	// 	int len = sqrt(r*r-abs(yy-y)*abs(yy-y));
	// 	for(int xx = fmax(x - len,0);xx<(int)fmin(x+len,SCREEN_WIDTH);xx++) {
	// 		fb_draw_pixel(xx,yy,color);
	// 	}
	// }
}

void fb_draw_circle(int x, int y, int r, int color)
{
	for (int yy = y - r; yy < (int)fmin(y + r, SCREEN_HEIGHT); yy++)
	{
		if (yy < 0)
			continue;
		int len = sqrt(r * r - abs(yy - y) * abs(yy - y));
		for (int xx = fmax(x - len, 0); xx < (int)fmin(x + len, SCREEN_WIDTH); xx++)
		{
			fb_draw_pixel(xx, yy, color);
		}
	}
}

void fb_draw_rect(int x, int y, int w, int h, int color)
{
	if (x < 0)
	{
		w += x;
		x = 0;
	}
	if (x + w > SCREEN_WIDTH)
	{
		w = SCREEN_WIDTH - x;
	}
	if (y < 0)
	{
		h += y;
		y = 0;
	}
	if (y + h > SCREEN_HEIGHT)
	{
		h = SCREEN_HEIGHT - y;
	}
	if (w <= 0 || h <= 0)
		return;
	int *buf = _begin_draw(x, y, w, h);
	/*---------------------------------------------------*/
	for (int i = 0,yy=y*SCREEN_WIDTH; i < h; i++,yy+=SCREEN_WIDTH)
	{
		for (int j = 0; j < w; j++)
		{
			*(buf + yy + (x + j)) = color;
		}
	}

	/*---------------------------------------------------*/
	return;
}

void fb_draw_line(int x1, int y1, int x2, int y2, int color)
{
	/*---------------------------------------------------*/
	int dx = x2 - x1;
	int dy = y2 - y1;
	int ux = ((dx > 0) << 1) - 1;
	int uy = ((dy > 0) << 1) - 1;
	int x = x1, y = y1, eps;
	eps = 0;
	dx = abs(dx);
	dy = abs(dy);
	int *buf = _begin_draw(x1<x2?x1:x2, y1<y2?y1:y2, dx, dy);
	uy *=SCREEN_WIDTH;
	y*=SCREEN_WIDTH;
	if (dx > dy)
	{
		for (x = x1; x != x2; x += ux)
		{
			*(buf + y + x) = color;
			eps += dy;
			if ((eps << 1) >= dx)
			{
				y += uy;
				eps -= dx;
			}
		}
	}
	else
	{
		y2*=SCREEN_WIDTH;
		for (y = y1*SCREEN_WIDTH; y != y2; y += uy)
		{
			*(buf + y + x) = color;
			eps += dx;
			if ((eps << 1) >= dy)
			{
				x += ux;
				eps -= dy;
			}
		}
	}
	*(buf + y + x) = color;

	/*---------------------------------------------------*/
	return;
}

void fb_draw_image(int x, int y, fb_image *image, int color)
{
	if (image == NULL)
		return;

	int ix = 0;				// image x
	int iy = 0;				// image y
	int w = image->pixel_w; // draw width
	int h = image->pixel_h; // draw height

	if (x < 0)
	{
		w += x;
		ix -= x;
		x = 0;
	}
	if (y < 0)
	{
		h += y;
		iy -= y;
		y = 0;
	}

	if (x + w > SCREEN_WIDTH)
	{
		w = SCREEN_WIDTH - x;
	}
	if (y + h > SCREEN_HEIGHT)
	{
		h = SCREEN_HEIGHT - y;
	}
	if ((w <= 0) || (h <= 0))
		return;

	int *buf = _begin_draw(x, y, w, h);
	/*---------------------------------------------------------------*/
	char *dst = (char *)(buf + y * SCREEN_WIDTH + x);
	char *src = image->content; // 不同的图像颜色格式定位不同
	/*---------------------------------------------------------------*/

	int alpha;
	int ww;
	int iw = image->pixel_w; // draw width
	int ih = image->pixel_h; // draw height
	int ixx = ix;
	int xx = x;
	int len = w*4;

	if (image->color_type == FB_COLOR_RGB_8880) /*lab3: jpg*/
	{
		int iyy = 0;
		int yy = y*SCREEN_WIDTH;
		for (int i = 0; i < h; i++, iyy+=iw,yy+=SCREEN_WIDTH)
		{
			// for (int j = 0, x = xx, ix = ixx; j < w; j++, ix++, x++)
			// {
			// 	unsigned int imgColor = *((int *)src + (iyy + ix));
			// 	*(buf + (yy + x)) = imgColor;
			// }
			memcpy(buf+yy+xx,((int*)src+iyy+ixx),len);
		}
		return;
	}
	else if (image->color_type == FB_COLOR_RGBA_8888) /*lab3: png*/
	{
		int iyy = 0;
		int yy = y*SCREEN_WIDTH;
		for (int i = 0; i < h; i++, iyy+=iw,yy+=SCREEN_WIDTH)
		{
			for (int j = 0, x = xx, ix = ixx; j < w; j++, ix++, x++)
			{
				unsigned int imgColor = *((int *)src + (iyy + ix));
				alpha = imgColor >> 24;
				unsigned int newColor = *(buf + (yy + x));
				switch (alpha)
				{
				case 0:
					break;
				case 255:
					newColor = imgColor;
					break;
				default:
				{
					unsigned int b = newColor & 0xff;
					unsigned int g = (newColor >> 8) & 0xff;
					unsigned int r = (newColor >> 16) & 0xff;
					b += ((((imgColor & 0xff) - b) * alpha) >> 8);
					g += ((((imgColor >> 8 & 0xff) - g) * alpha) >> 8);
					r += ((((imgColor >> 16 & 0xff) - r) * alpha) >> 8);
					newColor = (r << 16) | (g << 8) | b;
					break;
				}
				}
				*(buf + (yy + x)) = newColor;
			}
		}
		return;
	}
	else if (image->color_type == FB_COLOR_ALPHA_8) /*lab3: font*/
	{
		int iyy = 0;
		int yy = y*SCREEN_WIDTH;
		for (int i = 0; i < h; i++, iyy+=iw,yy+=SCREEN_WIDTH)
		{
			for (int j = 0, x = xx, ix = ixx; j < w; j++, ix++, x++)
			{
				unsigned int imgColor = color;
				alpha = *(src + (iyy + ix));
				unsigned int newColor = *(buf + (yy + x));
				switch (alpha)
				{
				case 0:
					break;
				case 255:
					newColor = imgColor;
					break;
				default:
				{
					unsigned int b = newColor & 0xff;
					unsigned int g = (newColor >> 8) & 0xff;
					unsigned int r = (newColor >> 16) & 0xff;
					b += ((((imgColor & 0xff) - b) * alpha) >> 8);
					g += ((((imgColor >> 8 & 0xff) - g) * alpha) >> 8);
					r += ((((imgColor >> 16 & 0xff) - r) * alpha) >> 8);
					newColor = (r << 16) | (g << 8) | b;
					break;
				}
				}
				*(buf + (yy + x)) = newColor;
			}
		}
		return;
	}
	/*---------------------------------------------------------------*/
	return;
}

void fb_draw_border(int x, int y, int w, int h, int color)
{
	if (w <= 0 || h <= 0)
		return;
	fb_draw_rect(x, y, w, 1, color);
	if (h > 1)
	{
		fb_draw_rect(x, y + h - 1, w, 1, color);
		fb_draw_rect(x, y + 1, 1, h - 2, color);
		if (w > 1)
			fb_draw_rect(x + w - 1, y + 1, 1, h - 2, color);
	}
}

/** draw a text string **/
ipair fb_draw_text(int x, int y, char *text, int font_size, int color)
{
	fb_image *img;
	fb_font_info info;
	int i = 0;
	int len = strlen(text);
	int w = 0;
	int h = 0;
	while (i < len)
	{
		img = fb_read_font_image(text + i, font_size, &info);
		if (img == NULL)
			break;
		fb_draw_image(x + info.left, y - info.top, img, color);
		w += info.advance_x;
		h = h<img->pixel_h?img->pixel_h:h;
		
		fb_free_image(img);

		x += info.advance_x;
		i += info.bytes;
	}
	return (ipair){w,h};
}
