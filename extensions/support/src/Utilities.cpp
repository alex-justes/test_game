#include "support/Utilities.h"

DirectionVector helpers::math::normalize(const DirectionVector &vector)
{
    return vector/vector.length();
}

float helpers::math::deg2rad(float deg)
{
    return (float)(deg * M_PI / 180.f);
}
