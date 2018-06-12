#include "core/Context.h"
#include "Context.h"
#include "Log.h"
#include "support/Objects.hpp"
#include "support/Utilities.h"

DemoLevel::DemoLevel(core::EventManager &event_manager, core::ScreenManager &screen_manager)
        :
        helpers::context::BasicContext(event_manager, screen_manager)
{
    DemoVillain::clear_amount();
}

void DemoLevel::create_tile(const RGBA &fill_color,
                            const RGBA &border_color,
                            const Size &size,
                            const Point &position)
{
    auto object = world_manager().create_object<extensions::complex::object::Decal>();
    auto rect = object->set_drawable<core::drawable::DrawableRect>();
    rect->set_box_size(size);
    rect->set_fill_color(fill_color);
    rect->set_border_color(border_color);
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
    subscribe(core::EventType::KeyPress);
    subscribe(core::EventType::MouseClick);
    set_finished(false);
    Size screen_size = screen_manager().screen_size();
    LOG_D("Started DemoLevel on %dx%d screen.", screen_size.x, screen_size.y)

    world_manager().set_world_size(screen_size);
    _world_size = world_manager().world_size();
    _camera = world_manager().create_camera({0, 0}, screen_size);
    _screen_id = screen_manager().create_screen({{0,             0},
                                                 {screen_size.x, screen_size.y}}, 0);

    screen_manager().attach_camera(_camera, _screen_id);

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

    create_invisible_wall({_world_size.x, _wall_tile_size.y},
                          {0, 0});
    create_invisible_wall({_world_size.x, _wall_tile_size.y},
                          {(uint32_t) 0, _world_size.y - _wall_tile_size.y});
    create_invisible_wall({_wall_tile_size.x, _world_size.y - 2 * _wall_tile_size.y},
                          {(uint32_t) 0, _wall_tile_size.y});
    create_invisible_wall({_wall_tile_size.x, _world_size.y - 2 * _wall_tile_size.y},
                          {_world_size.x - _wall_tile_size.x, _wall_tile_size.y});

    _spawn_positions[0] = {_world_size.x / 6, _world_size.y / 6};
    _spawn_positions[1] = {_world_size.x / 6, 5 * _world_size.y / 6 - 20};
    _spawn_positions[2] = {5 * _world_size.x / 6 - 20, _world_size.y / 6 - 20};
    _spawn_positions[3] = {5 * _world_size.x / 6 - 20, 5 * _world_size.y / 6};


    create_tile({0, 255, 255, 200}, {}, {20, 20}, _spawn_positions[0]);
    create_tile({0, 255, 255, 200}, {}, {20, 20}, _spawn_positions[1]);
    create_tile({0, 255, 255, 200}, {}, {20, 20}, _spawn_positions[2]);
    create_tile({0, 255, 255, 200}, {}, {20, 20}, _spawn_positions[3]);

    _spawn_positions[0] += Point{10, 10};
    _spawn_positions[1] += Point{10, 10};
    _spawn_positions[2] += Point{10, 10};
    _spawn_positions[3] += Point{10, 10};


    auto spawner = world_manager().create_object<RadialParticleGenerator<DemoVillain>>();
    auto drawable_generator = new helpers::generator::SimpleDrawableRectGenerator;
    drawable_generator->set_fill_color({255, 0, 255, 128});
    drawable_generator->set_border_color({0, 0, 0, 128});
    spawner->set_drawable_generator(std::unique_ptr<helpers::generator::DrawableGenerator>(drawable_generator));
    spawner->set_direction_range(360);
    spawner->set_generator_direction(0);
    spawner->set_speed_range(60, 150);
    spawner->set_particle_count_range(10, 20);
    spawner->set_size_range({15, 30}, {15, 30});
    spawner->set_life_time_range(1000, 12000);

    _villain_spawner = spawner;

    //create_invisible_wall({30, 30}, _spawn_positions[0]);
    //create_invisible_wall({30, 30}, _spawn_positions[1]);

/*    {
        auto explosion = world_manager().create_object<extensions::complex::object::RadialParticleGenerator<extensions::complex::object::Projectile>>();
//        auto explosion = world_manager().create_object<extensions::complex::object::RadialParticleGenerator<AutoDyingWallBouncer >>();
        auto drawable_generator = new helpers::generator::SimpleDrawableRectGenerator;
        drawable_generator->set_fill_color({255, 0, 255, 128});
        drawable_generator->set_border_color({0, 0, 0, 128});
        explosion->set_drawable_generator(std::unique_ptr<helpers::generator::DrawableGenerator>(drawable_generator));


        explosion->set_direction_range(30);
        explosion->set_generator_direction(90);
        explosion->set_speed_range(700, 1800);
        explosion->set_particle_count_range(40, 60);
        explosion->set_size_range({5, 10}, {5, 10});

        explosion->set_life_time_range(800, 1200);
        explosion->set_position(_world_size / 2);
        _particle_generator = explosion;
    }*/

}


