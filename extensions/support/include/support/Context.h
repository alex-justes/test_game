#ifndef EXTENSIONS_SUPPORT_CONTEXT_H
#define EXTENSIONS_SUPPORT_CONTEXT_H

#include "helpers/BasicContext.h"
#include "support/Types.h"

namespace extensions::support::context
{
    // Non-collidable
    class Decal :
            public virtual helpers::context::RenderableObject
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
                          public virtual helpers::context::CollidableObject
    {
    public:
        void set_position(const Point &position) override;
        bool update() override;
    };

    class MovableObject :
            public virtual helpers::context::CollidableObject,
            public virtual helpers::context::RenderableObject
    {
    public:
        virtual void move(const DirectionVector& vector);
        const Point& get_previous_position() const;
        void set_position(const Point &position) override;
        bool update() override;
    private:
        Point _previous_pos;
    };

    class Projectile :
            public virtual MovableObject,
            public virtual core::actor::Actor
    {
    public:
        bool act() override;
    };
}

#endif //EXTENSIONS_SUPPORT_CONTEXT_H
