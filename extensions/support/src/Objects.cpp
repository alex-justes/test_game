#include "support/Objects.h"

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

void Projectile::initialize()
{
    CollidableObjectExt::initialize();
    RenderableObjectExt::initialize();
}
