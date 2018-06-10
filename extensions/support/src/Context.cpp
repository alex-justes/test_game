#include "support/Context.h"
#include "Log.h"
#include "support/Utilities.h"

using namespace extensions::support::context;

void Decal::set_position(const Point &pos)
{
    set_changed(true);
    Position::set_position(pos);
}

void InvisibleWall::set_position(const Point &position)
{
    set_changed(true);
    Position::set_position(position);
}

bool MovableObject::update(bool force)
{
    if (collisions().empty())
    {
        save_previous_position();
    }
    if (force || changed())
    {
        CollidableObject::update(true);
        RenderableObject::update(true);
        return true;
    }
    return false;
}

void MovableObject::set_position(const Point &position)
{
    set_changed(true);
    Position::set_position(position);
}

void MovableObject::set_direction(const DirectionVector &vector)
{
    _direction_normalized = helpers::math::normalize(vector);
}

bool MovableObject::move()
{
    if (_velocity > 0 && _direction_normalized.length() > 0)
    {
        return move(direction()*velocity());
    }
    return false;
}

bool MovableObject::move(const DirectionVector &vector)
{
    _offset += vector;
    auto dx = (int32_t)(std::round(_offset.x));
    auto dy = (int32_t)(std::round(_offset.y));
    auto x = position().x;
    auto y = position().y;
    bool ret = false;
    if (std::abs(dx) >= 1)
    {
        x += dx;
        _offset.x = 0;
        ret = true;
    }
    if (std::abs(dy) >= 1)
    {
        y += dy;
        _offset.y = 0;
        ret = true;
    }
    set_position({x, y});
    return ret;

}

const Point& MovableObject::get_previous_position() const
{
    return _previous_pos;
}
void MovableObject::save_previous_position()
{
    _previous_pos = position();
}

void MovableObject::set_velocity(float velocity)
{
    _velocity = std::abs(velocity/1000.f);
}

void MovableObject::set_direction(float deg)
{
    auto angle = helpers::math::deg2rad(deg);
    _direction_normalized = helpers::math::normalize({std::cos(angle), std::sin(angle)});
}

float MovableObject::velocity() const
{
    return _velocity;
}

const DirectionVector& MovableObject::direction() const
{
    return _direction_normalized;
}

bool SelfMovableObject::move(const DirectionVector &vector)
{
    return MovableObject::move(vector);
}

bool SelfMovableObject::move()
{
    return MovableObject::move();
}

bool SelfMovableObject::act(uint32_t time_elapsed)
{
    return move(direction()*velocity()*time_elapsed);
}

bool Projectile::act(uint32_t time_elapsed)
{
    for (const auto& obj: collisions())
    {
        auto wall = dynamic_cast<const Wall*>(obj);
        if (wall != nullptr)
        {
            set_dead();
            break;
        }
    }
    return SelfMovableObject::act(time_elapsed);
}
