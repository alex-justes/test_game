#ifndef EXTENSIONS_SUPPORT_OBJECTS_H
#define EXTENSIONS_SUPPORT_OBJECTS_H

#include "core/BasicObjects.h"
#include "support/Behaviors.h"
#include "support/Actors.h"
#include "Log.h"
#include "Utilities.h"

namespace extensions
{
    namespace basic::object
    {
        class CollidableObjectExt :
                public virtual core::basic::object::CollidableObject
        {
        public:
            void initialize() override;
        };

        class RenderableObjectExt :
                public virtual core::basic::object::RenderableObject
        {
        public:
            void initialize() override;
        };
    }

    namespace complex::object
    {
        // Non-collidable
        class Decal : public virtual extensions::basic::object::RenderableObjectExt
        {
        };

        // Wall. Just. Invisible. Wall.
        class Wall : public virtual extensions::basic::object::CollidableObjectExt
        {
        };

        class AutoMovableObject :
                public virtual core::basic::object::UpdatableObject,
                public virtual extensions::basic::object::RenderableObjectExt,
                public virtual extensions::basic::object::CollidableObjectExt,
                public virtual extensions::basic::behavior::Velocity,
                public core::basic::actor::Evaluate,
                public extensions::basic::actor::Move
        {
        public:
            void initialize() override;
            void evaluate(uint32_t time_elapsed) override;
            bool update(bool force) override;
            void move(uint32_t time) override;
        private:
            ForceVector _offset {0, 0};
        };

        template<class T>
        class BouncableObject :
                public virtual AutoMovableObject,
                public virtual extensions::complex::behavior::GoodPosition
        {
        public:
            void evaluate(uint32_t time_elapsed) override;
        protected:
            CollidedSides find_collided_sides(const core::basic::object::CollidableObject *object);
        };

        using WallBouncer = BouncableObject<Wall>;


        // =============================================


        // NOTE: it can be moved outside of class
        template<class T>
        CollidedSides BouncableObject<T>::find_collided_sides(const core::basic::object::CollidableObject *object)
        {
            if (object == nullptr || !good_position_known())
            {
                return CollidedSides{};
            }
            PointI32 current_position = position();
            PointI32 previous_position = good_position();
            auto my_shape = collision_shape();
            auto object_shape = object->collision_shape();
            auto sides = CollidedSides{};
            ForceVector offset = current_position - previous_position;
            float collision_offset = 10.f;
            float top_y = 0;
            float bottom_y = 0;
            float left_x = 0;
            float right_x = 0;
            float object_y = 0;
            float object_x = 0;
            if (offset.y > 0)
            {
                top_y = my_shape.bottom_right.y - offset.y - collision_offset;
                bottom_y = my_shape.bottom_right.y;
                object_y = object_shape.top_left.y;
            }
            else
            {
                top_y = my_shape.top_left.y;
                bottom_y = my_shape.top_left.y - offset.y + collision_offset;
                object_y = object_shape.bottom_right.y;
            }
            if (offset.x > 0)
            {
                left_x = my_shape.bottom_right.x - offset.x - collision_offset;
                right_x = my_shape.bottom_right.x;
                object_x = object_shape.top_left.x;
            }
            else
            {
                left_x = my_shape.top_left.x;
                right_x = my_shape.bottom_right.x + collision_offset;
                object_x = object_shape.bottom_right.x;
            }
            bool vertical_collision = object_y >= top_y && object_y <= bottom_y;
            bool horizontal_collision = object_x >= left_x && object_x <= right_x;

            if (vertical_collision)
            {
                sides.bad = false;
                if (offset.y > 0)
                {
                    sides.bottom = true;
                }
                else
                {
                    sides.top = true;
                }
            }
            if (horizontal_collision)
            {

                sides.bad = false;
                if (offset.x > 0)
                {
                    sides.right = true;
                }
                else
                {
                    sides.left = true;
                }
            }
            return sides;
        }

        template<class T>
        void BouncableObject<T>::evaluate(uint32_t time_elapsed)
        {
            bool collided_with_T = false;
            if (!collisions().empty())
            {
                CollidedSides collided_sides;
                for (const auto &object: collisions())
                {
                    if (dynamic_cast<const T *>(object) != nullptr)
                    {
                        collided_with_T = true;
                        if (!good_position_known())
                        {
                            LOG_W("Mr. Anderson had no place in matrix... So... CS.... ")
                            set_dead();
                            break;
                        }
                        collided_sides |= find_collided_sides(object);
                    }
                }
                clear_collisions();
                int collided_sides_count = std::accumulate(std::begin(collided_sides.sides),
                                                           std::end(collided_sides.sides),
                                                           0, [](int sum, bool v)
                                                           { return sum + (v ? 1 : 0); });
                if (collided_with_T && collided_sides.bad)
                {
                    set_direction(helpers::math::RandomIntGenerator::generate_random_int(0, 359));
                }
                else
                {
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
            if (collided_with_T)
            {
                set_position(good_position());
            }
            else
            {
                save_good_position();
            }
            if (!dead())
            {
                move(time_elapsed);
            }
        }
    }
}

#endif //EXTENSIONS_SUPPORT_OBJECTS_H
