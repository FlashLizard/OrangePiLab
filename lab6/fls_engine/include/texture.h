#ifndef TEXTURE_H
#define TEXTURE_H
#include "string.h"
#include <memory>
#include <iostream>

namespace fls_engine
{
    class Texture
    {
    private:
        std::shared_ptr<const int> content;
    public:
        int width;
        int height;
        Texture(int width, int height, const int *content)
        {
            this->width = width;
            this->height = height;
            auto tmp = new int[width * height];
            memcpy(tmp, content, width * height * sizeof(int));
            this->content = std::shared_ptr<const int>(tmp);
        }
        Texture() {}
        const int operator[](int ind) const
        {
            return *(content.get() + ind);
        }
    };
}
#endif