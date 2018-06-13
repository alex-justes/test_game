#ifndef NORMAL_LEVEL_CONTEXT_H
#define NORMAL_LEVEL_CONTEXT_H

#include <atomic>
#include "helpers/BasicContext.hpp"
#include "support/Objects.hpp"


using namespace extensions::basic::object;
using namespace extensions::complex::object;

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
    static void set_detonate();
    static int killed();
private:
    static std::atomic_int _amount;
    static std::atomic_int _killed_by_hero;
    static bool _detonate;
    bool _killed_by_projectile {false};
};

class NormalLevel : public helpers::context::BasicContext
{
public:
    NormalLevel(core::EventManager &event_manager, core::ScreenManager &screen_manager);
    virtual ~NormalLevel();
    void act() override;
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
    Point _world_size;
    std::vector<Point> _spawn_positions;
    const Size _wall_tile_size{30, 30};
    const Size _camera_size{1024, 768};
    const Size _minimap_size{120, 120};
    core::Camera* _camera;
    Id _main_screen;
    core::Camera* _minimap_camera;
    Id _minimap_screen;
    const int32_t _key_timeout {500};
    const int32_t _shoot_timeout {1200};

    int32_t _timer;
    int32_t _targets;
    int32_t _speed;

    const Size _hero_size {20, 20};
    Hero* _hero;
    PointI32 _mouse_position;
    ParticleGenerator* _villain_spawner;

    bool _show_result {true};
    struct HeroActions
    {
        bool forward {false};
        int32_t forward_timeout;
        bool backward {false};
        int32_t backward_timeout;
        bool strafe_l {false};
        int32_t strafe_l_timeout;
        bool strafe_r {false};
        int32_t strafe_r_timeout;
        bool shoot {false};
        int32_t shoot_timeout;
    } _hero_actions;
};


#endif //NORMAL_LEVEL_CONTEXT_H
