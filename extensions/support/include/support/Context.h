#ifndef EXTENSIONS_SUPPORT_CONTEXT_H
#define EXTENSIONS_SUPPORT_CONTEXT_H

#include "helpers/BasicContext.h"

namespace extensions::support::context
{
    // Non-collidable
    class Decal :
            public helpers::context::RenderableObject
    {
    public:
        void set_position(const Point &pos) override;
        bool update() override;
    };

    // Wall. Just. Wall.
    class Wall
    {
    };

    // Non-renderable wall.
    class InvisibleWall : public Wall,
                          public helpers::context::CollidableObject
    {
        bool update() override;
    public:
        void set_position(const Point &position) override;
    };
}

#endif //EXTENSIONS_SUPPORT_CONTEXT_H
