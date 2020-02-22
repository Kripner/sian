#include "object.hh"
#include "scene.hh"

#include <cairo.h>

#include <initializer_list>
#include <memory>
#include <string>

namespace Sian {

Scene::Scene(const Config& config)
    : config(config),
      next_step_id(0)
{ }

void Scene::add(std::shared_ptr<Object> object)
{
    objects.push_back(object);
}

void Scene::add(std::initializer_list<std::shared_ptr<Object>> new_objects)
{
    objects.insert(objects.end(), new_objects.begin(), new_objects.end());
}

void Scene::add_show_creation(std::shared_ptr<Object> object)
{
    object->show_creation();
    add(object);
}

Scene::Snapshot Scene::snapshot() const
{
    std::shared_ptr<cairo_surface_t> surface(
            cairo_image_surface_create(
                CAIRO_FORMAT_ARGB32,
                config.main_scene_width,
                config.main_scene_height),
            cairo_surface_destroy);
    cairo_t* cr = cairo_create(surface.get());
    cairo_set_antialias(cr, CAIRO_ANTIALIAS_BEST);
    cairo_set_line_width(cr, 2.0);

    // background
    cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);
    cairo_rectangle(cr, 0.0, 0.0, config.main_scene_width, config.main_scene_height);
    cairo_fill(cr);

    // default color
    cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);

    for (const auto& object : objects)
    {
        object->draw(cr);
    }

    cairo_destroy(cr);
    return Snapshot(surface);
}

void Scene::step(double time_delta)
{
    for (const auto& object_ptr : objects)
    {
        object_ptr->step(time_delta, next_step_id);
    }
    ++next_step_id;
}

void Scene::Snapshot::save_png(std::string filename) const
{
    cairo_surface_write_to_png(surface.get(), filename.c_str());
}

Scene::Snapshot::Snapshot(std::shared_ptr<cairo_surface_t> surface)
    : surface(surface)
{ }

} // namespace Sian
