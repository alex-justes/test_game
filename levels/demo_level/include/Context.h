#ifndef DEMO_LEVEL_CONTEXT_H
#define DEMO_LEVEL_CONTEXT_H

#include "helpers/BasicContext.hpp"
#include "support/Objects.hpp"
#include <atomic>

using namespace extensions::complex::object;
using namespace extensions::basic::object;

class DemoVillain :
        public virtual Villain,
        public virtual AutoDyingWallBouncer,
        public virtual RadialParticleGenerator<Exhaust>
{
public:
    void initialize() override;
    void die() override;
    void evaluate(uint32_t time_elapsed) override;

    static int amount();
    static void clear_amount();
private:
    static std::atomic_int _amount;
    bool _killed_by_projectile {false};
};

class DemoLevel : public helpers::context::BasicContext
{
public:
    DemoLevel(core::EventManager &event_manager, core::ScreenManager &screen_manager);
    void initialize() override;
    void evaluate(uint32_t time_elapsed) override;
    void process_event(const core::Event *event) override;
private:
    void create_tile(const RGBA &fill_color,
                     const RGBA &border_color,
                     const Size &size,
                     const Point &position);

    void create_invisible_wall(const Size &size,
                               const Point &position);


    const Size _wall_tile_size{40, 40};
    Size _world_size;
    Id _screen_id;
    core::Camera *_camera{nullptr};

    ParticleGenerator* _villain_spawner;
    std::array<Point, 4> _spawn_positions;
    int32_t _time_to_spawn;
    int32_t _time_to_shoot;
    bool _spawn_key_pressed {false};
};


#endif //DEMO_LEVEL_CONTEXT_H
