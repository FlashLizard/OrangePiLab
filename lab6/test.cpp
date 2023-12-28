#include <iostream>
#include <vector>
#include <algorithm>
#include <time.h>

#include "fls_engine/engine.h"
#include "fb_tools.h"

using glm::vec3;
using glm::vec4;

using namespace std;
using namespace fls_engine;

void test1()
{

    all_fb_init();
    TextureShader tshader;
    ShaderRunner shaderRunner(SCREEN_WIDTH, SCREEN_HEIGHT, 4, 4);
    glm::vec3 viewPos = glm::vec3(0, 0, 4);
    glm::mat4 one(1);
    tshader.view = glm::lookAt(viewPos, viewPos + vec3(0, 0, -1), vec3(0, 1, 0));
    tshader.projection = glm::perspective(glm::radians(60.0f), SCREEN_WIDTH * 1.0f / SCREEN_HEIGHT, 0.1f, 500.0f);
    int start = task_get_time();
    int cnt = 0;
    auto diamondTex = readJPG("./res/diamond.jpg");
    auto soilTex = readJPG("./res/soil.jpg");
    auto obsTex = readJPG("./res/obsidian.jpg");
    auto bookTex = readJPG("./res/bookshelf.jpg");
    auto diamondVB = TextureVB::createRectangle(diamondTex);
    auto dcube = std::shared_ptr<SObject>(SObject::createCube(diamondTex));
    auto scube = std::shared_ptr<SObject>(SObject::createCube(soilTex));
    auto ocube = std::shared_ptr<SObject>(SObject::createCube(obsTex));
    auto bcube = std::shared_ptr<SObject>(SObject::createCube(bookTex));
    dcube->addChild(scube);
    scube->addChild(ocube);
    scube->addChild(bcube);
    scube->transform(glm::translate(one, glm::vec3(-1.2, 0, 0)));
    ocube->transform(glm::translate(one, glm::vec3(0, -1.2, 0)));
    bcube->transform(glm::translate(one, glm::vec3(0, 1.2, 0)));
    while (true)
    {
        shaderRunner.clearBuffer(FB_COLOR(255, 255, 255), 1000);
        dcube->transform(glm::rotate(one, glm::radians(1.0f), glm::vec3(0, 1, 0)));
        scube->transform(glm::rotate(one, glm::radians(0.3f), glm::vec3(-1, 0, 0)));
        auto dtmp = dcube->getVertexBundles();
        for (auto vb : dtmp)
        {
            tshader.loadVertexBundle(vb.get());
            shaderRunner.runOnce(&tshader);
        }
        fb_draw_buffer(shaderRunner.colorBuffer);
        fb_update();
        cnt++;
        int end = task_get_time();
        if (end - start >= 1000)
        {
            printf("fps:%d\n", cnt);
            cnt = 0;
            start = end;
        }
    }
}

void test2()
{

    all_fb_init();
    TextureLightShader tlshader;
    ShaderRunner shaderRunner(SCREEN_WIDTH, SCREEN_HEIGHT, 2, 2);
    glm::vec3 viewPos = glm::vec3(0, 0, 4);
    glm::mat4 one(1);
    tlshader.view = glm::lookAt(viewPos, viewPos + vec3(0, 0, -1), vec3(0, 1, 0));
    tlshader.projection = glm::perspective(glm::radians(60.0f), SCREEN_WIDTH * 1.0f / SCREEN_HEIGHT, 0.1f, 500.0f);
    int start = task_get_time();
    int cnt = 0;
    auto diamondTex = readJPG("./res/diamond.jpg");
    auto soilTex = readJPG("./res/soil.jpg");
    auto obsTex = readJPG("./res/obsidian.jpg");
    auto bookTex = readJPG("./res/bookshelf.jpg");
    auto diamondVB = TextureVB::createRectangle(diamondTex);
    auto dcube = std::shared_ptr<SObject>(SObject::createLightCube(diamondTex));
    auto scube = std::shared_ptr<SObject>(SObject::createLightCube(soilTex));
    auto ocube = std::shared_ptr<SObject>(SObject::createLightCube(obsTex));
    auto bcube = std::shared_ptr<SObject>(SObject::createLightCube(bookTex));
    dcube->addChild(scube);
    scube->addChild(ocube);
    scube->addChild(bcube);
    scube->transform(glm::translate(one, glm::vec3(-1.2, 0, 0)));
    ocube->transform(glm::translate(one, glm::vec3(0, -1.2, 0)));
    bcube->transform(glm::translate(one, glm::vec3(0, 1.2, 0)));
    while (true)
    {
        shaderRunner.clearBuffer(FB_COLOR(155, 155, 200), 1000);
        dcube->transform(glm::rotate(one, glm::radians(1.0f), glm::vec3(0, 1, 0)));
        scube->transform(glm::rotate(one, glm::radians(0.3f), glm::vec3(-1, 0, 0)));
        auto dtmp = dcube->getVertexBundles();
        for (auto vb : dtmp)
        {
            tlshader.loadVertexBundle(vb.get());
            shaderRunner.runOnce(&tlshader);
        }
        fb_draw_buffer(shaderRunner.colorBuffer);
        fb_update();
        cnt++;
        int end = task_get_time();
        if (end - start >= 1000)
        {
            printf("fps:%d\n", cnt);
            cnt = 0;
            start = end;
        }
    }
}

