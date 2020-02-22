#ifndef ANIMATOR_HH
#define ANIMATOR_HH

#include "config.hh"
#include "scene.hh"

#include <functional>
#include <list>

namespace Sian {

class Animator
{
public:
    Animator(const Config& config, Scene& scene);

    void step();

    void wait(double duration);

    using TickObserver = std::function<void(double)>;

    void register_tick_observer(const TickObserver& observer);

    void finish();
private:
    Config config;
    Scene& scene;
    double time;
    int output_counter = 0;
    std::list<TickObserver> tick_observers;
};

} // namespace Sian


#endif
