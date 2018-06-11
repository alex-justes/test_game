#include "support/Utilities.h"
#include "Log.h"

using namespace helpers::math;

std::random_device RandomIntGenerator::_random_device = std::random_device();
std::mt19937 RandomIntGenerator::_random_generator = std::mt19937(RandomIntGenerator::_random_device());

ForceVector helpers::math::normalize(const ForceVector &vector)
{
    if (vector.length() < EPS)
    {
        return ForceVector{0, 0};
    }
    return vector/vector.length();
}

float helpers::math::deg2rad(float deg)
{
    return (float)(deg * M_PI / 180.f);
}

float helpers::math::rad2deg(float rad)
{
    return (float)(rad * 180.f / M_1_PI);
}

// TODO: make it more explicit
bool helpers::math::intersects_1d(float n_left, float n_right, float m_left, float m_right)
{
    return !(m_right < n_left || m_left > n_right);
}

int RandomIntGenerator::generate_random_int(int lo, int hi)
{
    std::uniform_int_distribution<int> x(lo, hi);
    return x(_random_generator);
}
Point RandomIntGenerator::generate_random_point(const Roi &roi)
{
    std::uniform_int_distribution<uint32_t> x(roi.top_left.x, roi.bottom_right.x);
    std::uniform_int_distribution<uint32_t> y(roi.top_left.y, roi.bottom_right.y);
    return {x(_random_generator), y(_random_generator)};
}

Size RandomIntGenerator::generate_random_size(const Size &dim_x, const Size &dim_y)
{
    std::uniform_int_distribution<uint32_t> x(dim_x.x, dim_x.y);
    std::uniform_int_distribution<uint32_t> y(dim_y.x, dim_y.y);
    return {x(_random_generator), y(_random_generator)};
}


std::unique_ptr<core::drawable::Drawable>
helpers::generator::SimpleDrawableRectGenerator::generate(const Size &box_size)
{
    auto rect = new core::drawable::DrawableRect();
    rect->set_box_size(box_size);
    rect->set_fill_color(fill_color());
    rect->set_border_color(border_color());
    return std::unique_ptr<core::drawable::Drawable>(rect);
}
