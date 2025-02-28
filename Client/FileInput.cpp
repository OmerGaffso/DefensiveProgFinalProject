#include "FileInput.h"

FileInputHandler::FileInputHandler(const std::string& filename)
{
    file.open(filename);
    if (!file)
        throw std::runtime_error("Failed to open file " + filename);
}

std::string FileInputHandler::getInput()
{
    std::string line;
    if (std::getline(file, line))
        return line;
    //
    return ""; //EOF
}

FileInputHandler::~FileInputHandler()
{
    if (file.is_open())
        file.close();
}
