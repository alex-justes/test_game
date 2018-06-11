#include "support/Objects.hpp"

using namespace extensions::complex::object;
using namespace extensions::basic::object;

void CollidableObjectExt::initialize()
{
    set_collision_shape({position(), position() + collision_size()});
}

void RenderableObjectExt::initialize()
{
    if (drawable() != nullptr)
    {
        set_render_shape({position(), position() + drawable()->bounding_box().bottom_right});
    }
}


void AutoMovableObject::evaluate(uint32_t time_elapsed)
{
    this->move(time_elapsed);
}

bool AutoMovableObject::update(bool force)
{
    if (force || changed())
    {
        if (drawable() != nullptr)
        {
            set_render_shape({position(), position() + drawable()->bounding_box().bottom_right});
        }
        return true;
    }
    return false;
}

void AutoMovableObject::move(uint32_t time)
{
    _offset += velocity()*time;
    auto dx = (int32_t) (std::round(_offset.x));
    auto dy = (int32_t) (std::round(_offset.y));
    auto new_position = position();
    if (std::abs(dx) >= 1)
    {
        new_position.x += dx;
        _offset.x = 0;
    }
    if (std::abs(dy) >= 1)
    {
        new_position.y += dy;
        _offset.y = 0;
    }
    if (new_position != position())
    {
        set_changed(true);
        set_position(new_position);
    }
}
