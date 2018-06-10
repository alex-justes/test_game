#include "support/Context.h"
#include "Log.h"

using namespace extensions::support::context;


bool Decal::update()
{
    if (changed())
    {
        if (drawable() != nullptr)
        {
            render_shape() = AABB(drawable()->bounding_box().top_left + position(),
                                  drawable()->bounding_box().bottom_right + position());
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
