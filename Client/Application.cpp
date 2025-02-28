#include "Application.h"

Application::Application()
{
    inputHandler = new ConsoleInputHandler(); // 
}

Application::~Application()
{
    delete inputHandler;
}

void Application::run()
{
    ui.displayMenu();
    std::string userInput = inputHandler->getInput();

    std::cout << "User Input was: " << userInput << "\n";
}