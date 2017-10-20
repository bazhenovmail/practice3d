#include "fileutil.h"
#include <fstream>

namespace BallUtils
{

bool readFile(const std::wstring& filename, std::vector<char>& data)
{
    std::ifstream file(filename, std::ios::in | std::ios::binary | std::ios::ate);

    if(!file.is_open())
        return false;

    std::streampos size;

    size = file.tellg();

    data.resize(size);
    file.seekg(0, std::ios::beg);
    file.read(data.data(), size);
    if(file.fail())
    {
        file.close();
        return false;
    }
    file.close();

    return true;
}

}//BallUtils