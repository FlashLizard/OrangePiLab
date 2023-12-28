#ifndef SHADER
#define SHADER
#include <vector>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "vertex_bundle.h"
#include "texture.h"
#include "sobject.h"

namespace fls_engine
{
    int getColor(float r, float g, float b);
    int getColor(glm::vec3 color);
    glm::vec3 tofColor(int color);
    class BaseShader
    {
    public:
        glm::mat4 model;
        glm::mat4 view;
        glm::mat4 projection;
        std::vector<Vertex> vertexes;
        std::vector<int> elements;
        BaseShader(){};
        virtual std::vector<Vertex> genVertexes();
        virtual std::vector<int> genElements();
        virtual std::vector<int> genExInfo();
        virtual int runFrag(int x, int y, std::vector<float> const &fAttrs);
        virtual ~BaseShader() {}
        virtual void setWolrdLigth(glm::vec4 pos) {}
        virtual void loadVertexBundle(const VertexBundle *vb)
        {
            this->vertexes = vb->getVertexes();
            this->elements = vb->getElements();
        };
    };
    class ShaderRunner
    {
    private:
        int height;
        int width;
        int zoomX;
        int zoomY;
        float *depthBuffer;
        BaseShader *shaderProgram;

    public:
        int *colorBuffer;
        std::vector<std::vector<int> > exBuffer;

        ShaderRunner(int width, int height, int zoomX, int zoomY);
        ShaderRunner(int width, int height);
        void runOnce(BaseShader *shaderProgram);
        void clearBuffer(int color, float depth);
        float getDenominator(int ax, int ay, int bx, int by, int cx, int cy)
        {
            return (by - cy) * (ax - cx) + (cx - bx) * (ay - cy);
        }
        void barycentricCoordinates(int ax, int ay, int bx, int by, int cx, int cy, int px, int py,
                                    float denominator, float &w0, float &w1, float &w2)
        {
            w0 = ((by - cy) * (px - cx) + (cx - bx) * (py - cy)) / denominator;
            w1 = ((cy - ay) * (px - cx) + (ax - cx) * (py - cy)) / denominator;
            w2 = 1.0f - w0 - w1;
        }
        void drawHalfTriangle(const Vertex &a, const Vertex &b, const Vertex &c,const std::vector<int> &exInfo);
        void drawChunk(int x, int y, const std::vector<float> &fAttrs,const std::vector<int> &exInfo);
        void drawChunk(int x, int y, int color,const std::vector<int> & exInfo);
        ~ShaderRunner()
        {
            free(colorBuffer);
            free(depthBuffer);
        }
    };

    class OneColorShader : public BaseShader
    {
    public:
        enum
        {
            Z = 0,
        };
        int color;

        OneColorShader()
        {
            view = projection = model = glm::mat4(1);
        }
        virtual std::vector<Vertex> genVertexes() override
        {
            glm::mat4 mvp = projection * view * model;
            std::vector<Vertex> tmp;
            for (auto it = vertexes.begin(); it != vertexes.end(); it++)
            {
                auto v = it;
                tmp.push_back(Vertex(mvp * v->pos, v->fAttrs));
            }
            return tmp;
        }
        virtual std::vector<int> genElements() override
        {
            return elements;
        }
        virtual int runFrag(int x, int y, std::vector<float> const &fAttrs) override
        {
            return color;
        }
        virtual void loadVertexBundle(const VertexBundle *vb)
        {

            if (vb->getType() != VertexBundle::ONE_COLOR)
            {
                std::cout << "Error: OneColorShader load unfit VertexBundle" << std::endl;
            }
            else
            {
                auto ocvb = static_cast<const OneColorVB *>(vb);
                this->color = ocvb->color;
                this->vertexes = ocvb->getVertexes();
                this->elements = ocvb->getElements();
            }
        };
    };

    class LerpColorShader : public BaseShader
    {
    public:
        enum
        {
            R = 0,
            G,
            B,
            Z
        };

        LerpColorShader()
        {
            view = projection = model = glm::mat4(1);
        }
        virtual std::vector<Vertex> genVertexes() override
        {
            glm::mat4 mvp = projection * view * model;
            std::vector<Vertex> tmp;
            for (auto it = vertexes.begin(); it != vertexes.end(); it++)
            {
                auto v = it;
                tmp.push_back(Vertex(mvp * v->pos, v->fAttrs));
            }
            return tmp;
        }
        virtual std::vector<int> genElements() override
        {
            return elements;
        }
        virtual int runFrag(int x, int y, std::vector<float> const &fAttrs) override
        {
            return getColor(fAttrs[0], fAttrs[1], fAttrs[2]);
        }
    };

    class TextureShader : public BaseShader
    {
    protected:
        Texture texture;

    public:
        enum
        {
            U = 0,
            V,
            Z
        };

        TextureShader()
        {
            view = projection = model = glm::mat4(1);
        }
        virtual std::vector<Vertex> genVertexes() override
        {
            glm::mat4 mvp = projection * view * model;
            std::vector<Vertex> tmp;
            for (auto it = vertexes.begin(); it != vertexes.end(); it++)
            {
                auto v = it;
                tmp.push_back(Vertex(mvp * v->pos, v->fAttrs));
            }
            return tmp;
        }
        virtual std::vector<int> genElements() override
        {
            return elements;
        }
        virtual int runFrag(int x, int y, std::vector<float> const &fAttrs) override
        {
            int width = texture.width;
            int height = texture.height;
            int ix = fAttrs[0] * (width - 1), iy = fAttrs[1] * (height - 1);
            if (ix < 0 || ix >= width)
                return getColor(0, 0, 0);
            if (iy < 0 || iy >= height)
                return getColor(0, 0, 0);
            return texture[iy * width + ix];
        }
        virtual void loadVertexBundle(const VertexBundle *vb)
        {

            if (vb->getType() != VertexBundle::TEXTURE_VB)
            {
                std::cout << "Error: TextureShader load unfit VertexBundle" << std::endl;
            }
            else
            {
                auto tvb = static_cast<const TextureVB *>(vb);
                this->texture = tvb->texture;
                this->vertexes = tvb->getVertexes();
                this->elements = tvb->getElements();
            }
        };
    };
    class TextureLightShader : public TextureShader
    {
    public:

