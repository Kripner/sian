#include "config.hh"
#include "logger.hh"
#include "utils.hh"

#include <cstdlib> // std::exit
#include <functional>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace Sian {

// setup the default parameters
Config::Config()
    : requested_help(false),
      main_scene_width(1280), main_scene_height(720),
      fps(30),
      temporary_directory("pngs"),
      output_file("anim"),
      require_empty_tmp_dir(true)
{ }

struct Item
{
    std::vector<std::string> names;
    std::string description;
    std::function<void(Config&, const std::string&)> setter;
    bool expects_value = true;
};

const std::vector<Item> configurable_items =
{
    {
        {"h", "help"},
        "Show this manual.",
        [](Config& c, const std::string& val) { c.requested_help = true; },
        false
    },
    {
        {"w", "width"},
        "Set width of the animation in pixels.",
        [](Config& c, const std::string& val) { c.main_scene_width = std::stoi(val); }
    },
    {
        {"h", "height"},
        "Set height of the animation in pixels.",
        [](Config& c, const std::string& val) { c.main_scene_height = std::stoi(val); }
    },
    {
        {"f", "fps"},
        "Set frames per second.",
        [](Config& c, const std::string& val) { c.fps = std::stod(val); }
    },
    {
        {"d", "tmpdir"},
        "Specify the directory where temporary files can be place by the program. It should exist "
        "and be empty.",
        [](Config& c, const std::string& val)
        {
            c.output_file = val.size() > 1 && val[val.size() - 1] == '/' ?
                                val.substr(0, val.size() - 1) :
                                val;
        }
    },
    {
        {"o", "out"},
        "Specify name of the output animation file.",
        [](Config& c, const std::string& val) { c.output_file = val; }
    },
    {
        {"r", "override"},
        "If present, files in the directory used for placing temporary files will be overriden.",
        [](Config& c, const std::string& val) { c.require_empty_tmp_dir = false; },
        false
    }
};

class ArgsParser
{
public:
    ArgsParser(Config& config)
        : config(config),
          state(State::EXPECTING_NAME)
    { }

    void next_arg(const std::string& arg)
    {
        if (arg.empty())
            throw std::invalid_argument("empty argument");
        if (state == State::EXPECTING_NAME)
        {
            current_item = find_item(arg);
            if (current_item->expects_value)
            {
                state = State::EXPECTING_VALUE;
            }
            else
            {
                // pass empty string to indicate nonexistent value
                current_item->setter(config, "");
            }
        }
        else if (state == State::EXPECTING_VALUE)
        {
            try
            {
                current_item->setter(config, arg);
            }
            catch (const std::invalid_argument& err)
            {
                throw std::invalid_argument(Utils::str_format(
                        "Value for argument %s was given in incorrect format.",
                        current_item->names[0].c_str()));
            }

            state = State::EXPECTING_NAME;
        }
    }

    void end()
    {
        if (state == State::EXPECTING_VALUE)
        {
            throw std::invalid_argument(
                    "Value for the last argument was not present.");
        }
    }

private:
    const Item* find_item(const std::string& arg)
    {
        if (arg[0] != '-')
            throw std::invalid_argument("expected dash followed by argument name");
        std::string name = arg.size() == 1 || arg[1] != '-' ?
                            arg.substr(1, std::string::npos) :
                            arg.substr(2, std::string::npos);
        if (name.size() == 0)
            throw std::invalid_argument("unexpected dash");

        for (const Item& item : configurable_items)
        {
            for (const std::string& item_name : item.names)
            {
                if (name == item_name)
                    return &item;
            }
        }
        throw std::invalid_argument(
                Utils::str_format("unexpected name: \"%s\"", name.c_str()));
    }

    enum class State
    {
        EXPECTING_VALUE,
        EXPECTING_NAME
    };

    Config& config;
    State state;
    const Item* current_item;
};

void print_help(char* program_name)
{
    std::cout
        << "Sian - Simple Animation Library\n"
        << "Usage: " << program_name << " [OPTION]...\n\n";
    for (const auto& item : configurable_items)
    {
        std::cout << "  ";
        char comma[] = {'\0', ' ', '\0'};
        for (const auto& name : item.names)
        {
            std::cout << comma << "-" << name;
            comma[0] = ',';
        }
        std::cout << "\t\t" << item.description << std::endl;
    }
}

Config Config::from_args(int argc, char* argv[])
{
    Config conf;
    ArgsParser parser(conf);

    try
    {
        for (int i = 1; i < argc; ++i)
        {
                parser.next_arg(argv[i]);
        }
        parser.end();
    }
    catch (const std::invalid_argument& err)
    {
        std::cerr << "Invalid argument formatting: " << err.what() << std::endl;
        std::cerr << "For information about usage, run the program with -help option"
                  << std::endl;
        std::exit(1);
    }

    if (conf.requested_help)
    {
        print_help(argv[0]);
        std::exit(0);
    }
    return conf;
}

} // namespace Sian
