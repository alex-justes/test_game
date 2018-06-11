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
        };

        class AutoMovableObject :
                public virtual core::basic::object::UpdatableObject,
                public virtual extensions::basic::object::RenderableObjectExt,
                public virtual extensions::basic::behavior::Velocity,
                public core::basic::actor::Evaluate,
                public extensions::basic::actor::Move
        {
        public:
            void evaluate(uint32_t time_elapsed) override;
            bool update(bool force) override;
            void move(uint32_t time) override;
        private:
            ForceVector _offset{0, 0};
        };

        class AutoDyingObject :
                public virtual core::basic::object::Object,
                public virtual extensions::basic::behavior::LifeTime,
                public core::basic::actor::Evaluate
        {
        public:
            void evaluate(uint32_t time_elapsed) override;
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
                public virtual AutoDyingObject
        {
        public:
            void evaluate(uint32_t time_elapsed) override;
        };

        // TODO: type traits
        // T should be AutoMovable

        class ParticleGenerator
        {
        public:
            virtual void generate() = 0;
        };

        template<class Particle>
        class RadialParticleGenerator :
                public ParticleGenerator,
                public virtual core::basic::object::Object,
                public virtual core::basic::behavior::Position
        {
        public:
            using DrawableGenerator = helpers::generator::DrawableGenerator;
            RadialParticleGenerator();
            constexpr static bool check_traits();
            void set_drawable_generator(std::unique_ptr<DrawableGenerator>&& generator);
            void set_direction_range(int32_t from, int32_t to);
            void set_speed_range(int32_t from, int32_t to);
            void set_particle_count_range(int32_t from, int32_t to);
            void set_life_time_range(int32_t from, int32_t to);
            void set_size_range(const Size& from, const Size& to);
            void generate() override;
        private:
            PointI32 _direction{0, 0};
            PointI32 _speed{0, 0};
            PointI32 _particles{0, 0};
            PointI32 _life_time{0 ,0};
            Size _size_lo{0, 0}, _size_hi{0, 0};
            std::unique_ptr<DrawableGenerator> _drawable_generator;
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
            return true;
            //return std::is_base_of_v<AutoDyingObject, Particle> && std::is_base_of_v<AutoMovableObject, Particle>;
        }

        template<class Particle>
        RadialParticleGenerator<Particle>::RadialParticleGenerator()
        {
            static_assert(check_traits());
        }

        template<class Particle>
        void RadialParticleGenerator<Particle>::set_drawable_generator(
                std::unique_ptr<RadialParticleGenerator::DrawableGenerator> &&generator)
        {
            _drawable_generator = std::move(generator);
        }

        template<class Particle>
        void RadialParticleGenerator<Particle>::set_direction_range(int32_t from, int32_t to)
        {
            _direction = {from, to};
        }

        template<class Particle>
        void RadialParticleGenerator<Particle>::set_speed_range(int32_t from, int32_t to)
        {
            _speed = {from, to};
        }

        template<class Particle>
        void RadialParticleGenerator<Particle>::set_particle_count_range(int32_t from, int32_t to)
        {
            _particles = {from, to};
        }

        template<class Particle>
        void RadialParticleGenerator<Particle>::set_life_time_range(int32_t from, int32_t to)
        {
            _life_time = {from, to};
        }

        template<class Particle>
        void RadialParticleGenerator<Particle>::set_size_range(const Size &from, const Size &to)
        {
            _size_lo = from;
            _size_hi = to;
        }

        template<class Particle>
        void RadialParticleGenerator<Particle>::generate()
        {
            if (!_drawable_generator)
            {
                LOG_E("No drawable generator")
            }
            auto amount = helpers::math::RandomIntGenerator::generate_random_int(_particles.x, _particles.y);
            for (int i = 0; i < amount; ++i)
            {
                auto object = world_manager()->create_object<Particle>();
                auto size = helpers::math::RandomIntGenerator::generate_random_size(_size_lo, _size_hi);
                auto drawable = _drawable_generator->generate(size);
                object->set_drawable(std::move(drawable));
                //object->set_life_time(helpers::math::RandomIntGenerator::generate_random_int(_life_time.x, _life_time.y));
                object->set_position(position());
                if constexpr (std::is_base_of_v<core::basic::object::CollidableObject, Particle>)
                {
                    object->set_collision_size(size);
                }
                object->set_direction(helpers::math::RandomIntGenerator::generate_random_int(_direction.x, _direction.y));
                object->set_speed(helpers::math::RandomIntGenerator::generate_random_int(_speed.x, _speed.y));
            }
        }

    }
}

#endif //EXTENSIONS_SUPPORT_OBJECTS_H