void test3()
{
    all_fb_init();
    printf("begin\n");
    TextureLightShader tlshader;
    ShaderRunner shaderRunner(SCREEN_WIDTH, SCREEN_HEIGHT, 4, 4);
    glm::vec3 viewPos = glm::vec3(0, 0, 4);
    glm::mat4 one(1);
    tlshader.view = glm::lookAt(viewPos, viewPos + vec3(0, 0, -1), vec3(0, 1, 0));
    tlshader.projection = glm::perspective(glm::radians(60.0f), SCREEN_WIDTH * 1.0f / SCREEN_HEIGHT, 0.1f, 500.0f);
    int start = task_get_time();
    int cnt = 0;
    auto diamondTex = readJPG("./res/diamond.jpg");
    auto soilTex = readJPG("./res/soil.jpg");
    auto obsTex = readJPG("./res/obsidian.jpg");
    auto bookTex = readJPG("./res/bookshelf.jpg");
    auto diamondVB = TextureVB::createRectangle(diamondTex);
    auto dcube = std::shared_ptr<SObject>(SObject::createLightCube(diamondTex));
    auto scube = std::shared_ptr<SObject>(SObject::createLightCube(soilTex));
    auto ocube = std::shared_ptr<SObject>(SObject::createLightCube(obsTex));
    auto bcube = std::shared_ptr<SObject>(SObject::createLightCube(bookTex));
    dcube->addChild(scube);
    scube->addChild(ocube);
    scube->addChild(bcube);
    scube->transform(glm::translate(one, glm::vec3(-1.2, 0, 0)));
    ocube->transform(glm::translate(one, glm::vec3(0, -1.2, 0)));
    bcube->transform(glm::translate(one, glm::vec3(0, 1.2, 0)));
    while (true)
    {
        shaderRunner.clearBuffer(FB_COLOR(155, 155, 200), 1000);
        dcube->transform(glm::rotate(one, glm::radians(1.0f), glm::vec3(0, 1, 0)));
        scube->transform(glm::rotate(one, glm::radians(0.3f), glm::vec3(-1, 0, 0)));
        auto dtmp = dcube->getVertexBundles();
        for (auto vb : dtmp)
        {
            tlshader.loadVertexBundle(vb.get());
            shaderRunner.runOnce(&tlshader);
        }
        fb_draw_buffer(shaderRunner.colorBuffer);
        fb_update();
        cnt++;
        int end = task_get_time();
        if (end - start >= 1000)
        {
            printf("fps:%d\n", cnt);
            cnt = 0;
            start = end;
            if (scube->removeChild(ocube.get()) == nullptr)
            {
                ocube->changeParent(scube.get(), ocube);
            }
            else
            {
                ocube->changeParent(dcube.get(), ocube);
            }
        }
    }
}

