#include "vertex_bundle.h"
#include <cmath>

using std::shared_ptr;
using std::string;

namespace fls_engine
{
    void Vertex::print(const char *name) const
    {
        printf("%s:\npos:%lf %lf %lf %lf\n", name, pos.x, pos.y, pos.z, pos.w);
        for (auto it = fAttrs.begin(); it != fAttrs.end(); it++)
        {
            printf("%lf ", *it);
        }
        puts("\n");
    }

    void Vertex::toScreenPos(int width, int height)
    {
        pos[0] = (int)((1 + pos[0]) / 2 * width);
        pos[1] = (int)((1 - pos[1]) / 2 * height);
    }

    unsigned VertexBundle::typeCnt = VertexBundle::END;
    int VertexBundle::registerType()
    {
        unsigned type = typeCnt++;
        return type;
    }
    void VertexBundle::transform(const glm::mat4 &mat)
    {
        for (auto it = vertexes.begin(); it != vertexes.end(); it++)
        {
            it->transform(mat);
        }
    }

    // 以左上为起点，逆时针给出点
    /* OneColorVB*/
    // TIP: 赋值时左边要求要不是指针要不是引用，否则子类信息会遗失
    OneColorVB *OneColorVB::createTrangle(int color, const float *a, const float *b, const float *c)
    {
        std::vector<int> ele = {0, 1, 2};
        return new OneColorVB(color, std::vector<Vertex>({Vertex(a), Vertex(b), Vertex(c)}), ele);
    }
    OneColorVB *OneColorVB::createRectangle(int color, const float *a, const float *b, const float *c, const float *d)
    {
        return new OneColorVB(color, std::vector<Vertex>({Vertex(a), Vertex(b), Vertex(c), Vertex(d)}), std::vector<int>({0, 1, 2, 0, 2, 3}));
    }
    OneColorVB *OneColorVB::createRectangle(int color)
    {
        float vex[][4] = {
            {-0.5f, 0.5f, 0, 1}, {-0.5f, -0.5f, 0, 1}, {0.5f, -0.5f, 0, 1}, {0.5f, 0.5f, 0, 1}};
        return createRectangle(color, vex[0], vex[1], vex[2], vex[3]);
    }
    OneColorVB *OneColorVB::createCircle(int color, float radius)
    {
        const int cnt = 32;
        std::vector<Vertex> vertexes;
        std::vector<int> elements;
        vertexes.push_back(Vertex({0, 0, 0, 1}));
        float rot = 0;
        float delta = glm::radians(360.0 / cnt);
        for (int i = 0; i < cnt; i++)
        {
            vertexes.push_back(Vertex({radius * std::cos(rot), radius * std::sin(rot), 0, 1}));
            if (i != cnt - 1)
                elements.insert(elements.end(), {0, i + 1, i + 2});
            else
                elements.insert(elements.end(), {0, i + 1, 1});
            rot += delta;
        }
        return new OneColorVB(color, vertexes, elements);
    }
    OneColorVB *OneColorVB::createCube(int color)
    {
        std::vector<Vertex> vertexes = {
            Vertex({-0.5f, 0.5, 0.5, 1}),
            Vertex({-0.5f, -0.5, 0.5, 1}),
            Vertex({0.5f, -0.5, 0.5, 1}),
            Vertex({0.5f, 0.5, 0.5, 1}),
            Vertex({-0.5f, 0.5, -0.5, 1}),
            Vertex({-0.5f, -0.5, -0.5, 1}),
            Vertex({0.5f, -0.5, -0.5, 1}),
            Vertex({0.5f, 0.5, -0.5, 1}),
        };
        std::vector<int> elements = {
            0, 1, 2, 0, 2, 3,
            3, 2, 6, 3, 6, 7,
            7, 6, 5, 7, 5, 4,
            4, 5, 1, 4, 1, 0,
            4, 0, 3, 4, 3, 7,
            1, 5, 6, 1, 6, 2};
        return new OneColorVB(color, vertexes, elements);
    }
    OneColorVB *OneColorVB::createBall(int color)
    {
        std::vector<Vertex> vertexes;
        std::vector<int> elements;
        const int ySegment = 20;
        const int xSegment = 20;
        const float PI = std::acos(-1);
        for (int y = 0; y < ySegment; y++)
        {
            for (int x = 0; x < xSegment; x++)
            {
                float xi = (float)x / (float)(xSegment - 1);
                float yi = (float)y / (float)(ySegment - 1);
                float theta = yi * PI;
                float phi = xi * 2 * PI;
                float xPos = std::sin(theta) * std::cos(phi);
                float yPos = std::cos(theta);
                float zPos = std::sin(theta) * std::sin(phi);
                vertexes.push_back(Vertex({xPos, yPos, zPos, 1}));
            }
        }
        // 生成球的顶点
        for (int i = 0; i < ySegment - 1; i++)
        {
            for (int j = 0; j < xSegment - 1; j++)
            {
                elements.push_back(i * (xSegment) + j);
                elements.push_back((i + 1) * (xSegment) + j);
                elements.push_back((i + 1) * (xSegment) + j + 1);
                elements.push_back(i * (xSegment) + j);
                elements.push_back((i + 1) * (xSegment) + j + 1);
                elements.push_back(i * (xSegment) + j + 1);
            }
        }
        return new OneColorVB(color, vertexes, elements);
    }

