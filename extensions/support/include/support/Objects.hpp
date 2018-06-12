#ifndef EXTENSIONS_SUPPORT_OBJECTS_H
#define EXTENSIONS_SUPPORT_OBJECTS_H

#include "core/BasicObjects.h"
#include "helpers/BasicContext.hpp"
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
        public:
            Wall();
        };

        class Villain
        {
        };

        class AutoMovableObject :
                public virtual core::basic::object::UpdatableObject,
                public virtual extensions::basic::object::RenderableObjectExt,
                public virtual extensions::basic::behavior::Velocity,
                public virtual core::basic::actor::Evaluate,
                public virtual extensions::basic::actor::Move
        {
        public:
            void evaluate(uint32_t time_elapsed) override;
            bool update(bool force) override;
            void move(uint32_t time) override;
        private:
            ForceVector _offset{0, 0};
        };

        class Projectile :
                public virtual AutoMovableObject,
                public virtual extensions::basic::object::CollidableObjectExt
        {
        public:
            void initialize() override;
            void evaluate(uint32_t time_elapsed) override;
            bool update(bool force) override;
        };


        class AutoDyingObject :
                public virtual core::basic::object::Object,
                public virtual extensions::basic::behavior::LifeTime,
                public virtual core::basic::actor::Evaluate,
                public virtual core::basic::actor::Die
        {
        public:
            void evaluate(uint32_t time_elapsed) override;
            void die() override;
        };

        // TODO: type traits
        template<class T>
        class BouncableObject :
                public virtual AutoMovableObject,
                public virtual extensions::basic::object::CollidableObjectExt,
                public virtual extensions::complex::behavior::GoodPosition
        {
        public:
            void initialize() override;
            void evaluate(uint32_t time_elapsed) override;
        protected:
        public:
            bool update(bool force) override;
        protected:
            CollidedSides find_collided_sides(const core::basic::object::CollidableObject *object);
        };

        using WallBouncer = BouncableObject<Wall>;

        class AutoDyingWallBouncer :
                public virtual WallBouncer,
                public virtual AutoDyingObject,
                public virtual core::basic::actor::Evaluate
        {
        public:
            void evaluate(uint32_t time_elapsed) override;
        };

        class ParticleGenerator :
                public virtual core::basic::object::Object,
                public virtual core::basic::behavior::Position
        {
        public:
            virtual uint32_t generate() = 0;
            virtual uint32_t generate(const Point &position) = 0;
            virtual void set_generator_direction(float angle) = 0;
            virtual void set_generator_direction(const ForceVector &direction) = 0;
        };

        template<class Particle>
        class RadialParticleGenerator :
                public virtual ParticleGenerator
        {
        public:
            using DrawableGenerator = helpers::generator::DrawableGenerator;
            RadialParticleGenerator();
            constexpr static bool check_traits();
            void set_drawable_generator(std::unique_ptr<DrawableGenerator> &&generator);
            void set_direction_range(float angle);
            void set_speed_range(float from, float to);
            void set_particle_count_range(int32_t from, int32_t to);
            void set_life_time_range(int32_t from, int32_t to);
            void set_size_range(const Size &width, const Size &height);
            void set_generator_direction(float angle) override;
            void set_generator_direction(const ForceVector &direction) override;

            uint32_t generate(const Point &position) override;
            uint32_t generate() override;
        private:
            float generate_speed();
            float generate_direction();
            uint32_t generate_lifetime();
            uint32_t generate_amount();
            Size generate_size();

            float _direction{0};
            ForceVector _direction_range{0, 0};
            ForceVector _speed_range{0, 0};
            PointI32 _amount_range{0, 0};
            PointI32 _life_time_range{0, 0};
            Size _size_width_range{0, 0}, _size_height_range{0, 0};
            std::unique_ptr<DrawableGenerator> _drawable_generator;
            std::mt19937 _random_generator;
            std::normal_distribution<float> _life_time_dist;
            std::uniform_real_distribution<float> _direction_dist;
            std::normal_distribution<float> _speed_dist;
            std::normal_distribution<float> _amount_dist;
            std::normal_distribution<float> _size_width_dist;
            std::normal_distribution<float> _size_height_dist;
        };

        class Exhaust :
                public virtual AutoMovableObject,
                public virtual AutoDyingObject
        {
        public:
            void evaluate(uint32_t time_elapsed) override;
        };

        class Rocket :
                public virtual RadialParticleGenerator<Exhaust>,
                public virtual Projectile,
                public virtual core::basic::actor::Die
        {
        public:
            Rocket(int32_t timeout);
            void initialize() override;
            void evaluate(uint32_t time_elapsed) override;
            bool update(bool force) override;
            void die() override;
        protected:
            int32_t _timer {500};
            int32_t _timeout;
        };

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
            if (dead())
            {
                return;
            }
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
                            LOG_D("Mr. Anderson had no place in matrix... So... CS.... ")
                            set_dead();
                            break;
                        }
                        collided_sides |= find_collided_sides(object);
                    }
                }
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
                AutoMovableObject::evaluate(time_elapsed);
            }
        }
        template<class T>
        void BouncableObject<T>::initialize()
        {
            AutoMovableObject::initialize();
            CollidableObjectExt::initialize();
        }
        template<class T>
        bool BouncableObject<T>::update(bool force)
        {
            if (force || changed())
            {
                AutoMovableObject::update(force);
                set_collision_shape({position(), position() + collision_size()});
                set_changed(false);
                return true;
            }
            return false;
        }

        template<class Particle>
        constexpr bool RadialParticleGenerator<Particle>::check_traits()
        {
            return std::is_base_of_v<AutoMovableObject, Particle>;
        }

        template<class Particle>
        RadialParticleGenerator<Particle>::RadialParticleGenerator()
        {
            static_assert(check_traits());
            std::random_device device;
            _random_generator = std::mt19937(device());
        }

        template<class Particle>
        void RadialParticleGenerator<Particle>::set_drawable_generator(
                std::unique_ptr<RadialParticleGenerator::DrawableGenerator> &&generator)
        {
            _drawable_generator = std::move(generator);
        }

        template<class Particle>
        void RadialParticleGenerator<Particle>::set_direction_range(float angle)
        {
            _direction_range = {0.f, angle};
            _direction_dist = decltype(_direction_dist)(0, angle);
        }

        template<class Particle>
        void RadialParticleGenerator<Particle>::set_speed_range(float from, float to)
        {
            _speed_range = {from, to};
            float average = to + (to - from) / 2.f;
            _speed_dist = decltype(_speed_dist)(average, average / 2.f);
        }

        template<class Particle>
        void RadialParticleGenerator<Particle>::set_particle_count_range(int32_t from, int32_t to)
        {
            _amount_range = {from, to};
            float average = to + (to - from) / 2.f;
            _amount_dist = decltype(_amount_dist)(average, average / 4.f);
        }

        template<class Particle>
        void RadialParticleGenerator<Particle>::set_life_time_range(int32_t from, int32_t to)
        {
            _life_time_range = {from, to};
            float average = to + (to - from) / 2.f;
            _life_time_dist = decltype(_life_time_dist)(average, average);
        }

        template<class Particle>
        void RadialParticleGenerator<Particle>::set_generator_direction(float angle)
        {
            _direction = std::clamp(angle, 0.f, 360.f);
        }

        template<class Particle>
        void RadialParticleGenerator<Particle>::set_generator_direction(const ForceVector &direction)
        {
            auto angle = std::atan2(direction.y, direction.x);
            _direction = helpers::math::rad2deg(angle);
        }

        template<class Particle>
        void RadialParticleGenerator<Particle>::set_size_range(const Size &width, const Size &height)
        {
            _size_width_range = width;
            _size_height_range = height;
            float average_width = width.x + (width.y - width.x) / 2.f;
            float average_height = height.x + (height.y - height.x) / 2.f;
            _size_width_dist = decltype(_size_width_dist)(average_width, average_width / 6.f);
            _size_height_dist = decltype(_size_height_dist)(average_height, average_height / 6.f);
        }

        template<class Particle>
        float RadialParticleGenerator<Particle>::generate_speed()
        {
            return std::clamp(_speed_dist(_random_generator), _speed_range.x, _speed_range.y);
        }

        template<class Particle>
        float RadialParticleGenerator<Particle>::generate_direction()
        {
            float angle = std::clamp(_direction_dist(_random_generator), _direction_range.x, _direction_range.y);
            float average = (_direction_range.y - _direction_range.x) / 2.f;
            return _direction + angle - average;
        }

        template<class Particle>
        uint32_t RadialParticleGenerator<Particle>::generate_lifetime()
        {
            return std::clamp((uint32_t) (_life_time_dist(_random_generator)), (uint32_t) _life_time_range.x,
                              (uint32_t) _life_time_range.y);
        }

        template<class Particle>
        uint32_t RadialParticleGenerator<Particle>::generate_amount()
        {
            return std::clamp((uint32_t) (_amount_dist(_random_generator)), (uint32_t) _amount_range.x,
                              (uint32_t) _amount_range.y);
        }

        template<class Particle>
        Size RadialParticleGenerator<Particle>::generate_size()
        {
            return {
                    std::clamp((uint32_t) (_size_width_dist(_random_generator)), _size_width_range.x,
                               _size_width_range.y),
                    std::clamp((uint32_t) (_size_height_dist(_random_generator)), _size_height_range.x,
                               _size_height_range.y)
            };
        }

        template<class Particle>
        uint32_t RadialParticleGenerator<Particle>::generate()
        {
            return generate(position());
        }

        template<class Particle>
        uint32_t RadialParticleGenerator<Particle>::generate(const Point &position)
        {
            if (!_drawable_generator)
            {
                LOG_E("No drawable generator")
            }
            auto amount = generate_amount();

            for (uint32_t i = 0; i < amount; ++i)
            {
                auto object = world_manager()->create_object<Particle>();
                auto size = generate_size();
                auto drawable = _drawable_generator->generate(size);
                object->set_drawable(std::move(drawable));
                if constexpr (std::is_base_of_v<AutoDyingObject, Particle>)
                {
                    object->set_life_time(generate_lifetime());
                }
                object->set_position(position);
                if constexpr (std::is_base_of_v<core::basic::object::CollidableObject, Particle>)
                {
                    object->set_collision_size(size);
                }
                object->set_direction(generate_direction());
                object->set_speed(generate_speed());
            }
            return amount;
        }

    }
}

#endif //EXTENSIONS_SUPPORT_OBJECTS_H
