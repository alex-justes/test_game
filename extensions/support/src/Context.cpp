#include "support/Context.h"
#include "Log.h"

using namespace extensions::support::context;


bool Decal::update()
{
    if (_changed)
    {
        if (drawable() != nullptr)
        {
            render_shape() = AABB(drawable()->bounding_box().top_left + position(),
                                  drawable()->bounding_box().bottom_right + position());
        }
        _changed = false;
        return true;
    }
    return false;
}
void Decal::set_position(const Point &pos)
{
    _changed = true;
    position() = pos;
}
