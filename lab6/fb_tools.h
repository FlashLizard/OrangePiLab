#ifndef FB_TOOLS_H
#define FB_TOOLS_H
#include <future>
#include <queue>

#include "fls_engine/engine.h"

extern "C"
{
#include "../common/common.h"
}

using namespace std;
using namespace fls_engine;

void all_fb_init();
// handle fb.h
Texture readJPG(char *path);
void update_fb_screen(int *src);

extern std::function<void(int)> cFun;
static void touch_cb(int fd)
{
    cFun(fd);
}

struct TouchInfo
{
    int type, x, y, finger;
};
class FbUI;
class FbButton;

class FbUI : public Component
{
public:
    enum
    {
        PRESS,
        UP,
    };
    enum
    {
        BEGIN,
        CENTER,
        END
    };
    std::future<void> fbThread;
    std::queue<TouchInfo> infoQue1;
    std::queue<TouchInfo> infoQue2;
    int lastx[5];
    int lasty[5];
    int pressx[5];
    int pressy[5];
    int startp[5];
    shared_ptr<GObject> player;
    vector<FbButton *> btns;
    std::function<void(const vector<int>&,int)> exTouchFun;
    void addBtn(FbButton *btn);
    void clearUI()
    {
        btns.clear();
        infoQue1 = std::queue<TouchInfo>();
        infoQue2 = std::queue<TouchInfo>();
        exTouchFun = nullptr;
        player = nullptr;
    }
    FbButton *getBtn(int x,int y);
    void pressBtn(FbButton *btn);
    void releaseBtn(FbButton *btn);
    virtual void start() override;
    virtual void updateUI() override;
    virtual void update();
    virtual void renderUI() override;
};

class FbButton : public Component
{
public:
    string text;
    int textColor;
    int backgroundColor;
    int pressBackgroundColor;
    // 左上角， 屏幕坐标系
    int x, y;
    int tx,ty;
    int x_align;
    int y_align;
    int width, height;
    int status;
    int fontSize;
    FbUI *fbUI;
    std::function<void(void)> onPress;
    std::function<void(void)> onRelease;
    FbButton(int x, int y, int width, int height,FbUI *fbUI)
    {
        this->x = x;
        this->y = y;
        this->width = width;
        this->height = height;
        this->text = "";
        this->textColor = FB_COLOR(255, 255, 255);
        this->backgroundColor = FB_COLOR(155, 155, 155);
        this->pressBackgroundColor = FB_COLOR(55, 55, 55);
        this->status = 0;
        this->tx = this->ty = -1000;
        this->x_align = this->y_align = 0;
        this->fontSize = 40;
        this->fbUI = fbUI;
    }
    virtual void start() override;
};

#endif