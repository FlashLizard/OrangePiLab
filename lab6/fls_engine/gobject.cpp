#include "gobject.h"
#include <algorithm>

using std::vector;
using std::shared_ptr;

namespace fls_engine
{
    unsigned int GObject::cnt = 0;
    GObject::GObject()
    {
        id = cnt++;
        if (cnt == 0)
        {
            std::cout << "error: sobject cnt overflow" << std::endl;
        }
        parent = nullptr;
        root = nullptr;
        engine = nullptr;
        isActive = true;
        body = std::make_shared<SObject>();
        forward = glm::vec3(0, 0, -1);
        up = glm::vec3(0, 1, 0);
    }
    GObject::GObject(const SObject &body) : GObject()
    {
        this->body = std::shared_ptr<SObject>(body.clone());
    }
    // DOC: will run children first
    void GObject::update()
    {
        auto tmp = children;
        for (auto child : tmp)
        {
            if (child->isActive)
            {
                child->update();
            }
        }
        
        auto ctmp = components;
        for (auto component : ctmp)
        {
            if (component->isEnabled)
            {
                component->update();
            }
        }
    }
    void GObject::updateUI()
    {
        auto tmp = children;
        for (auto child : tmp)
        {
            if (child->isActive)
            {
                child->updateUI();
            }
        }
        auto ctmp = components;
        for (auto component : ctmp)
        {
            if (component->isEnabled)
            {
                component->updateUI();
            }
        }
    }
    void GObject::start()
    {
        auto tmp = children;
        for (auto child : tmp)
        {
            if (child->isActive)
            {
                child->start();
            }
        }
        auto ctmp = components;
        for (auto component : ctmp)
        {
            if (!component->started)
            {
                component->start();
                component->started = true;
            }
        }
    }
    void GObject::enable()
    {
        auto tmp = children;
        for (auto child : tmp)
        {
            if (child->isActive)
            {
                child->enable();
            }
        }
        auto ctmp = components;
        for (auto component : ctmp)
        {
            if (!component->enabled)
            {
                component->enable();
                component->enabled = true;
            }
        }
    }
    void GObject::renderUI()
    {
        auto tmp = children;
        for (auto child : tmp)
        {
            if (child->isActive)
            {
                child->renderUI();
            }
        }
        auto ctmp = components;
        for (auto component : ctmp)
        {
            if (component->isEnabled)
            {
                component->renderUI();
            }
        }
    }
    shared_ptr<GObject> GObject::addChild(shared_ptr<GObject> child)
    {
        this->children.push_back(child);
        child->setParent(this);
        return child;
    }
    shared_ptr<GObject> GObject::addChild(shared_ptr<GObject> child, glm::vec3 pos)
    {
        this->children.push_back(child);
        child->setParent(this);
        child->setWorldPos(pos);
        return child;
    }
    shared_ptr<GObject> GObject::addChild(shared_ptr<GObject> child, glm::vec3 pos, glm::vec3 forward, glm::vec3 up)
    {
        this->children.push_back(child);
        child->setParent(this);
        child->setWorldPos(pos);
        child->lookAt(forward, up);
        return child;
    }
    shared_ptr<GObject> GObject::removeChild(GObject *child)
    {
        auto id = child->getId();
        auto it = std::find_if(children.begin(), children.end(), [id](shared_ptr<GObject> ele)
                               { return ele->id == id; });
        if (it == children.end())
            return nullptr;
        auto tmp = *it;
        tmp->setParent(nullptr);
        children.erase(it);
        return tmp;
    }
    shared_ptr<GObject> GObject::removeChild(GObject *child, glm::vec3 &opos, glm::vec3 &oforward, glm::vec3 &oup)
    {
        auto id = child->getId();
        auto it = std::find_if(children.begin(), children.end(), [id](shared_ptr<GObject> ele)
                               { return ele->id == id; });
        if (it == children.end())
            return nullptr;
        auto tmp = *it;
        opos = tmp->getWorldPos();
        oforward = tmp->getForward();
        oup = tmp->getUp();
        tmp->setParent(nullptr);
        children.erase(it);
        return tmp;
    }
    void GObject::changeParent(GObject *parent, shared_ptr<GObject> self)
    {
        glm::vec3 pos(0, 0, 0);
        glm::vec3 forward(0, 0, 1);
        glm::vec3 up(0, 1, 0);
        if (this->parent != nullptr)
        {
            this->parent->removeChild(this, pos, forward, up);
        }
        if (parent != nullptr)
        {
            parent->addChild(self);
            this->setWorldPos(pos);
            this->lookAt(forward, up);
        }
        else
        {
            this->setParent(nullptr);
        }
    }
    void GObject::reset()
    {
        body->reset();
    }
    void GObject::setParent(GObject *parent)
    {
        if (parent == this->parent)
            return;
        this->parent = parent;
        if (parent==nullptr) {
            isActive = false;
            return;
        } else {
            isActive = true;
        }
        this->engine = parent->engine;
        this->root = parent->root;
    }
    std::vector<std::shared_ptr<VertexBundle> > GObject::getVertexBundles() const
    {
        std::vector<shared_ptr<VertexBundle> > result;
        auto model = this->getModelMat();
        for (auto it = children.begin(); it != children.end(); it++)
        {
            auto ptr = *it;
            auto tmp = ptr->getVertexBundles();
            for (auto vb : tmp)
            {
                vb->transform(model);
            }
            result.insert(result.end(), tmp.begin(), tmp.end());
        }
        for (auto child : body->children)
        {
            auto vbs = child->getVertexBundles();
            for (auto vb : vbs)
            {
                vb->transform(model);
                result.push_back(vb);
            }
        }
        auto vb = shared_ptr<VertexBundle>(body->getVertexBundle());
        if (vb != nullptr)
        {
            vb->transform(model);
            result.push_back(vb);
        }
        return result;
    }
    // DOC: 无parent
    GObject *GObject::clone() const
    {
        auto tmp = new GObject();
        tmp->root = root;
        tmp->engine = engine;
        tmp->isActive = true;
        tmp->body = std::shared_ptr<SObject>(body->clone());
        tmp->forward = forward;
        tmp->up = up;
        for (auto child : children)
        {
            auto ctmp = std::shared_ptr<GObject>(child->clone());
            tmp->addChild(ctmp);
        }
        for (auto component : components)
        {
            auto ctmp = std::shared_ptr<Component>(component->clone());
            tmp->addComponent(ctmp);
        }
        return tmp;
    }
    shared_ptr<GObject> GObject::getChild(unsigned ind) const
    {
        if (ind >= children.size())
            return children[ind];
        return nullptr;
    }
    void GObject::addComponent(shared_ptr<Component> component)
    {
        this->components.push_back(component);
        component->onwer = this;
    }

