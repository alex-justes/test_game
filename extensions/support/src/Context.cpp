#include "support/Context.h"
#include "Log.h"

using namespace extensions::support;

Context::Context(core::EventManager &event_manager, core::ScreenManager &screen_manager)
: helpers::context::BasicContext(event_manager, screen_manager)
{

}
void Context::initialize()
{
    subscribe(core::EventType::Keyboard);
    set_finished(false);
}
void Context::evaluate(uint32_t time_elapsed)
{
    BasicContext::evaluate(time_elapsed);
}
void Context::process_event(const core::Event *event)
{
    LOG_D("Event!")
}