class GameManager : public Component
{
public:
    enum
    {
        GRASS = 0,
        DIRT,
        DIAMOND,
        BOOKSHELF,
        OBSIDIAN,
        BLOCK_END
    };
    static const int N = 64;
    shared_ptr<GObject> exist[2 * N][2 * N][2 * N];
    vector<shared_ptr<GObject>> blocks;
    bool existBlock(glm::vec3 pos)
    {
        int x = round(N + pos.x);
        int y = floor(N + pos.y);
        int z = round(N + pos.z);
        return exist[x][y][z]!=nullptr;
    }
    int addBlockGO(char *path)
    {
        auto tex = readJPG(path);
        auto ob = shared_ptr<SObject>(SObject::createLightCube(tex));
        blocks.push_back(make_shared<GObject>(*ob));
        return blocks.size() - 1;
    }
    int addBlockGO(char *pathT, char *pathS, char *pathB)
    {
        auto texT = readJPG(pathT);
        auto texS = readJPG(pathS);
        auto texB = readJPG(pathB);
        auto ob = shared_ptr<SObject>(SObject::createLightCube(texT, texS, texB));
        blocks.push_back(make_shared<GObject>(*ob));
        return blocks.size() - 1;
    }
    int addBlockGO(char *pathT, char *pathS)
    {
        auto texT = readJPG(pathT);
        auto texS = readJPG(pathS);
        auto ob = shared_ptr<SObject>(SObject::createLightCube(texT, texS, texT));
        blocks.push_back(make_shared<GObject>(*ob));
        return blocks.size() - 1;
    }
    void addBlock(int id, glm::vec3 pos)
    {
        int x = round(N + pos.x);
        int y = floor(N + pos.y);
        int z = round(N + pos.z);
        auto tmp = onwer->root->addChild(shared_ptr<GObject>(blocks[id]->clone()));
        exist[x][y][z] = tmp;
        tmp->setWorldPos(pos);
    }
    void brokeBlock(glm::vec3 pos)
    {
        int x = round(N + pos.x);
        int y = floor(N + pos.y);
        int z = round(N + pos.z);
        auto tmp = exist[x][y][z];
        if(tmp==nullptr) return;
        exist[x][y][z] = nullptr;
        onwer->root->removeChild(tmp.get());
        std::cout<<"!!!!!"<<std::endl;
    }
    int grassId, dirtId, diamondId, bookshelfId, obsidianId;
    virtual void start() override
    {
        cout << "Hello World" << endl;
        grassId = addBlockGO("./res/grass_top.jpg", "./res/grass_side.jpg", "./res/dirt.jpg");
        dirtId = addBlockGO("./res/dirt.jpg");
        diamondId = addBlockGO("./res/diamond.jpg");
        bookshelfId = addBlockGO("./res/bookshelf_top.jpg", "./res/bookshelf.jpg");
        obsidianId = addBlockGO("./res/obsidian.jpg");
        cout << "block res load success" << endl;
        for (int i = 0; i < N; i++)
        {
            for (int j = 0; j < N; j++)
            {
                for (int k = 0; k < N; k++)
                {
                    exist[i][j][k] = nullptr;
                }
            }
        }
        for (int i = -5; i <= 5; i++)
        {
            for (int j = -5; j <= 5; j++)
            {
                addBlock(grassId, glm::vec3(i, -2, j));
            }
        }
        addBlock(dirtId, glm::vec3(-1, -1, 3));
        addBlock(diamondId, glm::vec3(1, -1, 3));
        addBlock(bookshelfId, glm::vec3(-1, -1, -3));
        addBlock(obsidianId, glm::vec3(1, -1, -3));
    }
};

class Player : public Component
{
public:
    int nowId;
    float velocity;
    const float G = -9.8;
    const int height = 2;
    GameManager *gm;
    void addBlock(glm::vec3 pos)
    {
        gm->addBlock(nowId, pos);
    }
    virtual void start() override
    {
        nowId = 0;
        velocity = 0.0;
    }
    virtual void update() override
    {
        auto pos = onwer->getWorldPos();
        pos -= glm::vec3(0, height - 1, 0);
        auto dt = onwer->engine->deltaTime / 1000.0;
        if (gm->existBlock(pos + glm::vec3(0, velocity * dt, 0)))
        {
            velocity = 0.0;
        }
        if (abs(velocity) > 0.01)
        {
            onwer->transform(glm::translate(glm::mat4(1), glm::vec3(0, velocity * dt, 0)));
        }
        if (!gm->existBlock(pos + glm::vec3(0, -0.01, 0)))
        {
            velocity += G * dt;
        }
    }
};

shared_ptr<Scene> createScene1(shared_ptr<GObject> fbUIManager);
shared_ptr<Scene> createScene2(shared_ptr<GObject> fbUIManager);

