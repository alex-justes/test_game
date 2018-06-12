#ifndef EXTENSIONS_SUPPORT_ACTORS_H
#define EXTENSIONS_SUPPORT_ACTORS_H

#include <cstdint>
#include "core/BasicActors.h"

namespace extensions::basic::actor
{
    class Move : public virtual core::basic::actor::IActor
    {
    public:
        virtual void move(uint32_t time) = 0;
    };

}

#endif //EXTENSIONS_SUPPORT_ACTORS_H
