#ifndef DEMO_LEVEL_CONTEXT_H
#define DEMO_LEVEL_CONTEXT_H

#include <random>
#include "helpers/BasicContext.h"
#include "support/Context.h"

class DemoLevel : public helpers::context::BasicContext
{
public:
    DemoLevel(core::EventManager &event_manager, core::ScreenManager &screen_manager);
    void initialize() override;
    void evaluate(uint32_t time_elapsed) override;
    void process_event(const core::Event *event) override;
private:
    using Projectile = extensions::support::context::Projectile;
    using Movable = extensions::support::context::SelfMovableObject;
    using Wall = extensions::support::context::Wall;
    void create_tile(const RGBA &fill_color,
                     const RGBA &border_color,
                     const Size &size,
                     const Point &position);

    void create_invisible_wall(const Size &size,
                               const Point &position);

    int generate_random_int(int lo, int hi);
    Point generate_random_point(const Roi &roi);
    Size generate_random_size(const Size &dim_x, const Size &dim_y);

    const Size _wall_tile_size{40, 40};
    Size _world_size;
    Roi _spawn_region;
    uint32_t _total_time{0};
    Id _screen_id;
    core::Camera *_camera{nullptr};
    bool _spawn_key_pressed{false};
    std::random_device _random_device;
    std::mt19937 _random_generator;
};


#endif //DEMO_LEVEL_CONTEXT_H
