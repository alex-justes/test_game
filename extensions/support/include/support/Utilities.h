#ifndef EXTENSIONS_SUPPORT_UTILITIES_H
#define EXTENSIONS_SUPPORT_UTILITIES_H

#include "Types.h"

namespace helpers::math
{
    DirectionVector normalize(const DirectionVector &vector);
    float deg2rad(float deg);
    bool intersects_1d(float n_left, float n_right, float m_left, float m_right);

    class RandomGenerator
    {
    public:
    private:

    };
}

#endif //EXTENSIONS_SUPPORT_UTILITIES_H
