
#include "Context.h"
#include "Log.h"
#include "helpers/Configuration.h"

#include <fstream>
#include <iostream>

NormalLevel::NormalLevel(core::EventManager &event_manager, core::ScreenManager &screen_manager) :
        BasicContext(event_manager, screen_manager)
{
    _hero_actions.shoot_timeout = -1;
}

NormalLevel::~NormalLevel()
{
    screen_manager().detach_camera(_camera, _main_screen);
    screen_manager().detach_camera(_minimap_camera, _minimap_screen);
    screen_manager().remove_screen(_main_screen);
    screen_manager().remove_screen(_minimap_screen);
}

void NormalLevel::create_tile(const RGBA &fill_color,
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

void NormalLevel::create_invisible_wall(const Size &size,
                                        const Point &position)
{
    auto object = world_manager().create_object<extensions::complex::object::Wall>();
    object->set_collision_size(size);
    object->set_position(position);
}

void NormalLevel::initialize()
{
    subscribe(core::EventType::KeyPress);
    subscribe(core::EventType::MouseMove);
    subscribe(core::EventType::MouseClick);

    set_finished(false);

    helpers::configuration::Configuration conf;
    conf.read("normal_level.conf");

    auto level_path = conf.get<std::string>("level", "default", "");
    _speed = conf.get<int>("Speed", "default", 0);
    _targets = conf.get<int>("CountTarget", "default", 0);
    _timer = 1000 * conf.get<int>("Time", "default", 0);

    std::fstream fin(level_path);
    size_t height = 0;
    size_t width = 0;
    std::list<std::string> map;
    for (std::string line; std::getline(fin, line);)
    {
        auto &vec = map.back();
        if (width == 0)
        {
            width = line.size();
        }
        else
        {
            if (line.size() != width)
            {
                LOG_E("Invalid level map (size): line: %d, size: %d", height, line.size())
                set_finished(true);
            }
        }
        height++;
        map.emplace_back(line);
    }
    LOG_S("Level: %dx%d", width, height)
    if (finished())
    {
        return;
    }
    --width;

    _world_size = {_wall_tile_size.x * width, _wall_tile_size.y * height};
    world_manager().set_world_size(_world_size);

    uint32_t x = 0;
    uint32_t y = 0;
    Point starting_pos{0, 0};
    for (const auto &line: map)
    {
        x = 0;
        for (const auto &s: line)
        {
            switch (s)
            {
                case '#':
                {
                    Point position{x * _wall_tile_size.x, y * _wall_tile_size.y};
                    create_tile({0, 255, 0, 255}, {0, 0, 0, 255}, _wall_tile_size, position);
                    create_invisible_wall(_wall_tile_size, position);
                }
                    break;
                case 'v':
                {
                    Point position{x * _wall_tile_size.x, y * _wall_tile_size.y};
                    create_tile({0, 255, 255, 128}, {0, 0, 0, 0}, _wall_tile_size, position);
                    _spawn_positions.emplace_back(Point{x * _wall_tile_size.x, y * _wall_tile_size.y});
                }
                    break;
                case 'o':
                    starting_pos = {x * _wall_tile_size.x, y * _wall_tile_size.y};
                    break;
                default:
                    break;
            }
            ++x;
        }
        ++y;
    }

    _hero = world_manager().create_object<Hero>();
    auto rect = _hero->set_drawable<core::drawable::DrawableRect>();
    rect->set_fill_color({255, 255, 255, 200});
    rect->set_border_color({0,0,0,255});
    rect->set_box_size(_hero_size);
    _hero->set_collision_size(_hero_size);
    _hero->set_position(starting_pos);
    _hero->set_speed(400);

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

    PointI32 camera_starting_pos = starting_pos;
    camera_starting_pos -= _camera_size / 2;

    PointI32 camera_offset{0, 0};

    if (camera_starting_pos.x < 0)
    {
        camera_offset.x = -camera_starting_pos.x;
    }
    if (camera_starting_pos.y < 0)
    {
        camera_offset.y = -camera_starting_pos.y;
    }
    if (camera_starting_pos.x + _camera_size.x > _world_size.x)
    {
        camera_offset.x = -(camera_starting_pos.x + _camera_size.x - _world_size.x + 1);
    }
    if (camera_starting_pos.y + _camera_size.y > _world_size.y)
    {
        camera_offset.y = -(camera_starting_pos.y + _camera_size.y - _world_size.y + 1);
    }

    camera_starting_pos += camera_offset;

    auto screen_size = screen_manager().screen_size();
    _main_screen = screen_manager().create_screen({{0, 0},
                                                   {screen_size}}, 5);
    _minimap_screen = screen_manager().create_screen({{0, 0},
                                                      _minimap_size}, 6, false);
    _camera = world_manager().create_camera({0, 0}, _world_size);
    _minimap_camera = world_manager().create_camera({0, 0}, _world_size);

    screen_manager().attach_camera(_camera, _main_screen);
    screen_manager().attach_camera(_minimap_camera, _minimap_screen);
}


void NormalLevel::process_event(const core::Event *event)
{
    auto key_press_event = dynamic_cast<const core::KeyPressEvent *>(event);
    if (key_press_event)
    {
        if (key_press_event->sym() == SDLK_q)
        {
            set_finished(true);
            return;
        }
        bool state = key_press_event->state() == core::KeyPressEvent::State::PRESSED;
        switch (key_press_event->sym())
        {
            case SDLK_w:
                _hero_actions.forward = state;
                _hero_actions.forward_timeout = _key_timeout;
                break;
            case SDLK_s:
                _hero_actions.backward = state;
                _hero_actions.backward_timeout = _key_timeout;
                break;
            case SDLK_a:
                _hero_actions.strafe_l = state;
                _hero_actions.strafe_l_timeout = _key_timeout;
                break;
            case SDLK_d:
                _hero_actions.strafe_r = state;
                _hero_actions.strafe_r_timeout = _key_timeout;
                break;
            default:
                break;
        }
    }
    auto mouse_event = dynamic_cast<const core::MouseEvent *>(event);
    if (mouse_event && mouse_event->context_id() == unique_id()
        && mouse_event->camera_id() == _camera->unique_id())
    {
        _mouse_position = mouse_event->position() + _camera->position();
        auto mouse_click = dynamic_cast<const core::MouseClickEvent *>(mouse_event);
        if (mouse_click)
        {
            if (mouse_click->button() == core::MouseClickEvent::Button::LEFT &&
                mouse_click->state() == core::MouseClickEvent::State::RELEASED)
            {
                _hero_actions.shoot = true;
            }
        }
    }
}

void NormalLevel::act()
{


}

void NormalLevel::evaluate(uint32_t time_elapsed)
{
    //_hero_actions = {false, false, false, false, false};
    _hero_actions.forward_timeout -= time_elapsed;
    _hero_actions.backward_timeout -= time_elapsed;
    _hero_actions.strafe_l_timeout -= time_elapsed;
    _hero_actions.strafe_r_timeout -= time_elapsed;
    _hero_actions.shoot_timeout -= time_elapsed;
    _hero_actions.shoot = false;
    _timer -= time_elapsed;
    BasicContext::evaluate(time_elapsed);

    if (_timer > 0 && DemoVillain::amount() < _targets)
    {
        int pos = helpers::math::RandomIntGenerator::generate_random_int(0, _spawn_positions.size());
        _villain_spawner->generate(_spawn_positions[pos]);
    }

    if (_timer < 0 && _show_result)
    {
        _show_result = false;
        DemoVillain::set_detonate();
        LOG_W("##########################")
        LOG_W("## Your score: %d", DemoVillain::killed())
        LOG_W("##########################")
    }


    //    _hero->set_direction(_mouse_position - _hero->position());
    PointF hero_direction = _mouse_position;
    hero_direction -= _hero->position();
    PointF final_direction {0, 0};
    if (_hero_actions.forward && _hero_actions.forward_timeout > 0)
    {
        final_direction += PointF{0.f, -1.f};
        //final_direction += hero_direction;
    }
    if (_hero_actions.backward && _hero_actions.backward_timeout > 0)
    {
        final_direction += PointF{0.f, 1.f};
        //final_direction -= hero_direction;
    }
    if (_hero_actions.strafe_l && _hero_actions.strafe_l_timeout > 0)
    {
        final_direction += PointF{-1.f, 0.f};
        //final_direction += PointF{hero_direction.y, -hero_direction.x};
    }
    if (_hero_actions.strafe_r && _hero_actions.strafe_r_timeout > 0)
    {
        final_direction += PointF{1.f, 0.f};
        //final_direction += PointF{-hero_direction.y, hero_direction.x};
    }
    _hero->set_direction(final_direction);

    if (_hero_actions.shoot && _hero_actions.shoot_timeout < 0)
    {
        auto rocket = world_manager().create_object<extensions::complex::object::Rocket>(50);
        rocket->set_direction(_mouse_position - _hero->position());
        rocket->set_speed(_speed);
        rocket->set_position(_hero->position());
        auto rect = rocket->set_drawable<core::drawable::DrawableRect>();
        rect->set_box_size({10,10});
        rect->set_fill_color({255, 0, 0, 255});
    }
}

std::atomic_int DemoVillain::_amount = 0;
std::atomic_int DemoVillain::_killed_by_hero = 0;
bool DemoVillain::_detonate = false;

void DemoVillain::clear_amount()
{
    _amount = 0;
}

void DemoVillain::set_detonate()
{
    _detonate = true;
}

int DemoVillain::amount()
{
    return _amount;
}

int DemoVillain::killed()
{
    return _killed_by_hero;
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
        ++_killed_by_hero;
    }
    if (_detonate || _killed_by_projectile)
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