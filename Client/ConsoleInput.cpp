#include "InputHandler.h"

class ConsoleInputHandler :public InputHandler
{
public:
    std::string getInput() override
    {
        std::string in;
        std::getline(std::cin, in);
        // Add checks for the input

        return in;
    }
};