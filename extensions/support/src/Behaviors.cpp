

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


ForceVector Velocity::velocity() const
{
    return _direction * _speed;
}
const ForceVector &Velocity::direction() const
{
    return _direction;
}
float Velocity::direction_angle() const
{
    auto angle = std::atan2(_direction.y, _direction.x);
    if (angle < 0)
    {
        angle = (float)(M_PI - angle);
    }
    return helpers::math::rad2deg(angle);
}
float Velocity::speed() const
{
    return _speed;
}
void Velocity::set_direction(const ForceVector &direction)
{
    _direction = helpers::math::normalize(direction);
}
void Velocity::set_direction(float direction)
{
    auto angle = helpers::math::deg2rad(direction);
    _direction = helpers::math::normalize({std::cos(angle), std::sin(angle)});
}
void Velocity::set_speed(float speed)
{
    _speed = speed/1000.f;
}
void Velocity::set_velocity(const ForceVector &velocity)
{
    _speed = velocity.length()/1000.f;
    _direction = helpers::math::normalize(velocity);
}
void Velocity::set_velocity(float direction, float speed)
{
    set_direction(direction);
    set_speed(speed);
}
void Velocity::set_velocity(const ForceVector &direction, float speed)
{
    _direction = helpers::math::normalize(direction);
    _speed = speed/1000.f;
}

int32_t LifeTime::life_time() const
{
    return _life_time;
}

void LifeTime::set_life_time(int32_t time)
{
    _life_time = time;
}