#include "engine.h"
using std::vector;
using std::shared_ptr;

namespace fls_engine
{
    Engine::Engine(int width, int height, int zoomX, int zoomY, UpdateScreen updateScreen, FreshScreen freshScreen, GetTime getTime) : shaderRunner(width, height, zoomX, zoomY)
    {
        shouldRestart = true;
        devInit = nullptr;
        stageDebug = false;
        this->updateScreen = updateScreen;
        this->freshScreen = freshScreen;
        this->getTime = getTime;
    }
    void Engine::run()
    {
        if (devInit != nullptr)
        {
            devInit();
        }
        while (shouldRestart)
        {
            if (scene == nullptr)
                return;
            // TIP: 因为中途可能切换scene, 所以要这样存一下最开始的scene
            shouldRestart = false;
            preTime = getTime();
            deltaTime = 0;
            auto current = scene;
            current->initScene(*current, *this);
            while (true)
            {
                shaderRunner.clearBuffer(current->backgroundColor, current->backgroundZ);
                current->start();
                if (stageDebug)
                    printf("start end\n");
                current->enable();
                if (stageDebug)
                    printf("enable end\n");
                current->updateUI();
                if (stageDebug)
                    printf("update end1\n");
                if (shouldRestart)
                    break;

                if (stageDebug)
                    printf("get begin\n");

                auto camera = getCamera();
                if (camera != nullptr)
                {
                    auto vbs = current->getVertexBundles();
                    if (this->filterVB != nullptr)
                    {
                        vbs = filterVB(*this, vbs);
                    }
                    if (stageDebug)
                        printf("get end\n");
                    if (stageDebug)
                        printf("get end2\n");
                    auto view = camera->getView();
                    auto projection = camera->getProjection();
                    auto normalMat = glm::transpose(glm::inverse(glm::mat3(view)));
                    auto lightPos = view * glm::vec4(-1, 5, -1,1);
                    for (auto shader : shaders)
                    {
                        shader->view = view;
                        shader->projection = projection;
                        shader->setWolrdLigth(lightPos);
                    }
                    if (stageDebug)
                        printf("get end3\n");
                    for (auto vb : vbs)
                    {
                        if (stageDebug)
                            printf("vb\n");
                        auto shader = shaders[vb->shaderId];
                        shader->loadVertexBundle(vb.get());
                        shaderRunner.runOnce(shader.get());
                    }

                    if (stageDebug)
                        printf("begin shader\n");
                }
                updateScreen(shaderRunner.colorBuffer);
                
                if (stageDebug)
                    printf("updateUI end\n");
                int nowTime = getTime();
                deltaTime = nowTime-preTime;
                preTime = nowTime;
                current->update();
                current->renderUI();
                freshScreen();
                if (stageDebug)
                    printf("one end\n");
            }
        }
    }
    unsigned int Engine::addShader(shared_ptr<BaseShader> shader)
    {
        shaders.push_back(shader);
        return shaders.size() - 1;
    }
    shared_ptr<CameraComponent> Engine::getCamera()
    {
        return this->scene->camera;
    }
    void Engine::loadScene(shared_ptr<Scene> scene)
    {
        shouldRestart = true;
        this->scene = scene;
    }

    vector<shared_ptr<VertexBundle> > Engine::filterBlockVB(Engine &engine, vector<shared_ptr<VertexBundle> > &vbs)
    {
        static int size = 128 * 128 * 128;
        static int flag[128 * 128 * 128];
        static int cnt = 0;
        static auto toInd = [](const vector<Vertex> &vertexes)
        {
            glm::vec4 all = vertexes[0].pos + vertexes[1].pos + vertexes[2].pos + vertexes[3].pos;
            int x = (32 + all.x) * 2;
            int y = (32 + all.y) * 2;
            int z = (32 + all.z) * 2;
            return x | (y << 7) | (z << 14);
        };
        cnt++;
        vector<shared_ptr<VertexBundle> > result;
        auto viewPos = engine.getCamera()->onwer->getWorldPos();
        for (auto vb : vbs)
        {
            auto vts = vb->getVertexes();
            if (vts.size() != 4)
                continue;
            int ind = toInd(vts);
            if (ind < 0 || ind >= size)
                continue;
            if (flag[ind] != cnt)
                flag[ind] = cnt;
            else
                flag[ind]--;
            auto a = vts[0];
            auto viewVec = viewPos - glm::vec3(a.pos);
            auto normal = glm::vec3(a.fAttrs[TextureLightVB::NX], a.fAttrs[TextureLightVB::NY], a.fAttrs[TextureLightVB::NZ]);
            if (glm::dot(normal, viewVec) <= -0.0001f)
                flag[ind]--;
        }

        for (auto vb : vbs)
        {
            auto vts = vb->getVertexes();
            if (vts.size() != 4)
            {
                result.push_back(vb);
                continue;
            }
            int ind = toInd(vts);
            if (ind < 0 || ind >= size)
            {
                result.push_back(vb);
                continue;
            }
            if (flag[ind] == cnt)
                result.push_back(vb);
        }

        return result;
    }
}