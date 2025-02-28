#include "ConsoleInput.h"

std::string ConsoleInputHandler::getInput()
{
    std::string in;
    std::getline(std::cin, in);
    // Add checks for the input

    return in;
}