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
    };

    // Wall. Just. Wall.
    // Non-renderable wall.
    class Wall : public virtual helpers::context::CollidableObject
    {
    public:
        void set_position(const Point &position) override;
    };

    class MovableObject :
            public virtual helpers::context::UpdatableObject,
            public virtual core::behavior::Position
    {
    public:
        const Point &previous_position() const;
        void set_position(const Point &position) override;
        void set_direction(const DirectionVector &vector);
        void set_direction(float deg);
        void set_velocity(float velocity);
        float velocity() const;
        const DirectionVector &direction() const;
        bool previous_position_known() const;
        virtual bool move(const DirectionVector &vector);
        virtual bool move();
    protected:
        void save_previous_position();
    private:
        Point _previous_pos;
        bool _previous_pos_known{false};
        DirectionVector _direction_normalized{0, 0};
        float _velocity{0};
        DirectionVector _offset{0, 0};

    };

    class SelfMovableObject :
            public virtual MovableObject,
            public virtual core::actor::Actor
    {
    public:
        bool act(uint32_t time_elapsed) override;
    protected:
        bool move(const DirectionVector &vector) override;
        bool move() override;
    };

    class WallBouncer :
            public virtual helpers::context::CollidableObject,
            public virtual helpers::context::RenderableObject,
            public virtual SelfMovableObject
    {
    public:
        bool act(uint32_t time_elapsed) override;
    };

    class Projectile :
            public virtual helpers::context::CollidableObject,
            public virtual helpers::context::RenderableObject,
            public virtual SelfMovableObject
    {
    public:
        bool act(uint32_t time_elapsed) override;
    };

}

#endif //EXTENSIONS_SUPPORT_CONTEXT_H
