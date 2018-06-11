#ifndef EXTENSIONS_SUPPORT_UTILITIES_H
#define EXTENSIONS_SUPPORT_UTILITIES_H

#include <random>
#include "Types.h"
#include "core/Types.h"

#define EPS 1.e-10f

namespace helpers::math
{
    ForceVector normalize(const ForceVector &vector);
    float deg2rad(float deg);
    float rad2deg(float rad);
    bool intersects_1d(float n_left, float n_right, float m_left, float m_right);

    class RandomIntGenerator
    {
    public:
        static int generate_random_int(int lo, int hi);
        static Point generate_random_point(const Roi &roi);
        static Size generate_random_size(const Size &dim_x, const Size &dim_y);
    private:
        RandomIntGenerator() = default;
        static std::random_device _random_device;
        static std::mt19937 _random_generator;
    };
}

#endif //EXTENSIONS_SUPPORT_UTILITIES_H
