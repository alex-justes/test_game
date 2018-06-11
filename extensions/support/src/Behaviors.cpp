

#include "support/Behaviors.h"
#include "support/Utilities.h"

using namespace extensions::complex::behavior;
using namespace extensions::basic::behavior;

const Point &GoodPosition::good_position() const
{
    return _good_position;
}

bool GoodPosition::good_position_known() const
{
    return _good_position_known;
}

void GoodPosition::save_good_position()
{
    _good_position_known = true;
    _good_position = position();
}


const ForceVector& Velocity::velocity() const
{
    return _velocity;
}

void Velocity::set_velocity(const ForceVector &velocity)
{
    _velocity = velocity;
}

void Velocity::set_velocity(float direction, float magnitude)
{
    float angle = helpers::math::deg2rad(direction);
    _velocity = {std::cos(angle), std::sin(angle)};
    _velocity *= magnitude;
}