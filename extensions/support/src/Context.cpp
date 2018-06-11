#include "support/Context.h"
#include "Log.h"
#include "support/Utilities.h"

using namespace extensions::support::context;

void Decal::set_position(const Point &pos)
{
    set_changed(true);
    Position::set_position(pos);
}

void Wall::set_position(const Point &position)
{
    set_changed(true);
    Position::set_position(position);
}

void MovableObject::set_position(const Point &position)
{
    set_changed(true);
    Position::set_position(position);
}

void MovableObject::set_direction(const DirectionVector &vector)
{
    _direction_normalized = helpers::math::normalize(vector);
}

bool MovableObject::move()
{
    if (_velocity > 0 && _direction_normalized.length() > 0)
    {
        return move(direction() * velocity());
    }
    return false;
}

bool MovableObject::move(const DirectionVector &vector)
{
    _offset += vector;
    auto dx = (int32_t) (std::round(_offset.x));
    auto dy = (int32_t) (std::round(_offset.y));
    auto x = position().x;
    auto y = position().y;
    bool ret = false;
    if (std::abs(dx) >= 1)
    {
        x += dx;
        _offset.x = 0;
        ret = true;
    }
    if (std::abs(dy) >= 1)
    {
        y += dy;
        _offset.y = 0;
        ret = true;
    }
    set_position({x, y});
    return ret;

}

const Point &MovableObject::previous_position() const
{
    return _previous_pos;
}
void MovableObject::save_previous_position()
{
    _previous_pos_known = true;
    _previous_pos = position();
//    LOG_D("Prev: %d %d", _previous_pos.x, _previous_pos.y)
//    LOG_D("Shape: %d %d %d %d", collision_shape().top_left.xction CommentByLineComment, collision_shape().top_left.y, collision_shape().bottom_right.x, collision_shape().bottom_right.y)
}

void MovableObject::set_velocity(float velocity)
{
    _velocity = std::abs(velocity / 1000.f);
}

void MovableObject::set_direction(float deg)
{
    auto angle = helpers::math::deg2rad(deg);
    _direction_normalized = helpers::math::normalize({std::cos(angle), std::sin(angle)});
}

float MovableObject::velocity() const
{
    return _velocity;
}

const DirectionVector &MovableObject::direction() const
{
    return _direction_normalized;
}

bool MovableObject::previous_position_known() const
{
    return _previous_pos_known;
}

bool SelfMovableObject::move(const DirectionVector &vector)
{
    return MovableObject::move(vector);
}

bool SelfMovableObject::move()
{
    return MovableObject::move();
}

bool SelfMovableObject::act(uint32_t time_elapsed)
{
    return move(direction() * velocity() * time_elapsed);
}

bool Projectile::act(uint32_t time_elapsed)
{
    bool status = true;
    for (const auto &obj: collisions())
    {
        auto wall = dynamic_cast<const Wall *>(obj);
        if (wall != nullptr)
        {
            set_dead();
            status = false;
            break;
        }
    }
    return status && SelfMovableObject::act(time_elapsed);
}

struct CollidedSides
{
    std::array<bool, 4> sides{false};
    bool &top{sides[0]};
    bool &left{sides[1]};
    bool &bottom{sides[2]};
    bool &right{sides[3]};
    bool bad{true};

    CollidedSides &operator|=(const CollidedSides &rhs)
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
};

CollidedSides
find_collided_sides(const extensions::support::context::MovableObject *who, const helpers::context::CollidableObject *whom)
{
    if (who == nullptr || whom == nullptr)
    {
        return CollidedSides{};
    }

    if (!who->previous_position_known())
    {
        return CollidedSides{};
    }

    PointI32 position = who->position();
    PointI32 previous_position = who->previous_position();
    auto shape_who = who->collision_shape();
    auto shape_whom = whom->collision_shape();

    auto sides = CollidedSides{};

    DirectionVector offset = position - previous_position;
    float collision_offset = 0.5f;
    float top_y = 0;
    float bottom_y = 0;
    float left_x = 0;
    float right_x = 0;
    float distance_y = 0;
    float distance_x = 0;
    float object_y = 0;
    float object_x = 0;
    if (offset.y > 0)
    {
        top_y = shape_who.bottom_right.y - offset.y - collision_offset;
        bottom_y = shape_who.bottom_right.y;
        object_y = shape_whom.top_left.y;
        distance_y = object_y - top_y;
    } else
    {
        top_y = shape_who.top_left.y;
        bottom_y = shape_who.top_left.y - offset.y + collision_offset;
        object_y = shape_whom.bottom_right.y;
        distance_y = bottom_y - object_y;
    }

    if (offset.x > 0)
    {
        left_x = shape_who.bottom_right.x - offset.x - collision_offset;
        right_x = shape_who.bottom_right.x;
        object_x = shape_whom.top_left.x;
        distance_x = object_x - left_x;
    } else
    {
        left_x = shape_who.top_left.x;
        right_x = shape_who.bottom_right.x + collision_offset;
        object_x = shape_whom.bottom_right.x;
        distance_x = right_x - object_x;
    }

    bool possible_vertical_collision = object_y >= top_y && object_y <= bottom_y;
    bool possible_horizontal_collision = object_x >= left_x && object_x <= right_x;

    if (possible_vertical_collision)
    {
        float x1;
        float x2;
        //if (helpers::math::intersects_1d(x1, x2, shape_whom.top_left.x, shape_whom.bottom_right.x))
        {
            sides.bad = false;
            if (offset.y > 0)
            {
                sides.bottom = true;
                LOG_D("Bottom! %.3f", distance_y)
            } else
            {
                sides.top = true;
                LOG_D("Top! %.3f", distance_y)
            }
        }
    }

    if (possible_horizontal_collision)
    {
        float y1;
        float y2;
        //if (helpers::math::intersects_1d(y1, y2, shape_whom.top_left.y, shape_whom.bottom_right.y))
        {
            sides.bad = false;
            if (offset.x > 0)
            {
                sides.right = true;
                LOG_D("Right!")
            } else
            {
                sides.left = true;
                LOG_D("Left!")
            }
        }
    }

    return sides;
}


bool WallBouncer::act(uint32_t time_elapsed)
{
    bool status = true;
    bool collided_with_walls = false;

    if (!collisions().empty())
    {
        CollidedSides collided_sides;
        for (const auto &object: collisions())
        {
            auto wall = dynamic_cast<const Wall *>(object);
            if (wall != nullptr)
            {
                collided_sides |= find_collided_sides(this, wall);
                if (collided_sides.bad)
                {
                    LOG_W("Mr. Anderson had no place in matrix... So... CS.... ")
                    set_dead();
                    status = false;
                    break;
                }
            }
        }
        if (!collided_sides.bad)
        {
            int collided_sides_count = std::accumulate(std::begin(collided_sides.sides), std::end(collided_sides.sides),
                                                       0, [](int sum, bool v)
                                                       { return sum + (v ? 1 : 0); });
            collided_with_walls = collided_sides_count > 0;
            if (collided_sides_count == 1)
            {
                if (collided_sides.top || collided_sides.bottom)
                {
                    set_direction({direction().x, -direction().y});
                }
                else
                {
                    set_direction({-direction().x, direction().y});
                }
            }
            else if (collided_sides_count > 1)
            {
                set_direction(-direction());
            }
        }
    }
    if (!collided_with_walls)
    {
        save_previous_position();
    }
    else
    {
        set_position(previous_position());
    }
    return status && SelfMovableObject::act(time_elapsed);
}

