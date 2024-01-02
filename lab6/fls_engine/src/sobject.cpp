#include "sobject.h"
#include <iostream>
#include <algorithm>

using std::shared_ptr;
using std::string;

namespace fls_engine
{
    unsigned int SObject::cnt = 0;
    SObject::SObject()
    {
        id = cnt++;
        if (cnt == 0)
        {
            std::cout << "error: sobject cnt overflow" << std::endl;
        }
        selfModelMat = glm::mat4(1);
        parent = nullptr;
    }
    SObject::SObject(const VertexBundle &vb) : SObject()
    {
        setVertexBundle(vb);
    }
    unsigned int SObject::getId() const
    {
        return id;
    }
    std::vector<std::shared_ptr<VertexBundle> > SObject::getVertexBundles() const
    {
        std::vector<shared_ptr<VertexBundle> > result;
        for (auto it = children.begin(); it != children.end(); it++)
        {
            auto ptr = *it;
            auto tmp = ptr->getVertexBundles();
            for (auto vb : tmp)
            {
                vb->transform(selfModelMat);
            }
            result.insert(result.end(), tmp.begin(), tmp.end());
        }
        auto vb = std::shared_ptr<VertexBundle>(this->getVertexBundle());

        if (vb != nullptr)
        {
            vb->transform(selfModelMat);
            result.push_back(vb);
        }
        return result;
    }
    void SObject::setSelfModleMat(const glm::mat4 &mat)
    {
        this->selfModelMat = mat;
    }
    void SObject::transform(const glm::mat4 &mat)
    {
        this->selfModelMat = mat * selfModelMat;
    }
    // TIP: 先remove，再add
    void SObject::addChild(shared_ptr<SObject> child)
    {
        this->children.push_back(child);
        child->setParent(this);
    }
    // TIP: 不能返回bool，否则没有shared指向child，会被自动回收
    shared_ptr<SObject> SObject::removeChild(SObject *child)
    {
        auto id = child->getId();
        auto it = std::find_if(children.begin(), children.end(), [id](shared_ptr<SObject> ele)
                               { return ele->id == id; });
        if (it == children.end())
            return nullptr;
        auto tmp = *it;
        tmp->setParent(nullptr);
        children.erase(it);
        return tmp;
    }
    shared_ptr<SObject> SObject::removeChild(const unsigned id)
    {
        auto it = std::find_if(children.begin(), children.end(), [id](shared_ptr<SObject> ele)
                               { return ele->id == id; });
        if (it == children.end())
            return nullptr;
        auto tmp = *it;
        tmp->setParent(nullptr);
        children.erase(it);
        return tmp;
    }
    // TIP: 认为已经add到对应parent中或remove掉了，否则如果这里也add或remove的话，会死循环
    void SObject::setParent(SObject *parent)
    {
        if (parent == this->parent)
            return;
        this->parent = parent;
    }
    // TIP: self是为了防止引用丢失
    void SObject::changeParent(SObject *parent, shared_ptr<SObject> self)
    {
        if (this->parent != nullptr)
        {
            this->parent->removeChild(this);
        }
        if (parent != nullptr)
        {
            parent->addChild(self);
        }
        else
        {
            this->setParent(nullptr);
        }
    }
    VertexBundle *SObject::getVertexBundle() const
    {
        // TIP: 此处必须要clone()（原型模式），否则后续对vb的操作会影响到sobject的vb
        if (vertexBundle != nullptr)
        {
            return vertexBundle->clone();
        }
        return nullptr;
    }
    void SObject::setVertexBundle(const VertexBundle &vb)
    {
        // TIP: 此处必须要clone()（原型模式），否则后续对vb的操作会影响到sobject的vb
        this->vertexBundle = std::shared_ptr<VertexBundle>(vb.clone());
    }
    void SObject::transformVertex(const glm::mat4 &mat)
    {
        if (this->vertexBundle != nullptr)
        {
            vertexBundle->transform(mat);
        }
    }
    void SObject::reset()
    {
        this->setSelfModleMat(glm::mat4(1));
    }
    SObject *SObject::clone() const
    {
        auto tmp = new SObject();
        tmp->setSelfModleMat(selfModelMat);
        if (vertexBundle)
        {
            tmp->setVertexBundle(*vertexBundle);
        }

        for (auto child : children)
        {
            auto ctmp = std::shared_ptr<SObject>(child->clone());
            tmp->addChild(ctmp);
        }
        return tmp;
    }
    shared_ptr<SObject> SObject::getChild(unsigned ind)
    {
        if (ind >= children.size())
            return children[ind];
        return nullptr;
    }
    // static create
    SObject *SObject::createLightCube(const Texture &texture1, const Texture &texture2, const Texture &texture3, const Texture &texture4, const Texture &texture5, const Texture &texture6)
    {
        auto tmp = new SObject();
        auto vb = TextureLightVB::createRectangle(texture1);
        auto one = glm::mat4(1);
        //Front
        vb->transform(glm::translate(one, glm::vec3(0, 0, -0.5)));
        tmp->addChild(shared_ptr<SObject>(new SObject(*vb)));

        //Right
        vb->transform(glm::rotate(one, glm::radians(90.0f), glm::vec3(0, 1, 0)));
        vb->texture = texture2;
        tmp->addChild(shared_ptr<SObject>(new SObject(*vb)));

        //Back
        vb->transform(glm::rotate(one, glm::radians(90.0f), glm::vec3(0, 1, 0)));
        vb->texture = texture3;
        tmp->addChild(shared_ptr<SObject>(new SObject(*vb)));

        //Left
        vb->transform(glm::rotate(one, glm::radians(90.0f), glm::vec3(0, 1, 0)));
        vb->texture = texture4;
        tmp->addChild(shared_ptr<SObject>(new SObject(*vb)));

        //Top
        vb->transform(glm::rotate(one, glm::radians(90.0f), glm::vec3(0, 1, 0)));
        vb->transform(glm::rotate(one, glm::radians(90.0f), glm::vec3(1, 0, 0)));
        vb->texture = texture5;
        tmp->addChild(shared_ptr<SObject>(new SObject(*vb)));

        //Bottom
        vb->transform(glm::rotate(one, glm::radians(180.0f), glm::vec3(1, 0, 0)));
        vb->texture = texture6;
        tmp->addChild(shared_ptr<SObject>(new SObject(*vb)));
        return tmp;
    }
    SObject *SObject::createLightCube(const Texture &texture)
    {
        return createLightCube(texture,texture,texture,texture,texture,texture);
    }
    SObject *SObject::createLightCube(const Texture &textureTop,const Texture &textureSide,const Texture &textureBottom) {
        return createLightCube(textureSide,textureSide,textureSide,textureSide,textureTop,textureBottom);
    }  
    SObject *SObject::createCube(const Texture &texture)
    {
        auto tmp = new SObject();
        auto vb = TextureVB::createRectangle(texture);
        auto one = glm::mat4(1);
        vb->transform(glm::translate(one, glm::vec3(0, 0, -0.5)));
        for (int i = 1; i <= 4; i++)
        {
            tmp->addChild(shared_ptr<SObject>(new SObject(*vb)));
            vb->transform(glm::rotate(one, glm::radians(90.0f), glm::vec3(0, 1, 0)));
        }
        vb->transform(glm::rotate(one, glm::radians(90.0f), glm::vec3(1, 0, 0)));
        tmp->addChild(shared_ptr<SObject>(new SObject(*vb)));
        vb->transform(glm::rotate(one, glm::radians(180.0f), glm::vec3(1, 0, 0)));
        tmp->addChild(shared_ptr<SObject>(new SObject(*vb)));
        return tmp;
    }

}
