#ifndef LOGGER_HH
#define LOGGER_HH

#include <string>

namespace Sian {

class Logger
{
public:
    static void info(std::string msg);

    static void error(std::string msg);
};

} // namespace Sian

#endif
