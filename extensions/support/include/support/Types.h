#ifndef EXTENSIONS_SUPPORT_TYPES_H
#define EXTENSIONS_SUPPORT_TYPES_H

#include "helpers/Containers.hpp"

using ForceVector = helpers::containers::Vector2D<float>;

struct CollidedSides
{
    std::array<bool, 4> sides{false};
    bool &top{sides[0]};
    bool &left{sides[1]};
    bool &bottom{sides[2]};
    bool &right{sides[3]};
    bool bad{true};
    CollidedSides &operator|=(const CollidedSides &rhs);
};

#endif //EXTENSIONS_SUPPORT_TYPES_H
