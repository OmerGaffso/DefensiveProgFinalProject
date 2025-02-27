#include <fstream>
#include "InputHandler.h"
#include <string>

class FileInputHandler :public InputHandler
{
private:
    std::ifstream file;
public:
    FileInputHandler(const std::string& filename)
    {
        file.open(filename);
        if (!file)
            throw std::runtime_error("Failed to open file " + filename);
    }

    std::string getInput() override
    {
        std::string line;
        if (std::getline(file, line))
            return line;
        //
        return ""; //EOF
    }

    ~FileInputHandler()
    {
        if (file.is_open())
            file.close();
    }
};
