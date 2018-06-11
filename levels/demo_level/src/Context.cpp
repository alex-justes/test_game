#include "core/Context.h"
#include "Context.h"
#include "Log.h"
#include "support/Objects.hpp"
#include "support/Utilities.h"

DemoLevel::DemoLevel(core::EventManager &event_manager, core::ScreenManager &screen_manager)
        :
        helpers::context::BasicContext(event_manager, screen_manager)
{

}

void DemoLevel::create_tile(const RGBA &fill_color,
                            const RGBA &border_color,
                            const Size &size,
                            const Point &position)
{
    auto object = world_manager().create_object<extensions::complex::object::Decal>();
    auto rect = object->set_drawable<core::drawable::DrawableRect>();
    rect->size() = size;
    rect->fill_color() = fill_color;
    rect->border_color() = border_color;
    object->set_position(position);
}

void DemoLevel::create_invisible_wall(const Size &size,
                                      const Point &position)
{
    auto object = world_manager().create_object<extensions::complex::object::Wall>();
    object->set_collision_size(size);
    object->set_position(position);
}

void DemoLevel::initialize()
{
    subscribe(core::EventType::Keyboard);
    set_finished(false);
    Size screen_size = screen_manager().screen_size();
    LOG_D("Started DemoLevel on %dx%d screen.", screen_size.x, screen_size.y)

    world_manager().set_world_size(screen_size);
    _world_size = world_manager().world_size();
    _spawn_region = {_wall_tile_size * 2, _world_size - _wall_tile_size * 3};
    _camera = world_manager().create_camera({0, 0}, screen_size);
    _screen_id = screen_manager().create_screen({{0,             0},
                                                 {screen_size.x, screen_size.y}}, 0);

    auto mini_map = screen_manager().create_screen({{0, 0}, _world_size / 10}, 1);

    screen_manager().attach_camera(_camera, _screen_id);
    screen_manager().attach_camera(_camera, mini_map);

    for (uint32_t y = 1; y < (_world_size.y / _wall_tile_size.y); ++y)
    {
        create_tile({0, 255, 0, 255}, {0, 0, 0, 255}, _wall_tile_size, {(uint32_t) 0, y * _wall_tile_size.y});
        create_tile({0, 255, 0, 255}, {0, 0, 0, 255}, _wall_tile_size,
                    {_world_size.x - _wall_tile_size.x, y * _wall_tile_size.y});
    }
    for (uint32_t x = 0; x < (_world_size.x / _wall_tile_size.x + 1); ++x)
    {
        create_tile({0, 255, 0, 255}, {0, 0, 0, 255}, _wall_tile_size, {x * _wall_tile_size.x, (uint32_t) 0});
        create_tile({0, 255, 0, 255}, {0, 0, 0, 255}, _wall_tile_size,
                    {x * _wall_tile_size.x, _world_size.y - _wall_tile_size.y});
    }

    Point p1 = {_world_size.x / 2 - _wall_tile_size.x / 2, 4 * _world_size.y / 5};
    Point p2 = {_world_size.x / 2 - 2 * _wall_tile_size.x, 1 * _world_size.y / 3};
    create_tile({255, 255, 0, 255}, {0, 0, 0, 255}, _wall_tile_size, p1);
    create_tile({255, 255, 0, 255}, {0, 0, 0, 255}, _wall_tile_size, p2);

    create_invisible_wall(_wall_tile_size, p1);
    create_invisible_wall(_wall_tile_size, p2);

    create_invisible_wall({_world_size.x, _wall_tile_size.y},
                          {0, 0});
    create_invisible_wall({_world_size.x, _wall_tile_size.y},
                          {(uint32_t) 0, _world_size.y - _wall_tile_size.y});
    create_invisible_wall({_wall_tile_size.x, _world_size.y - 2 * _wall_tile_size.y},
                          {(uint32_t) 0, _wall_tile_size.y});
    create_invisible_wall({_wall_tile_size.x, _world_size.y - 2 * _wall_tile_size.y},
                          {_world_size.x - _wall_tile_size.x, _wall_tile_size.y});

}



void DemoLevel::evaluate(uint32_t time_elapsed)
{
    _spawn_key_pressed = false;
    BasicContext::evaluate(time_elapsed);

    if (_spawn_key_pressed)
    {
        auto object = world_manager().create_object<extensions::complex::object::WallBouncer>();
        auto rect = object->set_drawable<core::drawable::DrawableRect>();
        auto size = helpers::math::RandomIntGenerator::generate_random_size({15, 40}, {15, 40});
//        object->set_direction(0);
//        size = {40, 40};
//        object->set_speed(500);
        object->set_direction(helpers::math::RandomIntGenerator::generate_random_int(0, 360));
        object->set_speed(helpers::math::RandomIntGenerator::generate_random_int(450, 500));
        object->set_collision_size(size);
        rect->size() = size;
        rect->fill_color() = {255, 0, 0, 128};
        rect->border_color() = {0, 0, 0, 255};
        object->set_position(_world_size / 2);

    }


/*    // On next frame it should appear...
    if (_spawn_key_pressed)
    {
       *//* auto point = generate_random_point(_spawn_region);
        auto size = generate_random_size({15, 40}, {15, 40});
        create_tile({255, 0, 0, 0}, {0, 0, 0, 0}, size, point);
        LOG_D("%d %d", point.x, point.y)*//*
       static bool sw = false;
        auto object = world_manager().create_object<extensions::support::context::WallBouncer>();
        auto rect = object->set_drawable<core::drawable::DrawableRect>();
        auto size = generate_random_size({15, 40}, {15, 40});
        object->set_direction(generate_random_int(0, 360));
//        object->set_direction(sw ? 90 : 270);
        static int dir = 0;
        //object->set_direction(90);
    //    dir += 7;
      //  sw = !sw;
        //size = {80, 20};
        //object->set_velocity(800);
        object->set_velocity(generate_random_int(450, 500));
        object->set_collision_size(size);
        rect->size() = size;
        rect->fill_color() = {255, 0, 0, 128};
        rect->border_color() = {0, 0, 0, 255};
        object->set_position({_world_size/2});
    }*/


    /*_total_time += time_elapsed;
    if (_total_time > 1500)
    {
        set_finished(true);
    }*/
}

void DemoLevel::process_event(const core::Event *event)
{
    auto keyboard_event = dynamic_cast<const core::KeyboardEvent *>(event);
    if (keyboard_event != nullptr)
    {
        if (keyboard_event->state() == core::KeyboardEvent::State::PRESSED)
        {
            //LOG_D("key pressed: %d", keyboard_event->sym())
            switch (keyboard_event->sym())
            {
                case SDLK_SPACE:
                    _spawn_key_pressed = true;
                    break;
                case SDLK_q:
                    set_finished(true);
                default:
                    break;
            }
        }
    }
}


