#ifndef EXTENSIONS_SUPPORT_OBJECTS_H
#define EXTENSIONS_SUPPORT_OBJECTS_H

#include "core/BasicObjects.h"

namespace extensions
{
    namespace basic::object
    {
        class CollidableObjectExt :
                public virtual core::basic::object::CollidableObject
        {
        public:
            void initialize() override;
        };

        class RenderableObjectExt :
                public virtual core::basic::object::RenderableObject
        {
        public:
            void initialize() override;
        };
    }

    namespace complex::object
    {
        // Non-collidable
        class Decal : public virtual extensions::basic::object::RenderableObjectExt
        {
        };

        // Wall. Just. Invisible. Wall.
        class Wall : public virtual extensions::basic::object::CollidableObjectExt
        {
        };

        class Projectile :
                public virtual extensions::basic::object::RenderableObjectExt,
                public virtual extensions::basic::object::CollidableObjectExt
        {
        public:
            void initialize() override;
        };
    }
}

#endif //EXTENSIONS_SUPPORT_OBJECTS_H
