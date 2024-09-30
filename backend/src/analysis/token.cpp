#include <string>

namespace analysis
{
    class Token
    {
    public:
        std::string chars;
        bool positions;
        bool stopped;
        bool remove_stops;
        std::string mode;

        Token(std::string chars, bool positions = false, bool stopped = false, bool remove_stops = true, std::string mode = "") : chars{chars}, positions{positions}, stopped{stopped}, remove_stops{remove_stops}, mode{mode} {};
    };
}