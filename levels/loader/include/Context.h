#ifndef LOADER_CONTEXT_H
#define LOADER_CONTEXT_H

#include "core/Context.h"

using namespace core;

class Loader : public Context
{
public:
    Loader(EventManager &event_manager, ScreenManager &screen_manager);
    void initialize() override;
    void evaluate(uint32_t time_elapsed) override;
protected:
    void load_next_entry();
    void reload_entry();
    void process_event(std::shared_ptr<const Event> event);
    EventManager _event_manager;
    ContextLoader _context_loader;
    std::string _demo_entry_point;
    std::string _normal_entry_point;
    std::array<std::string, 2> _entry_points;
    int _i_next_context {0};
    int _i_current_context{0};

    Context* _current_context {nullptr};

    bool _reload_current_context{false};
    bool _switch_current_context{false};
};

#endif //LOADER_CONTEXT_H
