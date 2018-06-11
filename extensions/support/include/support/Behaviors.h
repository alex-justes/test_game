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
        const ForceVector& velocity() const;
        void set_velocity(const ForceVector& velocity);
        void set_velocity(float direction, float magnitude);
    private:
        ForceVector _velocity;
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
