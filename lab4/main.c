#include <stdio.h>

#include "../common/common.h"


#define COLOR_BACKGROUND FB_COLOR(0xff, 0xff, 0xff)
#define RED FB_COLOR(255, 0, 0)
#define ORANGE FB_COLOR(255, 165, 0)
#define YELLOW FB_COLOR(255, 255, 0)
#define GREEN FB_COLOR(0, 255, 0)
#define CYAN FB_COLOR(0, 127, 255)
#define BLUE FB_COLOR(0, 0, 255)
#define PURPLE FB_COLOR(139, 0, 255)
#define WHITE FB_COLOR(255, 255, 255)
#define BLACK FB_COLOR(0, 0, 0)
#define GRAY FB_COLOR(125, 125, 125)
#define MAX_BTN_CNT 1024
#define FONT_SIZE 30

typedef void (*FunCallback)(void);

enum BTN_STATUS
{
	PRESS = 0,
	UP
};

enum ALIGN
{
	BEGIN = 0,
	CENTER,
	END
};

typedef struct
{
	int x, y, w, h, color, pColor;
	char *text;
	int tx, ty;
	int x_align, y_align;
	int status;
	int fontSize;
	int fontColor;
	FunCallback onClick;
	FunCallback onPress;
	FunCallback onUp;
} Button;
Button *buttons[MAX_BTN_CNT];
int btnCnt = 0;
Button *clearBtn;

void renderButton(Button *btn, int color)
{
	fb_draw_rect(btn->x, btn->y, btn->w, btn->h, color);
	if (btn->text)
	{
		if (btn->tx == -1000)
		{
			ipair xy = fb_draw_text(-1000, -1000, btn->text, btn->fontSize, BLACK);
			switch (btn->x_align)
			{
			case BEGIN:
				btn->tx = btn->x;
				break;
			case CENTER:
				btn->tx = btn->x + btn->w / 2 - xy.x / 2;
				break;
			case END:
				btn->tx = btn->x + btn->w - xy.x;
			}
			switch (btn->y_align)
			{
			case BEGIN:
				btn->ty = btn->y + xy.y;
				break;
			case CENTER:
				btn->ty = btn->y + btn->h / 2 + xy.y / 2;
				break;
			case END:
				btn->ty = btn->y + btn->h;
			}
			//printf("text %d %d \n", btn->tx, btn->ty);
		}
		fb_draw_text(btn->tx, btn->ty, btn->text, btn->fontSize, BLACK);
	}
	fb_update();
}

void fressButton(int press)
{
	if (press)
	{
		for (int i = 0; i < btnCnt; i++)
		{
			renderButton(buttons[i], buttons[i]->pColor);
		}
	}
	else
	{
		for (int i = 0; i < btnCnt; i++)
		{
			renderButton(buttons[i], buttons[i]->color);
		}
	}
}

int initButton(Button *btn)
{
	if (btnCnt < MAX_BTN_CNT)
	{
		buttons[btnCnt++] = btn;
		btn->status = UP;
		btn->text = NULL;
		btn->onClick = (FunCallback)NULL;
		btn->onPress = (FunCallback)NULL;
		btn->onUp = (FunCallback)NULL;
		btn->tx = btn->ty = -1000;
		btn->x_align = btn->y_align = CENTER;
		btn->fontSize = 30;
		btn->fontColor = BLACK;
	}
	else
	{
		return -1;
	}
	return 0;
}

int inRect(int x, int y, int w, int h, int cx, int cy)
{
	return cx >= x && cx < x + w && cy >= y && cy < y + h;
}

void onPress(int x, int y)
{
	for (int i = 0; i < btnCnt; i++)
	{
		Button *btn = buttons[i];
		if (!inRect(btn->x, btn->y, btn->w, btn->h, x, y))
			continue;
		if (btn->status == UP)
		{
			btn->status = PRESS;
			if (btn->onPress)
			{

				btn->onPress();
			}

			renderButton(btn, btn->pColor);
		}
	}
}

void onRelease(int x, int y)
{
	for (int i = 0; i < btnCnt; i++)
	{
		Button *btn = buttons[i];
		if (!inRect(btn->x, btn->y, btn->w, btn->h, x, y))
			continue;
		if (btn->status == PRESS)
		{
			btn->status = UP;
			if (btn->onUp)
			{
				btn->onUp();
			}
			if (btn->onClick)
			{
				btn->onClick();
			}

			renderButton(btn, btn->color);
		}
	}
}

void clearScreen()
{
	fb_draw_rect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, COLOR_BACKGROUND);
	fb_update();
}

unsigned int color[10] = {RED, ORANGE, YELLOW, GREEN, CYAN, BLUE, PURPLE, WHITE, BLACK, GRAY};
int lastx[10], lasty[10];
static int touch_fd;
static void touch_event_cb(int fd)
{
	int type, x, y, finger;
	int radius = 5;
	type = touch_read(fd, &x, &y, &finger);
	switch (type)
	{
	case TOUCH_PRESS:
		printf("TOUCH_PRESS：x=%d,y=%d,finger=%d\n", x, y, finger);
		onPress(x, y);
		if (clearBtn->status != PRESS)
		{
			fb_draw_circle(x, y, radius, color[finger]);
			lastx[finger] = x;
			lasty[finger] = y;
		}
		break;
	case TOUCH_MOVE:
		printf("TOUCH_MOVE：x=%d,y=%d,finger=%d\n", x, y, finger);
		if (clearBtn->status != PRESS)
		{
			fb_draw_bold_line(lastx[finger], lasty[finger], x, y, radius, color[finger], 0);
			lastx[finger] = x;
			lasty[finger] = y;
		}
		break;
	case TOUCH_RELEASE:
		printf("TOUCH_RELEASE：x=%d,y=%d,finger=%d\n", x, y, finger);
		onRelease(x, y);
		break;
	case TOUCH_ERROR:
		printf("close touch fd\n");
		close(fd);
		task_delete_file(fd);
		break;
	default:
		return;
	}
	fb_update();
	return;
}

void init()
{

	// for (int i = 0; i < 10; i++)
	// {
	// 	lastx[i] = lasty[i] = -1;
	// }
	clearBtn = (Button*)malloc(sizeof(Button));
	initButton(clearBtn);

	clearBtn->x = 10;
	clearBtn->y = 10;
	clearBtn->w = 100;
	clearBtn->h = 50;
	clearBtn->color = RED;
	clearBtn->pColor = FB_COLOR(100, 0, 0);
	clearBtn->text = "Clear";
	clearBtn->onPress = clearScreen;
	fressButton(0);
}

int main(int argc, char *argv[])
{
	fb_init("/dev/fb0");
	font_init("./font.ttc");
	fb_draw_rect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, COLOR_BACKGROUND);
	fb_update();

	init();

	// 打开多点触摸设备文件, 返回文件fd
	touch_fd = touch_init("/dev/input/event1");
	// 添加任务, 当touch_fd文件可读时, 会自动调用touch_event_cb函数
	task_add_file(touch_fd, touch_event_cb);

	task_loop(); // 进入任务循环
	return 0;
}
