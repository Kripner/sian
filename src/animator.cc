#include "animator.hh"
#include "logger.hh"
#include "utils.hh"

#include <cstdlib> // std::system
#include <sstream>
#include <stdexcept> // std::invalid_argument
#include <string>
#include <sys/types.h>
#include <sys/stat.h>

namespace Sian {

void setup(const Config& config)
{
    const std::string dir = config.temporary_directory;

    struct stat info;
    if (stat(dir.c_str(), &info) != 0 || !(info.st_mode & S_IFDIR))
    {
        throw std::invalid_argument(Utils::str_format(
                "Cannot access directory %s/. Please make sure it exists.",
                dir.c_str()));
    }
}

Animator::Animator(const Config& config, Scene& scene)
    : config(config), scene(scene), time(0)
{
    setup(config);
}

void Animator::step()
{
    const std::string filename = config.temporary_directory + "/" +
                                 std::to_string(output_counter++) + ".png";

    struct stat info;
    if (stat(filename.c_str(), &info) == 0 && config.require_empty_tmp_dir)
    {
        throw std::invalid_argument(Utils::str_format(
                "The temporary directory %s/ is not empty. Please remove its content or "
                "run the program with -r option to state that you wish to do it automatically.",
                config.temporary_directory.c_str()));
    }

    scene.snapshot().save_png(filename);

    const double delta = 1 / config.fps;
    scene.step(delta);
    time += delta;

    for (const TickObserver& observer : tick_observers)
    {
        observer(delta);
    }
}

void Animator::wait(double duration)
{
    const double start_time = time;
    while (time - start_time < duration)
    {
        step();
    }
}

void Animator::register_tick_observer(const TickObserver& observer)
{
    tick_observers.push_back(observer);
}

std::string output_filename(const Config& conf)
{
    const std::string fn = conf.output_file;
    const std::string ext = std::string(".mp4");
    // for any filename that doesn't end with '.mp4'
    // or is equal to '.mp4', we append '.mp4'
    if (fn.size() < ext.size() + 1 ||
        fn.compare(fn.size() - ext.size(), ext.size(), ext) != 0)
        return fn + ext;
    return fn;
}


void Animator::finish()
{
    const std::string output = output_filename(config);
    if (system(NULL) != 0)
    {
        std::ostringstream ss;
        ss << "ffmpeg -r "
           << std::to_string(config.fps)
           << " -f image2 -i "
           << config.temporary_directory
           << "/%d.png -vcodec libx264 -crf 25 -pix_fmt yuv420p "
           << output;
        Logger::info(ss.str());
        std::system(ss.str().c_str());
    }
}

} // namespace Sian
