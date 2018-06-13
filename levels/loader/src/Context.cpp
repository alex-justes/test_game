#include "Context.h"
#include "helpers/Configuration.h"

void Loader::initialize()
{
    set_finished(false);
    subscribe(EventType::KeyPress);
    subscribe(EventType::MouseClick);
    subscribe(EventType::MouseMove);
    helpers::configuration::Configuration conf;
    conf.read("levels.conf");
    _demo_entry_point = conf.get<std::string>("entry_point", "demo_level", "");
    _normal_entry_point = conf.get<std::string>("entry_point", "normal_level", "");
    _entry_points[0] = _demo_entry_point;
    _entry_points[1] = _normal_entry_point;

    load_next_entry();

}

void Loader::load_next_entry()
{
    if (_current_context != nullptr)
    {
        _context_loader.unload_context(_current_context->unique_id());
    }
    _i_current_context = _i_next_context;
    _current_context = _context_loader.load_context(_entry_points[_i_next_context++].c_str(), _event_manager, screen_manager());
    _current_context->initialize();
    if (_i_next_context >= _entry_points.size())
    {
        _i_next_context = 0;
    }
}

void Loader::reload_entry()
{
    if (_current_context != nullptr)
    {
        _context_loader.unload_context(_current_context->unique_id());
    }
    _current_context = _context_loader.load_context(_entry_points[_i_current_context].c_str(), _event_manager, screen_manager());
    _current_context->initialize();
}

void Loader::process_event(std::shared_ptr<const Event> event)
{
    if (!event)
    {
        return;
    }
    bool dispatched = false;
    auto key_press = dynamic_cast<const KeyPressEvent *>(event.get());
    if (key_press && key_press->state() == KeyPressEvent::State::PRESSED)
    {
        switch (key_press->sym())
        {
            case SDLK_ESCAPE:
                set_finished(true);
                dispatched = true;
                break;
            case SDLK_r:
                _reload_current_context = true;
                dispatched = true;
                break;
            case SDLK_h:
                _switch_current_context = true;
                dispatched = true;
                break;
            default:
                break;
        }
    }
    if (!dispatched)
    {
        _event_manager.dispatch(event);
    }
}

void Loader::evaluate(uint32_t time_elapsed)
{
    Item event;
    // Process events
    while (events_pop(event))
    {
        process_event(event);
    }

    if (_switch_current_context)
    {
        load_next_entry();
        _switch_current_context = false;
    }
    if (_reload_current_context)
    {
        reload_entry();
        _reload_current_context = false;
    }

    _current_context->evaluate(time_elapsed);
}
Loader::Loader(EventManager &event_manager, ScreenManager &screen_manager) :
        Context(event_manager, screen_manager)
{

}



