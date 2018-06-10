#ifndef DEMO_LEVEL_CONTEXT_H
#define DEMO_LEVEL_CONTEXT_H

#include "helpers/BasicContext.h"

class DemoLevel : public helpers::context::BasicContext
{
public:
    DemoLevel(core::EventManager &event_manager, core::ScreenManager &screen_manager);
    void initialize() override;
    void evaluate(uint32_t time_elapsed) override;
    void process_event(const core::Event *event) override;
private:
    const Size _wall_tile_size {40, 40};
    Size _world_size;
    uint32_t _total_time{0};
    Id _screen_id;
    core::Camera* _camera {nullptr};
};

#endif //DEMO_LEVEL_CONTEXT_H