    // TextureVB
    std::vector<float> TextureVB::texVertex[4] = {{0, 0}, {0, 1}, {1, 1}, {1, 0}};
    TextureVB *TextureVB::createTrangle(const Texture &texture, const float *a, const float *b, const float *c, const std::vector<int> order)
    {
        std::vector<int> ele = {0, 1, 2};

        return new TextureVB(texture, std::vector<Vertex>({Vertex(a), Vertex(b), Vertex(c)}),
                             std::vector<std::vector<float> >({TextureVB::texVertex[order[0]], TextureVB::texVertex[order[1]], TextureVB::texVertex[order[2]]}),
                             ele);
    }
    TextureVB *TextureVB::createRectangle(const Texture &texture, const float *a, const float *b, const float *c, const float *d, const std::vector<int> order)
    {
        auto texCoords = std::vector<std::vector<float> >();
        texCoords.push_back(texVertex[order[0]]);
        texCoords.push_back(texVertex[order[1]]);
        texCoords.push_back(texVertex[order[2]]);
        texCoords.push_back(texVertex[order[3]]);
        return new TextureVB(texture, std::vector<Vertex>({Vertex(a), Vertex(b), Vertex(c), Vertex(d)}), texCoords, std::vector<int>({0, 1, 2, 0, 2, 3}));
    }
    TextureVB *TextureVB::createRectangle(const Texture &texture)
    {
        float vex[][4] = {
            {-0.5f, 0.5f, 0, 1}, {-0.5f, -0.5f, 0, 1}, {0.5f, -0.5f, 0, 1}, {0.5f, 0.5f, 0, 1}};

        return TextureVB::createRectangle(texture, vex[0], vex[1], vex[2], vex[3], std::vector<int>({0, 1, 2, 3}));
    }

    TextureLightVB *TextureLightVB::createTrangle(const Texture &texture, const float *a, const float *b, const float *c, const std::vector<int> order) {
        auto tmp = TextureVB::createTrangle(texture,a,b,c,order);
        auto result = new TextureLightVB(*tmp);
        delete tmp;
        return result;
    }
    TextureLightVB *TextureLightVB::createRectangle(const Texture &texture, const float *a, const float *b, const float *c, const float *d, const std::vector<int> order) {
        auto tmp = TextureVB::createRectangle(texture,a,b,c,d,order);
        auto result = new TextureLightVB(*tmp);
        delete tmp;
        return result;
    }
    TextureLightVB *TextureLightVB::createRectangle(const Texture &texture) {
        float vex[][4] = {
            {-0.5f, 0.5f, 0, 1}, {-0.5f, -0.5f, 0, 1}, {0.5f, -0.5f, 0, 1}, {0.5f, 0.5f, 0, 1}};

        return createRectangle(texture, vex[0], vex[1], vex[2], vex[3], std::vector<int>({0, 1, 2, 3}));
    }
}