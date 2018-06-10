#include "core/Context.h"
#include "support/Context.h"
#include "Context.h"
#include "Log.h"

DemoLevel::DemoLevel(core::EventManager &event_manager, core::ScreenManager &screen_manager)
        :
        helpers::context::BasicContext(event_manager, screen_manager)
{

}

void DemoLevel::initialize()
{
    set_finished(false);
    Size screen_size = screen_manager().screen_size();
    LOG_D("Started DemoLevel on %dx%d screen.", screen_size.x, screen_size.y)

    world_manager().set_world_size(screen_size);
    _world_size = world_manager().world_size();
    _camera = world_manager().create_camera({0, 0}, screen_size);
    _screen_id = screen_manager().create_screen({{0,             0},
                                                 {screen_size.x, screen_size.y}}, 0);

    screen_manager().attach_camera(_camera, _screen_id);

    auto create_tile = [this](const core::drawable::RGBA &fill_color,
                              const core::drawable::RGBA &border_color,
                              const Size &size,
                              const Point &position)
    {
        auto object = object_manager().create<extensions::support::context::Decal>();
        auto rect = object->set_drawable<core::drawable::DrawableRect>();
        rect->size() = size;
        rect->fill_color() = fill_color;
        rect->border_color() = border_color;
        object->set_position(position);
        world_manager().add_object(object);
    };

    for (uint32_t y = 1; y < (_world_size.y / _wall_tile_size.y); ++y)
    {
        create_tile({0, 255, 0, 0}, {0, 0, 0, 0}, _wall_tile_size, {(uint32_t) 0, y * _wall_tile_size.y});
        create_tile({0, 255, 0, 0}, {0, 0, 0, 0}, _wall_tile_size, {_world_size.x - _wall_tile_size.x, y * _wall_tile_size.y});
    }
    for (uint32_t x = 0; x < (_world_size.x / _wall_tile_size.x + 1); ++x)
    {
        create_tile({0, 255, 0, 0}, {0, 0, 0, 0}, _wall_tile_size, {x * _wall_tile_size.x, (uint32_t) 0});
        create_tile({0, 255, 0, 0}, {0, 0, 0, 0}, _wall_tile_size, {x * _wall_tile_size.x, _world_size.y - _wall_tile_size.y});
    }
}

void DemoLevel::evaluate(uint32_t time_elapsed)
{
    BasicContext::evaluate(time_elapsed);

    /*_total_time += time_elapsed;
    if (_total_time > 1500)
    {
        set_finished(true);
    }*/
}

void DemoLevel::process_event(const core::Event *event)
{
    LOG_D("Hoho! Event!")
}