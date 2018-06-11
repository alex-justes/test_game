#ifndef EXTENSIONS_SUPPORT_BEHAVIORS_H
#define EXTENSIONS_SUPPORT_BEHAVIORS_H

#include "core/BasicBehaviors.hpp"
#include "Types.h"

namespace extensions::basic::behavior
{
    class Velocity :
            public virtual core::basic::behavior::IBehavior
    {
    public:
        ForceVector velocity() const;
        const ForceVector &direction() const;
        float direction_angle() const;
        float speed() const;
        void set_direction(const ForceVector &direction);
        void set_direction(float direction);
        void set_speed(float speed);
        void set_velocity(const ForceVector &velocity);
        void set_velocity(float direction, float speed);
        void set_velocity(const ForceVector &direction, float speed);
    private:
        ForceVector _direction;
        float _speed;
    };

    class LifeTime :
            public virtual core::basic::behavior::IBehavior
    {
    public:
        int32_t life_time() const;
        void set_life_time(int32_t time);
    private:
        int32_t _life_time;
    };
}

namespace extensions::complex::behavior
{
    class GoodPosition :
            public virtual core::basic::behavior::Position
    {
    public:
        const Point &good_position() const;
        bool good_position_known() const;
        virtual void save_good_position();
    private:
        Point _good_position;
        bool _good_position_known{false};
    };
}


#endif //EXTENSIONS_SUPPORT_BEHAVIORS_H
