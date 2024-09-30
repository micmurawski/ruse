#pragma once
#include <string>

namespace analysis
{

    class Token
    {
    public:
        std::string chars;
        std::string mode;
        bool positions;
        bool stopped;
        bool remove_stops;
        Token(std::string chars, bool positions = false, bool stopped = false, bool remove_stops = true, std::string mode = "");
    };

}