    glm::mat4 GObject::getModelMat() const
    {
        return body->getModelMat() * getLookAtMat();
    }
    glm::mat4 GObject::getWorldMat() const
    {
        glm::mat4 mat = body->getModelMat();
        GObject *gb = this->parent;
        while (gb != nullptr)
        {
            mat = gb->getModelMat() * mat;
            gb = gb->parent;
        }
        return mat;
    }
    glm::vec3 GObject::getSelfPos() const
    {
        glm::vec4 pos(0, 0, 0, 1);
        GObject *gb = this->parent;
        if (gb != nullptr)
        {
            pos = gb->getModelMat() * pos;
        }
        return glm::vec3(pos);
    }
    glm::vec3 GObject::getWorldPos() const
    {
        glm::vec4 pos(0, 0, 0, 1);
        return glm::vec3(getWorldMat() * pos);
    }
    // DOC: Relative to the parent's coordinate system
    void GObject::setWorldPos(glm::vec3 targetPos)
    {
        auto pos = getWorldPos();
        body->transform(glm::translate(glm::mat4(1), targetPos - glm::vec3(pos)));
    }
    glm::mat4 GObject::getLookAtMat() const
    {
        return glm::lookAt(glm::vec3(0, 0, 0), forward, up);
    }
    glm::vec3 GObject::getForward() const
    {
        return forward;
    }
    glm::vec3 GObject::getUp() const
    {
        return up;
    }
    void GObject::lookAt(glm::vec3 tForward, glm::vec3 tUp)
    {
        forward = tForward;
        up = tUp;
    }
    void GObject::transform(glm::mat4 mat)
    {
        body->transform(mat);
    }
    unsigned int GObject::getId() const
    {
        return id;
    }
    bool GObject::operator==(const GObject &b) const
    {
        return id == b.getId();
    }
    void GObject::initScene(Scene &root, Engine &engine)
    {
        this->root = &root;
        this->engine = &engine;
        for (auto child : children)
        {
            child->initScene(root, engine);
        }
    }
}