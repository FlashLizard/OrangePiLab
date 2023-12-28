#ifndef ENGINE_H
#define ENGINE_H


#include <vector>
#include "shader.h"

#include "typedef.h"
#include "gobject.h"
#include "component.h"


namespace fls_engine
{
    class Engine
    {
    private:
        int preTime;
    public:
        int deltaTime;
        std::vector<std::shared_ptr<BaseShader> > shaders;
        ShaderRunner shaderRunner;
        std::shared_ptr<Scene> scene;
        bool shouldRestart;
        bool stageDebug;
        DevInit devInit;
        UpdateScreen updateScreen;
        FreshScreen freshScreen;
        GetTime getTime;
        
        std::function<std::vector<std::shared_ptr<VertexBundle> > (Engine&,std::vector<std::shared_ptr<VertexBundle> >&)> filterVB;

        Engine(int width, int height, int zoomX, int zoomY,UpdateScreen updateScreen, FreshScreen freshScreen,GetTime getTime);
        void run();
        unsigned int addShader(std::shared_ptr<BaseShader> shader);
        std::shared_ptr<CameraComponent> getCamera();
        void loadScene(std::shared_ptr<Scene> scene);
        
        //64*64*64
        static std::vector<std::shared_ptr<VertexBundle> > filterBlockVB(Engine& engine,std::vector<std::shared_ptr<VertexBundle> >& vbs);
    };
}

#endif