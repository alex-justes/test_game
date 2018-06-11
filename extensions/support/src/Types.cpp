#include "support/Types.h"

CollidedSides &CollidedSides::operator|=(const CollidedSides &rhs)
{
    if (this != &rhs)
    {
        for (int i = 0; i < 4; ++i)
        {
            sides[i] = sides[i] || rhs.sides[i];
        }
        bad = rhs.bad;
    }
    return *this;
}