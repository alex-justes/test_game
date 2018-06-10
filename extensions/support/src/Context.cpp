#include "support/Context.h"
#include "Log.h"

using namespace extensions::support::context;


bool Decal::update()
{
    if (changed())
    {
        if (drawable() != nullptr)
        {
            set_render_shape({drawable()->bounding_box().top_left + position(),
                              drawable()->bounding_box().bottom_right + position()});
        }
        set_changed(false);
        return true;
    }
    return false;
}
void Decal::set_position(const Point &pos)
{
    set_changed(true);
    Position::set_position(pos);
}

bool InvisibleWall::update()
{
    if (changed())
    {
        collision_shape() = AABB(position(), position() + collision_size());
        set_changed(false);
        return true;
    }
    return false;
}
void InvisibleWall::set_position(const Point &position)
{
    set_changed(true);
    Position::set_position(position);
}

bool MovableObject::update()
{
    if (changed())
    {
        collision_shape() = AABB(position(), position() + collision_size());
        if (drawable() != nullptr)
        {
            set_render_shape({drawable()->bounding_box().top_left + position(),
                              drawable()->bounding_box().bottom_right + position()});
        }
        set_changed(false);
        return true;
    }
    return false;
}

void MovableObject::set_position(const Point &position)
{
    set_changed(true);
    Position::set_position(position);
}

void MovableObject::move(const DirectionVector &vector)
{
    PointI32 tmp_pos {position().x, position().y};
    tmp_pos += vector;
    _previous_pos = position();
    set_position({std::max(0, tmp_pos.x), std::max(0, tmp_pos.y)});
}

const Point& MovableObject::get_previous_position() const
{
    return _previous_pos;
}

bool Projectile::act()
{
    move({5, 5});
    return true;
}