shared_ptr<Scene> createScene2(shared_ptr<GObject> fbUIManager)
{

    glm::mat4 one(1);
    auto scene = make_shared<Scene>();
    auto player = make_shared<GObject>();
    auto gameManager = make_shared<GObject>();
    auto btns = make_shared<GObject>();

    auto fbUI = getComponent<FbUI>(fbUIManager->components);
    auto gameManagerCmt = std::make_shared<GameManager>();

    auto playerCmt = std::make_shared<Player>();
    playerCmt->gm = gameManagerCmt.get();

    player->addComponent(playerCmt);

    fbUI->clearUI();
    const int btnL = 90;
    const int btnI = 10;
    auto fBtn = make_shared<FbButton>(btnL + 2 * btnI, SCREEN_HEIGHT - 3 * (btnL + btnI), btnL, btnL, fbUI.get());
    auto bBtn = make_shared<FbButton>(btnL + 2 * btnI, SCREEN_HEIGHT - 1 * (btnL + btnI), btnL, btnL, fbUI.get());
    auto rBtn = make_shared<FbButton>(2 * (btnL + btnI) + btnI, SCREEN_HEIGHT - 2 * (btnL + btnI), btnL, btnL, fbUI.get());
    auto lBtn = make_shared<FbButton>(btnI, SCREEN_HEIGHT - 2 * (btnL + btnI), btnL, btnL, fbUI.get());
    auto eBtn = make_shared<FbButton>(btnI, btnI, btnL, btnL, fbUI.get());
    auto jBtn = make_shared<FbButton>(SCREEN_WIDTH - 2 * btnL + 2 * btnI, SCREEN_HEIGHT - 2 * (btnL + btnI), btnL, btnL, fbUI.get());

    vector<shared_ptr<FbButton>> blockBtns;
    for (int i = 0; i < GameManager::BLOCK_END; i++)
    {
        auto btn = make_shared<FbButton>((4 + i) * (btnL + btnI), SCREEN_HEIGHT - 1 * (btnL + btnI), btnL, btnL, fbUI.get());
        blockBtns.push_back(btn);
        btn->onRelease = [playerCmt, i]()
        {
            playerCmt->nowId = i;
        };
        btn->text = "B0";
        btn->text[1] = '1' + i;
        btn->fontSize = 30;
        btns->addComponent(btn);
    }

    const float speed = 1;
    fBtn->onPress = [player, one, speed, gameManagerCmt]()
    {
        float dt = player->engine->deltaTime / 1000.0;
        auto forward = player->getForward();
        forward = glm::normalize(glm::vec3(forward.x, 0, forward.z)) * speed * dt;
        auto prePos = player->getWorldPos() + forward + glm::vec3(0, -1, 0);
        if (gameManagerCmt->existBlock(prePos))
        {
            return;
        }
        player->transform(glm::translate(one, forward));
    };
    fBtn->text = "前";
    bBtn->onPress = [player, one, speed, gameManagerCmt]()
    {
        float dt = player->engine->deltaTime / 1000.0;
        auto forward = player->getForward();
        forward = glm::normalize(glm::vec3(forward.x, 0, forward.z)) * speed * dt;

        auto prePos = player->getWorldPos() - forward + glm::vec3(0, -1, 0);
        if (gameManagerCmt->existBlock(prePos))
        {
            return;
        }
        player->transform(glm::translate(one, -forward));
    };
    bBtn->text = "后";
    rBtn->onPress = [player, one, speed, gameManagerCmt]()
    {
        float dt = player->engine->deltaTime / 1000.0;
        auto forward = player->getForward();
        auto up = player->getUp();
        auto right = glm::cross(forward, up) * speed * dt;
        auto prePos = player->getWorldPos() + right + glm::vec3(0, -1, 0);
        if (gameManagerCmt->existBlock(prePos))
        {
            return;
        }
        player->transform(glm::translate(one, right));
    };
    rBtn->text = "右";
    lBtn->onPress = [player, one, speed, gameManagerCmt]()
    {
        float dt = player->engine->deltaTime / 1000.0;
        auto forward = player->getForward();
        auto up = player->getUp();
        auto right = glm::cross(forward, up) * speed * dt;
        auto prePos = player->getWorldPos() - right + glm::vec3(0, -1, 0);
        if (gameManagerCmt->existBlock(prePos))
        {
            return;
        }
        player->transform(glm::translate(one, -right));
    };
    lBtn->text = "左";
    eBtn->onRelease = [scene, fbUIManager]()
    {
        scene->engine->loadScene(createScene1(fbUIManager));
    };
    eBtn->text = "退";
    jBtn->onRelease = [playerCmt]()
    {
        playerCmt->velocity = 4.0;
    };
    jBtn->text = "跳";

    btns->addComponent(fBtn);
    btns->addComponent(bBtn);
    btns->addComponent(rBtn);
    btns->addComponent(lBtn);
    btns->addComponent(eBtn);
    btns->addComponent(jBtn);

    fbUI->player = player;
    fbUI->exTouchFun = [gameManagerCmt, playerCmt](const vector<int> &info, int time)
    {
        if (info.size() != 2)
            return;
        if (time > 500)
        {
            int pos = info[1];
            int x = pos & 0xff;
            x -= 128;
            pos >>= 8;
            int y = pos & 0xff;
            y -= 128;
            pos >>= 8;
            int z = pos & 0xff;
            z -= 128;
            gameManagerCmt->brokeBlock(glm::vec3(x, y, z));
        }
        else
        {
            int pos = info[0];
            int x = pos & 0xff;
            x -= 128;
            pos >>= 8;
            int y = pos & 0xff;
            y -= 128;
            pos >>= 8;
            int z = pos & 0xff;
            z -= 128;
            playerCmt->addBlock(glm::vec3(x, y, z));
        }
    };

    fbUIManager->addComponent(fbUI);

    scene->addChild(btns);
    scene->addChild(player);
    scene->addChild(gameManager);
    scene->addChild(fbUIManager);
    scene->backgroundColor = FB_COLOR(175, 175, 210);

    player->lookAt(glm::vec3(0, 0, -1), glm::vec3(0, 1, 0));

    auto camera = make_shared<CameraComponent>(*player, glm::radians(60.0f), SCREEN_WIDTH * 1.0f / SCREEN_HEIGHT, 0.1f, 100.0f);
    scene->camera = getComponent<CameraComponent>(player->components);

    gameManager->addComponent(gameManagerCmt);

    return scene;
}

