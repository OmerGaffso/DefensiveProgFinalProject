#include "Application.h"
#include "UI.h"
#include "InputHandler.h"

Application::Application()
{
    InputHandler con = new ConsoleInputHandler(); // 
}

Application::~Application()
{

}

void Application::run()
{
    UI ui;
    ui.displayMenu();

}