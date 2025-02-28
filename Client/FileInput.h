#pragma once
#include <fstream>
#include "InputHandler.h"
#include <string>

class FileInputHandler :public InputHandler
{
private:
    std::ifstream file;
public:
    FileInputHandler(const std::string& filename);
    std::string getInput() override;
    ~FileInputHandler();
};

