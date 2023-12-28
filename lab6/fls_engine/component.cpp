#include "component.h"
using std::shared_ptr;
namespace fls_engine
{
    Component::Component()
    {
        onwer = nullptr;
        isEnabled = true;
        enabled = false;
        started = false;
    }
    Component::Component(GObject &onwer)
    {
        this->onwer = &onwer;
        onwer.addComponent(shared_ptr<Component>(this));
    }
    Component *Component::clone() const
    {
        auto tmp = new Component(*this);
        tmp->onwer = nullptr;
        return tmp;
    }

    CameraComponent::CameraComponent(GObject &onwer, float fovy, float aspect, float near, float far) : Component(onwer)
    {
        this->fovy = fovy;
        this->aspect = aspect;
        this->near = near;
        this->far = far;
        this->projection = glm::perspective(fovy, aspect, near, far);
    }
    glm::mat4 CameraComponent::getView()
    {
        auto pos = onwer->getWorldPos();
        auto forward = onwer->getForward();
        auto up = onwer->getUp();
        return glm::lookAt(pos, pos + forward, up);
    }
    void CameraComponent::setProjection(float fovy, float aspect, float near, float far)
    {
        this->fovy = fovy;
        this->aspect = aspect;
        this->near = near;
        this->far = far;
        this->projection = glm::perspective(fovy, aspect, near, far);
    }
    glm::mat4 CameraComponent::getProjection()
    {
        return projection;
    }
    CameraComponent *CameraComponent::clone() const
    {
        auto tmp = new CameraComponent(*this);
        tmp->onwer = nullptr;
        return tmp;
    }
}