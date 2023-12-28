#ifndef VERTEX_BUNDLE
#define VERTEX_BUNDLE
#include <vector>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <set>
#include <memory>
#include "texture.h"


namespace fls_engine
{

    // 拷贝时是值拷贝
    class Vertex
    {
    public:
        glm::vec4 pos;
        std::vector<float> fAttrs;
        Vertex(const float *pos)
        {
            this->pos = glm::make_vec4(pos);
        }
        Vertex(const float *pos, std::vector<float> const &fAttrs)
        {
            this->pos = glm::make_vec4(pos);
            this->fAttrs = fAttrs;
        }
        Vertex(const glm::vec4 &pos)
        {
            this->pos = pos;
        }
        Vertex(const glm::vec4 &pos, std::vector<float> const &fAttrs)
        {
            this->pos = pos;
            this->fAttrs = fAttrs;
        }
        void print(const char *name) const;
        void addFloatAttr(float attr)
        {
            fAttrs.push_back(attr);
        }
        void addFloatAttrs(std::vector<float> const &attrs)
        {
            fAttrs.insert(fAttrs.end(), attrs.begin(), attrs.end());
        }
        void divideW()
        {
            float w = pos[3];
            pos[0] /= w;
            pos[1] /= w;
            pos[2] /= w;
            pos[3] = 1;
        }
        void toScreenPos(int width, int height);
        float x() const
        {
            return pos[0];
        }
        float y() const
        {
            return pos[1];
        }
        float z() const
        {
            return pos[2];
        }
        float w() const
        {
            return pos[3];
        }
        void transform(glm::mat4 mat)
        {
            pos = mat * pos;
        }
        bool checkZ() const
        {
            if (z() < -1 || z() > 1)
                return false;
            return true;
        }
        bool checkXY(int width, int height) const
        {
            if (x() < 0 || x() >= width)
                return false;
            if (y() < 0 || y() >= height)
                return false;
            return true;
        }
    };

    class VertexBundle
    {
    private:
        static unsigned int typeCnt;

    protected:
        std::vector<Vertex> vertexes;
        std::vector<int> elements;
        unsigned int type;

    public:
        unsigned int shaderId;
        enum
        {
            NONE = 0,
            ONE_COLOR,
            LERP_COLOR,
            TEXTURE_VB,
            TEXTURE_LIGHT_VB,
            END
        };
        static int registerType();
        VertexBundle(std::vector<Vertex> const &vertexes, std::vector<int> const &elements)
        {
            this->vertexes = vertexes;
            this->elements = elements;
            this->type = NONE;
            this->shaderId = 0;
        }
        VertexBundle(std::vector<Vertex> const &vertexes, std::vector<int> const &elements, unsigned type)
        {
            this->vertexes = vertexes;
            this->elements = elements;
            this->type = type;
            this->shaderId = 0;
        }
        virtual void transform(const glm::mat4 &mat);
        unsigned getType() const
        {
            return type;
        }
        virtual VertexBundle *clone() const
        {
            auto tmp = new VertexBundle(vertexes, elements);
            return tmp;
        }
        const std::vector<Vertex> &getVertexes() const
        {
            return vertexes;
        }
        const std::vector<int> &getElements() const
        {
            return elements;
        }
        virtual ~VertexBundle(){};
    };

    class OneColorVB : public VertexBundle
    {
    public:
        int color;
        OneColorVB(int color, std::vector<Vertex> const &vertexes, std::vector<int> const &elements) : VertexBundle(vertexes, elements)
        {
            this->type = VertexBundle::ONE_COLOR;
            this->color = color;
        }
        virtual OneColorVB *clone() const
        {
            auto tmp = new OneColorVB(color, vertexes, elements);
            return tmp;
        }

        // create function
        static OneColorVB *createTrangle(int color, const float *a, const float *b, const float *c);
        static OneColorVB *createRectangle(int color, const float *a, const float *b, const float *c, const float *d);
        static OneColorVB *createRectangle(int color);
        static OneColorVB *createCircle(int color, float radius);
        static OneColorVB *createCube(int color);
        static OneColorVB *createBall(int color);
    };
    class LerpColorVB : public VertexBundle
    {
    public:
        LerpColorVB(std::vector<Vertex> const &vertexes, std::vector<std::vector<float> > const &colors, std::vector<int> const &elements) : VertexBundle(vertexes, elements)
        {
            this->type = VertexBundle::LERP_COLOR;
            for (int i = 0; i < vertexes.size(); i++)
            {
                this->vertexes[i].fAttrs = colors[i];
            }
        }
        virtual VertexBundle *clone() const
        {
            auto tmp = new VertexBundle(vertexes, elements, VertexBundle::LERP_COLOR);
            return tmp;
        }
    };

    class TextureVB : public VertexBundle
    {
    protected:
        TextureVB(const Texture &texture, std::vector<Vertex> const &vertexes, std::vector<int> const &elements) : VertexBundle(vertexes, elements)
        {
            this->type = VertexBundle::TEXTURE_VB;
            this->texture = texture;
        }

