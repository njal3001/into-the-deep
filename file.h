#include <string>

namespace Uboat
{
    struct File
    {
        std::string path;
        size_t size;
        char *data;

        File(const std::string& path);
        ~File();
    };
};
