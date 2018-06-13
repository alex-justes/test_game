#include "support/Objects.hpp"

using namespace extensions::complex::object;
using namespace extensions::basic::object;

Wall::Wall()
{
    set_static_shape(true);
}

void CollidableObjectExt::initialize()
{
    set_collision_shape({position(), position() + collision_size()});
}

void RenderableObjectExt::initialize()
{
    if (drawable() != nullptr)
    {
        set_render_shape({position(), position() + drawable()->bounding_box().bottom_right});
    }
}


void AutoMovableObject::evaluate(uint32_t time_elapsed)
{
    this->move(time_elapsed);
}

bool AutoMovableObject::update(bool force)
{
    if (force || changed())
    {
        if (drawable() != nullptr)
        {
            set_render_shape({position(), position() + drawable()->bounding_box().bottom_right});
        }
        return true;
    }
    return false;
}

void AutoMovableObject::move(uint32_t time)
{
    _offset += velocity() * time;
    auto dx = (int32_t) (std::round(_offset.x));
    auto dy = (int32_t) (std::round(_offset.y));
    auto new_position = position();
    if (std::abs(dx) >= 1)
    {
        new_position.x += dx;
        _offset.x = 0;
    }
    if (std::abs(dy) >= 1)
    {
        new_position.y += dy;
        _offset.y = 0;
    }
    if (new_position != position())
    {
        set_changed(true);
        set_position(new_position);
    }
}

void AutoDyingObject::evaluate(uint32_t time_elapsed)
{
    auto remaining_time = remaining_life_time() - (int32_t) (time_elapsed);
    if (remaining_time < 0)
    {
        set_dead();
    }
    else
    {
        set_remaining_life_time(remaining_time);
        auto renderable = dynamic_cast<core::basic::object::RenderableObject *>(this);
        if (renderable != nullptr)
        {
            auto drawable_with_fade = dynamic_cast<core::basic::actor::Fade *>(renderable->get_drawable());
            if (drawable_with_fade != nullptr)
            {
                float percent = 1.f - 1.f / (life_time() * remaining_life_time());
                drawable_with_fade->fade(percent);
            }
        }
    }
}
void AutoDyingObject::die()
{

}

void AutoDyingWallBouncer::evaluate(uint32_t time_elapsed)
{
    AutoDyingObject::evaluate(time_elapsed);
    BouncableObject::evaluate(time_elapsed);
}

void Projectile::evaluate(uint32_t time_elapsed)
{
    if (!collisions().empty())
    {
        for (const auto &object: collisions())
        {
            auto wall = dynamic_cast<const Wall *>(object);
            if (wall != nullptr)
            {
                set_dead();
                break;
            }
            auto villain = dynamic_cast<const Villain *>(object);
            if (villain != nullptr)
            {
                set_dead();
                break;
            }
        }
    }
    if (!dead())
    {
        AutoMovableObject::evaluate(time_elapsed);
    }
}
void Projectile::initialize()
{
    AutoMovableObject::initialize();
    CollidableObjectExt::initialize();
}
bool Projectile::update(bool force)
{
    if (force || changed())
    {
        set_collision_shape({position(), position() + collision_size()});
        AutoMovableObject::update(true);
        set_changed(false);
        return true;
    }
    return false;
}

Rocket::Rocket(int32_t timeout)
: _timeout(timeout),
  _timer(-1)
{

}

void Rocket::initialize()
{
    Projectile::initialize();
    AutoMovableObject::initialize();
    auto drawable_generator = new helpers::generator::SimpleDrawableRectGenerator;
    drawable_generator->set_fill_color({255, 0, 0, 128});
    drawable_generator->set_border_color({0, 0, 0, 0});
    set_drawable_generator(std::unique_ptr<helpers::generator::DrawableGenerator>(drawable_generator));
    set_direction_range(90);
    set_generator_direction(-direction());
    set_speed_range(speed()*1.8f*_timeout, speed()*2.2f*_timeout);
    set_particle_count_range(10, 20);
    set_size_range({2, 6}, {2, 6});
    set_life_time_range(1000, 1500);
    //set_position((collision_shape().bottom_right - collision_shape().top_left) / 2);
}

void Rocket::evaluate(uint32_t time_elapsed)
{
    _timer -= time_elapsed;
    if (_timer < 0)
    {
        generate(position() + Point{render_shape().width()/2, render_shape().height()/2});
        _timer = _timeout;
    }
    Projectile::evaluate(time_elapsed);
}
bool Rocket::update(bool force)
{
    if (force || changed())
    {
        Projectile::update(true);
        //set_position((collision_shape().bottom_right - collision_shape().top_left) / 2);
        set_changed(false);
        return true;
    }
    return false;
}
void Rocket::die()
{
    set_direction_range(360);
    set_speed_range(speed()*2.5f*_timeout, speed()*4.f*_timeout);
    set_particle_count_range(30, 40);
    generate(position() + (collision_shape().bottom_right - collision_shape().top_left)/2);
}


void Exhaust::evaluate(uint32_t time_elapsed)
{
    AutoDyingObject::evaluate(time_elapsed);
    AutoMovableObject::evaluate(time_elapsed);
}

void Hero::initialize()
{
    RenderableObjectExt::initialize();
    CollidableObjectExt::initialize();
}

bool Hero::update(bool force)
{
    if (force || changed())
    {
        AutoMovableObject::update(true);
        set_collision_shape({position(), position() + collision_size()});
        return true;
    }
    return false;
}

void Hero::evaluate(uint32_t time_elapsed)
{
    bool collided_with_walls = false;
    if (!collisions().empty())
    {
        for (const auto& object: collisions())
        {
            if (dynamic_cast<const Wall *>(object))
            {
                collided_with_walls = true;
                break;
            }
        }
    }
    if (collided_with_walls)
    {
        set_position(good_position());
    }
    else
    {
        save_good_position();
        AutoMovableObject::evaluate(time_elapsed);
    }
}
