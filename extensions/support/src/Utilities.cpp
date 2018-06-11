#include "support/Utilities.h"
#include "Log.h"

ForceVector helpers::math::normalize(const ForceVector &vector)
{
    if (vector.length() < 0.00000001f)
    {
        LOG_E("WOW")
    }
    return vector/vector.length();
}

float helpers::math::deg2rad(float deg)
{
    return (float)(deg * M_PI / 180.f);
}

// TODO: make it more explicit
bool helpers::math::intersects_1d(float n_left, float n_right, float m_left, float m_right)
{
    return !(m_right < n_left || m_left > n_right);
}
