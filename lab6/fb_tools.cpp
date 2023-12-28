#include "fb_tools.h"
#include <stdio.h>

static int start;
static int cnt = 0;
std::function<void(int)> cFun;

void all_fb_init()
{
    fb_init("/dev/fb0");
    font_init("./res/font.ttc");
    start = task_get_time();
}

Texture readJPG(char *path)
{
    auto tmp = fb_read_jpeg_image(path);
    auto ttmp = Texture(tmp->pixel_w, tmp->pixel_h, (int *)tmp->content);
    fb_free_image(tmp);
    printf("read jpg %s success\n",path);
    return ttmp;
}

void update_fb_screen(int *src)
{
    fb_draw_buffer(src);
    cnt++;
    int end = task_get_time();
    if (end - start >= 1000)
    {
        printf("fps:%d\n", cnt);
        cnt = 0;
        start = end;
    }
}

void FbUI::start()
{
    for (int i = 0; i < 5; i++)
    {
        lastx[i] = lasty[i] = -1;
    }
    fbThread = std::async(std::launch::async, [this]()
                          {
            int touch_fd = touch_init("/dev/input/event2");
            auto &que = this->infoQue1;
            cFun = [&que](int fd) {
                int type, x, y, finger;
                type = touch_read(fd, &x, &y, &finger);
                que.push((TouchInfo){type,x,y,finger});
            };
            task_add_file(touch_fd, touch_cb);
            task_loop();
            std::cout<<"ui thread end"<<std::endl; });
}
void FbUI::updateUI()
{
    static int start = task_get_time();
    int cnt = 100;
    bool flag = false;
    TouchInfo tmp;
    int end = task_get_time();
    if (infoQue1.size() < 30 && (end - start) < 300)
    {
        return;
    }
    start = end;
    while ((!infoQue1.empty()) && cnt > 0)
    {
        cnt--;
        auto touchInfo = infoQue1.front();
        infoQue1.pop();
        switch (touchInfo.type)
        {
        case TOUCH_PRESS:
            if (flag)
            {
                infoQue2.push(tmp);
                flag = false;
            }
            infoQue2.push(touchInfo);
            break;
        case TOUCH_MOVE:
        {
            tmp = touchInfo;
            flag = true;
            break;
        }
        case TOUCH_RELEASE:
            if (flag)
            {
                infoQue2.push(tmp);
                flag = false;
            }
            infoQue2.push(touchInfo);
            break;
        case TOUCH_ERROR:
            break;
        default:
            break;
        }
    }
    if (flag)
        infoQue2.push(tmp);
}
void FbUI::update()
{
    int cnt = 5;
    while (!infoQue2.empty() && cnt >= 0)
    {
        cnt--;
        auto touchInfo = infoQue2.front();
        infoQue2.pop();
        switch (touchInfo.type)
        {
        case TOUCH_PRESS:
        {
            pressx[touchInfo.finger] = lastx[touchInfo.finger] = touchInfo.x;
            pressy[touchInfo.finger] =lasty[touchInfo.finger] = touchInfo.y;
            startp[touchInfo.finger] = task_get_time();
            auto btn = getBtn(touchInfo.x, touchInfo.y);
            if (btn != nullptr)
                pressBtn(btn);
            break;
        }
        case TOUCH_MOVE:
        {
            if (player != nullptr)
            {
                glm::vec4 forward = glm::vec4(player->getForward(), 0);
                int dx = touchInfo.x - lastx[touchInfo.finger];
                int dy = touchInfo.y - lasty[touchInfo.finger];
                float thetaX = glm::radians(dx * 0.5);
                float thetaY = glm::radians(dy * 0.5);
                glm::mat4 one(1);
                forward = glm::rotate(one, -thetaX, glm::vec3(0, 1, 0)) * forward;
                auto aixs = glm::cross(glm::vec3(0, 1, 0), glm::vec3(forward));
                forward = glm::rotate(one, thetaY, aixs) * forward;
                player->lookAt(glm::normalize(glm::vec3(forward)), glm::vec3(0, 1, 0));
                lastx[touchInfo.finger] = touchInfo.x;
                lasty[touchInfo.finger] = touchInfo.y;
            }
            break;
        }
        case TOUCH_RELEASE:
        {
            if(pressx[touchInfo.finger]!=touchInfo.x||pressy[touchInfo.finger]!=touchInfo.y) break;
            auto btn = getBtn(touchInfo.x, touchInfo.y);

            if (btn != nullptr)
                releaseBtn(btn);
            else
            {
                if (exTouchFun != nullptr)
                {
                    int ind = touchInfo.y * SCREEN_WIDTH + touchInfo.x;
                    auto info = onwer->engine->shaderRunner.exBuffer[ind];
                    this->exTouchFun(info,task_get_time()-startp[touchInfo.finger]);
                }
            }
            break;
        }
        case TOUCH_ERROR:
            break;
        default:
            return;
        }
    }
    for (auto btn : btns)
    {
        if (btn->status == PRESS && btn->onPress != nullptr)
            btn->onPress();
    }
}

void FbUI::renderUI()
{
    for (auto btn : btns)
    {
        int x = btn->x;
        int y = btn->y;
        int w = btn->width;
        int h = btn->height;
        int backgroundColor = btn->backgroundColor;
        int pressBackgroundColor = btn->pressBackgroundColor;
        switch (btn->status)
        {
        case UP:
            fb_draw_rect(x, y, w, h, backgroundColor);
            break;
        case PRESS:
            fb_draw_rect(x, y, w, h, pressBackgroundColor);
            break;
        default:
            break;
        }
        if (btn->text != "")
        {
            if (btn->tx == -1000)
            {
                ipair xy = fb_draw_text(-1000, -1000, (char *)btn->text.c_str(), btn->fontSize, btn->textColor);
                switch (btn->x_align)
                {
                case BEGIN:
                    btn->tx = btn->x;
                    break;
                case CENTER:
                    btn->tx = x + w / 2 - xy.x / 2;
                    break;
                case END:
                    btn->tx = x + w - xy.x;
                }
                switch (btn->y_align)
                {
                case BEGIN:
                    btn->ty = y + xy.y;
                    break;
                case CENTER:
                    btn->ty = y + h / 2 + xy.y / 2;
                    break;
                case END:
                    btn->ty = y + h;
                }
            }
            fb_draw_text(btn->tx, btn->ty, (char *)(btn->text.c_str()), btn->fontSize, btn->textColor);
        }
    }
}

void FbUI::pressBtn(FbButton *btn)
{
    btn->status = PRESS;
}
void FbUI::releaseBtn(FbButton *btn)
{
    if (btn->status == PRESS)
    {
        btn->status = UP;
        if (btn->onRelease != nullptr)
            btn->onRelease();
    }
}

FbButton *FbUI::getBtn(int x, int y)
{
    for (auto btn : btns)
    {
        if (btn->x < x && btn->y < y && x < btn->x + btn->width && y < btn->y + btn->height)
            return btn;
    }
    return nullptr;
}

void FbUI::addBtn(FbButton *btn)
{
    btn->status = UP;
    btn->tx = btn->ty = -1000;
    btn->x_align = btn->y_align = CENTER;
    btns.push_back(btn);
}

void FbButton::start()
{
    fbUI->addBtn(this);
}