shared_ptr<Scene> createScene1(shared_ptr<GObject> fbUIManager)
{
    auto scene = make_shared<Scene>();
    auto btns = make_shared<GObject>();

    auto fbUI = getComponent<FbUI>(fbUIManager->components);
    fbUI->clearUI();

    const int w = 200;
    const int h = 90;
    const int btnI = 30;
    int x = SCREEN_WIDTH / 2 - w / 2;
    int y = SCREEN_HEIGHT / 2 - h / 2;

    auto title = make_shared<FbButton>(x, y - h - 4 * btnI, w, h, fbUI.get());
    auto sBtn = make_shared<FbButton>(x, y, w, h, fbUI.get());
    auto eBtn = make_shared<FbButton>(x, y + h + btnI, w, h, fbUI.get());

    sBtn->text = "开始游戏";
    eBtn->text = "退出游戏";
    title->text = "MINECRAFT";

    title->backgroundColor = title->pressBackgroundColor = FB_COLOR(175, 175, 210);
    title->textColor = FB_COLOR(110, 160, 120);
    title->fontSize = 50;

    sBtn->onRelease = [scene, fbUIManager]()
    {
        scene->engine->loadScene(createScene2(fbUIManager));
    };
    eBtn->onRelease = [scene, fbUIManager]()
    {
        scene->engine->loadScene(nullptr);
    };

    btns->addComponent(sBtn);
    btns->addComponent(eBtn);
    btns->addComponent(title);

    scene->addChild(btns);
    scene->addChild(fbUIManager);
    scene->backgroundColor = FB_COLOR(175, 175, 210);
    return scene;
}

void test4()
{
    Engine engine(SCREEN_WIDTH, SCREEN_HEIGHT, 4, 4, update_fb_screen, fb_update, task_get_time);
    engine.devInit = all_fb_init;
    engine.filterVB = Engine::filterBlockVB;
    int shader1 = engine.addShader(shared_ptr<BaseShader>(new TextureLightShader()));
    auto fbUI = make_shared<FbUI>();
    auto fbUIManager = make_shared<GObject>();
    fbUIManager->addComponent(fbUI);

    auto scene1 = createScene1(fbUIManager);

    engine.loadScene(scene1);
    engine.run();
}