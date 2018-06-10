#ifndef EXTENSIONS_SUPPORT_CONTEXT_H
#define EXTENSIONS_SUPPORT_CONTEXT_H

#include "helpers/BasicContext.h"

namespace extensions::support::context
{
    class Decal :
            public helpers::context::RenderableObject,
            public core::behavior::Updatable
    {
    public:
        void set_position(const Point &pos);
        bool update() override;
    private:
        bool _changed{true};
    };

}

#endif //EXTENSIONS_SUPPORT_CONTEXT_H