void DemoLevel::evaluate(uint32_t time_elapsed)
{
    _spawn_key_pressed = false;
    BasicContext::evaluate(time_elapsed);
    _time_to_spawn -= time_elapsed;
    _time_to_shoot -= time_elapsed;
    if (DemoVillain::amount() < 60 && _time_to_spawn < 0)
    {
        auto where = helpers::math::RandomIntGenerator::generate_random_int(0, 4);
        _villain_spawner->generate(_spawn_positions[where]);
        _time_to_spawn = 800;
    }

    if (_time_to_shoot < 0)
    {
        _time_to_shoot = 500;
        auto rocket = world_manager().create_object<extensions::complex::object::Rocket>(50);
        rocket->set_direction(helpers::math::RandomIntGenerator::generate_random_int(0, 360));
        rocket->set_speed(600);
        rocket->set_position(_world_size / 2);
        auto rect = rocket->set_drawable<core::drawable::DrawableRect>();
        rect->set_box_size({10,10});
        rect->set_fill_color({255, 0, 0, 255});
    }
}

void DemoLevel::process_event(const core::Event *event)
{
    auto keyboard_event = dynamic_cast<const core::KeyPressEvent *>(event);
    if (keyboard_event != nullptr)
    {
        if (keyboard_event->state() == core::KeyPressEvent::State::PRESSED)
        {
            switch (keyboard_event->sym())
            {
                case SDLK_SPACE:
                    _spawn_key_pressed = true;
                    break;
                case SDLK_q:
                    set_finished(true);
                    break;
                default:
                    break;
            }
        }
    }
}

std::atomic_int DemoVillain::_amount = 0;

void DemoVillain::clear_amount()
{
    _amount = 0;
}

int DemoVillain::amount()
{
    return _amount;
}

void DemoVillain::initialize()
{
    BouncableObject::initialize();
    _amount++;
}
void DemoVillain::die()
{
    _amount--;
    if (_killed_by_projectile)
    {
        auto drawable_generator = new helpers::generator::SimpleDrawableRectGenerator;
        drawable_generator->set_fill_color({255, 0, 255, 128});
        drawable_generator->set_border_color({0, 0, 0, 0});
        set_drawable_generator(std::unique_ptr<helpers::generator::DrawableGenerator>(drawable_generator));
        set_direction_range(360);
        set_generator_direction(0);
        set_speed_range(speed() * 5.f * 50, speed() * 10.f * 50);
        set_particle_count_range(40, 60);
        set_size_range({5, 10}, {5, 10});
        set_life_time_range(1500, 2000);
        generate(position() + Point{render_shape().width()/2, render_shape().height()/2});
    }
}
void DemoVillain::evaluate(uint32_t time_elapsed)
{
    if (!collisions().empty())
    {
        for (const auto& obj: collisions())
        {
            auto projectile = dynamic_cast<const Projectile*>(obj);
            if (projectile)
            {
                _killed_by_projectile = true;
                set_dead();
            }
        }
    }
    if (!dead())
    {
        AutoDyingWallBouncer::evaluate(time_elapsed);
    }
}
