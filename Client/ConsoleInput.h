#pragma once
#include "InputHandler.h"

class ConsoleInputHandler :public InputHandler
{
public:
    std::string getInput() override;

};