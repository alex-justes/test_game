#ifndef TEST_GAME_CONTEXT_H
#define TEST_GAME_CONTEXT_H

#include "helpers/BasicContext.h"

namespace extensions::support
{
    class Context: public helpers::context::BasicContext
    {
    public:
        Context(core::EventManager &event_manager, core::ScreenManager &screen_manager);
        void initialize() override;
        void evaluate(uint32_t time_elapsed) override;
        void process_event(const core::Event *event) override;
    };
}

#endif //TEST_GAME_CONTEXT_H
