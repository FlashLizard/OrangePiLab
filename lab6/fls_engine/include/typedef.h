#ifndef TYPEDEF_H
#define TYPEDEF_H

#include <memory>
#include <algorithm>
#include <vector>

namespace fls_engine
{
    class Engine;
    class Component;
    class CameraComponent;
    class GObject;
    class Scene;
    typedef void (*DevInit)(void);
    typedef void (*UpdateScreen)(int *src);
    typedef void (*FreshScreen)(void);
    typedef int (*GetTime)(void);

    template<typename T>
    std::shared_ptr<T> getComponent(const std::vector<std::shared_ptr<Component> > &components)
    {
        auto it = std::find_if(components.begin(), components.end(),
                               [](const auto &component)
                               {
                                   return dynamic_cast<T *>(component.get()) != nullptr;
                               });

        if (it != components.end())
        {
            return std::static_pointer_cast<T>(*it);
        }
        else
        {
            return nullptr;
        }
    }
}

#endif