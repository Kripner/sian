#ifndef SCENE_HH
#define SCENE_HH

#include "object.hh"
#include "config.hh"

#include <cairo.h>

#include <initializer_list>
#include <memory>
#include <string>
#include <vector>

namespace Sian {

class Scene
{
public:
    explicit Scene(const Config& config);

    void add(std::shared_ptr<Object> object);

    void add(std::initializer_list<std::shared_ptr<Object>> new_objects);

    void add_show_creation(std::shared_ptr<Object> object);

    void step(double time_delta);

    class Snapshot
    {
    public:
        Snapshot(std::shared_ptr<cairo_surface_t> surface);

        void save_png(std::string filename) const;

    private:
        std::shared_ptr<cairo_surface_t> surface;
    };

    Snapshot snapshot() const;

private:
    Config config;
    std::vector<std::shared_ptr<Object>> objects;
    StepID next_step_id;
};

} // namespace Sian

#endif
