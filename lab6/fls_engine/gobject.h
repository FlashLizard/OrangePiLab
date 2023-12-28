#ifndef GOBJECT_H
#define GOBJECT_H

#include "engine.h"
#include <vector>
#include <memory>
#include <algorithm>


namespace fls_engine
{
    //class Component;
    class GObject
    {
    protected:
        static unsigned int cnt;
        unsigned int id;
        std::vector<std::shared_ptr<GObject> > children;

        void setParent(GObject *parent);

    public:
        std::vector<std::shared_ptr<Component> > components;
        Engine *engine;
        GObject *root;
        GObject *parent;
        glm::vec3 forward;
        glm::vec3 up;
       std::shared_ptr<SObject> body;
        bool isActive;

        GObject();
        GObject(const SObject &body);
        void update();
        void updateUI();
        void start();
        void enable();
        void renderUI();
        std::vector<std::shared_ptr<VertexBundle> > getVertexBundles() const;
       std::shared_ptr<GObject> removeChild(GObject *child);
        // 保持世界坐标的添加
       std::shared_ptr<GObject> removeChild(GObject *child, glm::vec3 &opos, glm::vec3 &oforward, glm::vec3 &oup);
        void addComponent(std::shared_ptr<Component> component);
       std::shared_ptr<GObject>  addChild(std::shared_ptr<GObject> child, glm::vec3 pos, glm::vec3 forward, glm::vec3 up);
       std::shared_ptr<GObject>  addChild(std::shared_ptr<GObject> child, glm::vec3 pos);
       std::shared_ptr<GObject>  addChild(std::shared_ptr<GObject> child);
        void changeParent(GObject *parent,std::shared_ptr<GObject> self);
        void reset();
        GObject *clone() const;
       std::shared_ptr<GObject> getChild(unsigned ind) const;
        glm::mat4 getModelMat() const;
        glm::mat4 getWorldMat() const;
        glm::mat4 getLookAtMat() const;
        glm::vec3 getSelfPos() const;
        glm::vec3 getWorldPos() const;
        // DOC: Relative to the parent's coordinate system
        void setWorldPos(glm::vec3 targetPos);
        glm::vec3 getForward() const;
        glm::vec3 getUp() const;
        void lookAt(glm::vec3 tForward, glm::vec3 tUp);
        void transform(glm::mat4 mat);
        unsigned int getId() const;
        bool operator==(const GObject &b) const;
        virtual void initScene(Scene &root, Engine &engine);
        virtual ~GObject() {}
    };

    class Scene : public GObject
    {
    public:
        int backgroundColor;
        int backgroundZ;
       std::shared_ptr<CameraComponent> camera;
        Scene()
        {
            backgroundColor = 0xff000000;
            backgroundZ = 1000;
        }
        virtual ~Scene() {}
    };
}

#endif