    public:
        static std::vector<float> texVertex[4];
        Texture texture;
        TextureVB(const Texture &texture, std::vector<Vertex> const &vertexes, std::vector<std::vector<float> > const &texCoords, std::vector<int> const &elements) : VertexBundle(vertexes, elements)
        {
            this->type = VertexBundle::TEXTURE_VB;
            this->texture = texture;
            for (int i = 0; i < vertexes.size(); i++)
            {
                this->vertexes[i].fAttrs = texCoords[i];
            }
        }
        virtual TextureVB *clone() const
        {
            auto tmp = new TextureVB(texture, vertexes, elements);
            return tmp;
        }
        // create function
        static TextureVB *createTrangle(const Texture &texture, const float *a, const float *b, const float *c, const std::vector<int> order);
        static TextureVB *createRectangle(const Texture &texture, const float *a, const float *b, const float *c, const float *d, const std::vector<int> order);
        static TextureVB *createRectangle(const Texture &texture);
    };

    class TextureLightVB : public TextureVB
    {
    protected:
        TextureLightVB(const Texture &texture, std::vector<Vertex> const &vertexes, std::vector<int> const &elements) : TextureVB(texture, vertexes, elements)
        {
            this->type = VertexBundle::TEXTURE_LIGHT_VB;
            resetMaterial();
        }

    public:
        enum
        {
            U = 0,
            V,
            NX,
            NY,
            NZ,
            SIZE
        };
        glm::vec3 ambient;
        glm::vec3 diffuse;
        glm::vec3 specular;
        float shininess;
        TextureLightVB(const Texture &texture, std::vector<Vertex> const &vertexes, std::vector<std::vector<float> > const &texCoords, std::vector<std::vector<float> > const &normals, std::vector<int> const &elements) : TextureVB(texture, vertexes, texCoords, elements)
        {
            this->type = VertexBundle::TEXTURE_LIGHT_VB;
            int cnt = this->vertexes.size();
            for (int i = 0; i < cnt; i++)
            {
                this->vertexes[i].addFloatAttrs(normals[i]);
            }
            resetMaterial();
        }
        TextureLightVB(const Texture &texture, std::vector<Vertex> const &vertexes, std::vector<std::vector<float> > const &texCoords, std::vector<int> const &elements) : TextureVB(texture, vertexes, texCoords, elements)
        {
            this->type = VertexBundle::TEXTURE_LIGHT_VB;
            genNormal();
            resetMaterial();
        }
        TextureLightVB(const TextureVB &tvb) : TextureVB(tvb)
        {
            this->type = VertexBundle::TEXTURE_LIGHT_VB;
            genNormal();
            resetMaterial();
        }
        virtual void resetMaterial()
        {
            this->ambient = glm::vec3(0.7);
            this->diffuse = glm::vec3(1);
            this->specular = glm::vec3(0.4);
            this->shininess = 2.0f;
        }
        virtual void genNormal()
        {
            for (auto &v : this->vertexes)
            {
                v.fAttrs.resize(SIZE);
            }
            int cnt = this->elements.size();
            for (int i = 0; i < cnt - 2; i += 3)
            {
                Vertex &a = this->vertexes[elements[i]];
                Vertex &b = this->vertexes[elements[i + 1]];
                Vertex &c = this->vertexes[elements[i + 2]];
                auto normal = -glm::cross(glm::vec3(b.pos - a.pos), glm::vec3(c.pos - b.pos));
                auto nptr = glm::value_ptr(normal);
                std::copy(nptr, nptr + 3, a.fAttrs.begin() + NX);
                std::copy(nptr, nptr + 3, b.fAttrs.begin() + NX);
                std::copy(nptr, nptr + 3, c.fAttrs.begin() + NX);
            }
        }
        virtual void print() const
        {
            for (int i = 0; i < vertexes.size(); i++)
            {
                std::string name = "1";
                name[0] = ((char)('a' + i));
                vertexes[i].print(name.c_str());
            }
        }
        virtual void transform(const glm::mat4 &mat) override
        {
            for (auto it = vertexes.begin(); it != vertexes.end(); it++)
            {
                it->transform(mat);
                auto normal = glm::vec3(it->fAttrs[NX],it->fAttrs[NY],it->fAttrs[NZ]);
                normal = glm::normalize(glm::transpose(glm::inverse(glm::mat3(mat)))*normal);
                it->fAttrs[NX] = normal.x;
                it->fAttrs[NY] = normal.y;
                it->fAttrs[NZ] = normal.z;
            }
        }
        virtual TextureLightVB *clone() const
        {
            auto tmp = new TextureLightVB(texture, vertexes, elements);
            tmp->ambient = ambient;
            tmp->diffuse = diffuse;
            tmp->specular = specular;
            tmp->shininess = shininess;
            return tmp;
        }
        // create function
        static TextureLightVB *createTrangle(const Texture &texture, const float *a, const float *b, const float *c, const std::vector<int> order);
        static TextureLightVB *createRectangle(const Texture &texture, const float *a, const float *b, const float *c, const float *d, const std::vector<int> order);
        static TextureLightVB *createRectangle(const Texture &texture);
    };
}

#endif