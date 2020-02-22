#include "logger.hh"

#include <iostream>
#include <string>

namespace Sian {

void Logger::info(std::string msg)
{
    std::cout << msg << std::endl;
}

void Logger::error(std::string msg)
{
    std::cerr << msg << std::endl;
}

} // namespace Sian
