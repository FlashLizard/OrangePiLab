#ifndef COMPONENT_H
#define COMPONENT_H

#include "engine.h"

namespace fls_engine
{
    class Component
    {
    protected:
    public:
        GObject *onwer;
        bool isEnabled;
        bool enabled;
        bool started;
        Component();
        Component(GObject &onwer);
        virtual void update() {}
        virtual void updateUI() {}
        virtual void start() {}
        virtual void enable() {}
        virtual void renderUI() {}
        virtual Component *clone() const;
        virtual ~Component() {}
    };
    class CameraComponent : public Component
    {
    protected:
        glm::mat4 projection;

    public:
        float fovy;
        float aspect;
        float near;
        float far;
        CameraComponent(GObject &onwer, float fovy, float aspect, float near, float far);
        glm::mat4 getView();
        void setProjection(float fovy, float aspect, float near, float far);
        glm::mat4 getProjection();
        virtual CameraComponent *clone() const;
        virtual ~CameraComponent() {}
    };
}

#endif