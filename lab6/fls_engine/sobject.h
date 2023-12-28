#ifndef SOBJECT
#define SOBJECT

#include "vertex_bundle.h"
#include <glm/glm.hpp>
#include <vector>
#include <set>
#include <memory>

namespace fls_engine
{
    class SObject
    {
    protected:
        static unsigned int cnt;
        unsigned int id;
        //TIP: 这里能确保父亲消失时其所有子节点也会消失，所以父亲节点不可能释放后再被子节点访问，所以可以为普通指针。
        SObject* parent;
        glm::mat4 selfModelMat;
        std::shared_ptr<VertexBundle> vertexBundle;

        void setParent(SObject *parent);

    public:
        std::vector<std::shared_ptr<SObject> > children;
        SObject();
        SObject(const VertexBundle &vb);
        unsigned int getId() const;
        std::vector<std::shared_ptr<VertexBundle> > getVertexBundles() const;
        void setSelfModleMat(const glm::mat4 &mat);
        void transform(const glm::mat4 &mat);
        void addChild(std::shared_ptr<SObject> child);
        std::shared_ptr<SObject> removeChild(SObject* child);
        std::shared_ptr<SObject> removeChild(const unsigned id);
        // meanwhile, will remove this from previous parent
        void changeParent(SObject *parent, std::shared_ptr<SObject> self);
        VertexBundle* getVertexBundle() const;
        void setVertexBundle(const VertexBundle &vb);
        void transformVertex(const glm::mat4 &mat);
        void reset();
        SObject* clone() const;
        std::shared_ptr<SObject> getChild(unsigned ind);
        virtual ~SObject(){};
        void rightTransform(const glm::mat4 &mat) {
            selfModelMat = selfModelMat*mat;
        }
        glm::mat4 getModelMat() {
            return selfModelMat;
        }
        bool operator == (const SObject &b) const {
            return id==b.getId();
        }

        //staitc sobject
        static SObject* createCube(const Texture &texture);
        static SObject* createLightCube(const Texture &textureTop,const Texture &textureSide,const Texture &textureBottom);
        //front right back left top bottom
        static SObject* createLightCube(const Texture &texture1,const Texture &texture2,const Texture &texture3,const Texture &texture4,const Texture &texture5,const Texture &texture6);
        static SObject* createLightCube(const Texture &texture);
    };

}

#endif