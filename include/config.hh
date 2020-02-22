#ifndef CONFIG_HH
#define CONFIG_HH

#include <string>

namespace Sian {

class Config
{
public:
    bool requested_help;
    int main_scene_width;
    int main_scene_height;
    double fps;
    std::string temporary_directory;
    std::string output_file;
    bool require_empty_tmp_dir;

    Config();

    static Config from_args(int argc, char* argv[]);
};

} // namespace Sian

#endif
