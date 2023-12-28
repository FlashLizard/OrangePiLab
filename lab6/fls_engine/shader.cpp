#include "shader.h"
#include <cmath>
#include <stdio.h>

using std::vector;
namespace fls_engine
{
    int getColor(float r, float g, float b)
    {
        unsigned int rr = 255 * r;
        unsigned int gg = 255 * g;
        unsigned int bb = 255 * b;
        rr = std::min(rr,255u);
        gg = std::min(gg,255u);
        bb = std::min(bb,255u);
        return (int)(0xff000000 | (rr << 16) | (gg << 8) | bb);
    }
    int getColor(glm::vec3 color) {
        return getColor(color.r,color.g,color.b);
    }
    glm::vec3 tofColor(int color)
    {
        float b = (color&0x000000ff)/255.0;
        color>>=8;
        float g = (color&0x000000ff)/255.0;
        color>>=8;
        float r = (color&0x000000ff)/255.0;
        return glm::vec3(r,g,b);
    }
    ShaderRunner::ShaderRunner(int width, int height, int zoomX, int zoomY)
    {
        // TIP: 与屏幕像素数一致
        colorBuffer = (int *)malloc(height * width * sizeof(int));
        exBuffer.resize(height*width);
        this->zoomX = zoomX;
        this->zoomY = zoomY;
        height /= zoomY;
        width /= zoomX;
        this->height = height;
        this->width = width;
        // TIP: 与缩放后的像素数一致
        depthBuffer = (float *)malloc(height * width * sizeof(float));
    }
    ShaderRunner::ShaderRunner(int width, int height) : ShaderRunner(width, height, 1, 1) {}
    void ShaderRunner::runOnce(BaseShader *shaderProgram)
    {
        this->shaderProgram = shaderProgram;
        // TIP: generate vertexes in NDC
        auto vertexs = shaderProgram->genVertexes();
        auto elements = shaderProgram->genElements();
        auto exInfo = shaderProgram->genExInfo();

        for (auto it = vertexs.begin(); it != vertexs.end(); it++)
        {
            it->divideW();
            it->toScreenPos(this->width,this->height);
            it->addFloatAttr(it->z());
        }
        for (int i = 0; i + 2 < elements.size(); i += 3)
        {
            Vertex *a = &vertexs[elements[i]];
            Vertex *b = &vertexs[elements[i + 1]];
            Vertex *c = &vertexs[elements[i + 2]];
            
            if(!(a->checkZ()&&b->checkZ()&&c->checkZ())) continue;
            if(!(a->checkXY(width,height)||b->checkXY(width,height)||c->checkXY(width,height))) {continue;}
            
            if (b->y() < a->y())
                std::swap(a, b);
            if (c->y() < a->y())
                std::swap(a, c);
            if (c->y() < b->y())
                std::swap(c, b);

            //  drawTop
            drawHalfTriangle(*a, *b, *c, exInfo);
            // drawBottom
            drawHalfTriangle(*c, *b, *a, exInfo);

        }
    }
    void ShaderRunner::clearBuffer(int color, float depth)
    {
        vector<int> tmp;
        for (int i = 0; i < height; i++)
        {
            for (int j = 0; j < width; j++)
            {
                int ind = i * width + j;
                drawChunk(j, i, color,tmp);
                this->depthBuffer[ind] = depth;
            }
        }
    }
    void ShaderRunner::drawHalfTriangle(const Vertex &a, const Vertex &b, const Vertex &c,const vector<int> &exInfo)
    {

        int ax = a.x(), ay = a.y();
        int bx = b.x(), by = b.y();
        int cx = c.x(), cy = c.y();

        float invSlope1 = static_cast<float>(bx - ax) / (by - ay);
        float invSlope2 = static_cast<float>(cx - ax) / (cy - ay);
        int startY = std::max(0, ay < by ? ay : by);
        int endY = std::min(height - 1, ay > by ? ay : by);
        float denominator = getDenominator(ax, ay, bx, by, cx, cy);
        for (int y = startY; y <= endY; ++y)
        {
            int startX = ax + static_cast<int>((y - ay) * invSlope1);
            int endX = ax + static_cast<int>((y - ay) * invSlope2);

            if (startX > endX)
                std::swap(startX, endX);
            startX = std::max(startX, 0);
            endX = std::min(width - 1, endX);
            for (int x = startX; x <= endX; ++x)
            {
                float aw, bw, cw;
                barycentricCoordinates(ax, ay, bx, by, cx, cy, x, y, denominator, aw, bw, cw);
                int zi = a.fAttrs.size() - 1;
                float z = aw * a.fAttrs[zi] + bw * b.fAttrs[zi] + cw * c.fAttrs[zi];
                int zInd = y * width + x;
                if (depthBuffer[zInd] > z)
                {
                    depthBuffer[zInd] = z;
                    std::vector<float> fAttrs;
                    for (int i = 0; i < (int)a.fAttrs.size() - 1; i++)
                    {

                        float attr = aw * a.fAttrs[i] + bw * b.fAttrs[i] + cw * c.fAttrs[i];
                        fAttrs.push_back(attr);
                    }
                    fAttrs.push_back(z);
                    drawChunk(x, y, fAttrs, exInfo);
                }
            }
        }
    }
    void ShaderRunner::drawChunk(int x, int y, const vector<float> &fAttrs,const vector<int> &exInfo)
    {
        int color = this->shaderProgram->runFrag(x, y, fAttrs);
        int scrWidth = zoomX * width;
        int size = scrWidth * zoomY * height;
        y *= zoomY;
        x *= zoomX;
        for (int i = 0; i < zoomY; i++, y++)
        {
            for (int j = 0; j < zoomX; j++)
            {
                int ind = std::min(y * scrWidth + x + j, size - 1);
                colorBuffer[ind] = color;
                exBuffer[ind] = exInfo;
            }
        }
    }
    void ShaderRunner::drawChunk(int x, int y, int color,const vector<int> &exInfo)
    {
        int scrWidth = zoomX * width;
        int size = scrWidth * zoomY * height;
        y *= zoomY;
        x *= zoomX;
        for (int i = 0; i < zoomY; i++, y++)
        {
            for (int j = 0; j < zoomX; j++)
            {
                int ind = std::min(y * scrWidth + x + j, size - 1);
                colorBuffer[ind] = color;
                exBuffer[ind] = exInfo;
            }
        }
    }
    std::vector<Vertex> BaseShader::genVertexes()
    {
        return std::vector<Vertex>();
    }
    std::vector<int> BaseShader::genElements()
    {
        return std::vector<int>();
    }
    vector<int> BaseShader::genExInfo()
    {
        return vector<int>();
    }
    int BaseShader::runFrag(int x, int y, std::vector<float> const &fAttrs)
    {
        return 0;
    }
}