        glm::vec4 lightPos;

        glm::vec3 ambient;
        glm::vec3 diffuse;
        glm::vec3 specular;
        float shininess;

        enum
        {
            U = 0,
            V,
            NX,
            NY,
            NZ,
            VX,
            VY,
            VZ,
            Z // only in frag
        };
        TextureLightShader()
        {
            view = projection = model = glm::mat4(1);
            this->ambient = glm::vec3(0.3);
            this->diffuse = glm::vec3(0.6);
            this->specular = glm::vec3(1);
            this->shininess = 4.0f;
            this->lightPos = glm::vec4(0,0,-1,0);
        }
        virtual void setWolrdLigth(glm::vec4 pos) {
            lightPos = pos;
        }
        virtual std::vector<Vertex> genVertexes() override
        {
            glm::mat4 mvp = projection * view * model;
            glm::mat4 mv4 = view * model;
            glm::mat4 mv = glm::mat4(glm::mat3(view * model));
            glm::mat3 normalMat = glm::transpose(glm::inverse(glm::mat3(mv)));
            std::vector<Vertex> tmp;
            for (auto it = vertexes.begin(); it != vertexes.end(); it++)
            {
                auto v = it;
                auto nv = Vertex(mv4 * v->pos, v->fAttrs);
                nv.fAttrs.resize(Z); 
                
                nv.fAttrs[VX] = nv.x();
                nv.fAttrs[VY] = nv.y();
                nv.fAttrs[VZ] = nv.z();
                nv.pos = projection * nv.pos;
                glm::vec3 normal = glm::vec3(nv.fAttrs[NX], nv.fAttrs[NY], nv.fAttrs[NZ]);
                normal = normalMat * normal;
                nv.fAttrs[NX] = normal.x;
                nv.fAttrs[NY] = normal.y;
                nv.fAttrs[NZ] = normal.z;
                // Z will auto add
                tmp.push_back(nv);
            }
            return tmp;
        }
        virtual std::vector<int> genElements() override
        {
            return elements;
        }
        virtual std::vector<int> genExInfo() override
        {
            std::vector<int> exInfo;
            if(vertexes.size()!=4) return exInfo;
            auto pos = glm::vec3((vertexes[0].pos+vertexes[1].pos+vertexes[2].pos+vertexes[3].pos)*0.25f);
            auto a= vertexes[0];
            auto normal = glm::normalize(glm::vec3(a.fAttrs[NX], a.fAttrs[NY], a.fAttrs[NZ]));
            pos+=normal*0.5f;
            int x = 128+pos.x;
            int y = 128+pos.y;
            int z = 128+pos.z;
            exInfo.push_back(x|(y<<8)|(z<<16));
            pos-=normal;
            x = 128+pos.x;
            y = 128+pos.y;
            z = 128+pos.z;
            exInfo.push_back(x|(y<<8)|(z<<16));
            return exInfo;
        }
        virtual int runFrag(int x, int y, std::vector<float> const &fAttrs) override
        {
            int width = texture.width;
            int height = texture.height;
            int ix = fAttrs[0] * (width - 1), iy = fAttrs[1] * (height - 1);
            if (ix < 0) ix = 0;
            if(ix>=width) ix = width-1;
            if (iy < 0) iy = 0;
            if(iy>=height) iy = height-1;
            auto tcolor = tofColor(texture[iy*width+ix]);
            glm::vec3 lightVec;
            glm::vec3 vPos = glm::vec3(fAttrs[VX], fAttrs[VY], fAttrs[VZ]);
            if (lightPos.w == 0.0)
                lightVec = glm::normalize(glm::vec3(lightPos));
            else
                lightVec = glm::normalize(glm::vec3(lightPos) - vPos);

            glm::vec3 viewVec = -glm::normalize(vPos);
            glm::vec3 normal = glm::normalize(glm::vec3(fAttrs[NX], fAttrs[NY], fAttrs[NZ]));
            glm::vec3 reflect = glm::reflect(-lightVec, normal);

            float N_dot_L = glm::max(0.0f, glm::dot(normal, lightVec));
            float R_dot_V = glm::max(0.0f, glm::dot(reflect, viewVec));
            auto ambientColor = ambient * tcolor;
            auto diffuseColor = diffuse * tcolor * N_dot_L;
            auto specColor = specular * tcolor * (float)pow(R_dot_V, shininess);
            return  getColor(ambientColor + diffuseColor + specColor);
        }
        virtual void loadVertexBundle(const VertexBundle *vb)
        {

            if (vb->getType() != VertexBundle::TEXTURE_LIGHT_VB)
            {
                std::cout << "Error: TextureLightShader load unfit VertexBundle" << std::endl;
            }
            else
            {
                auto tlvb = static_cast<const TextureLightVB *>(vb);
                this->texture = tlvb->texture;
                this->vertexes = tlvb->getVertexes();
                this->elements = tlvb->getElements();
                this->ambient = tlvb->ambient;
                this->diffuse = tlvb->diffuse;
                this->specular = tlvb->specular;
                this->shininess = tlvb->shininess;
            }
        };
        virtual void setLightPosInView(glm::vec4 pos) {
            this->lightPos = pos;
        }
    };
}
#endif