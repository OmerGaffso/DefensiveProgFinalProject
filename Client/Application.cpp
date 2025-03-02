#include "Application.h"
#include "Utility.h"

Application::Application() : m_appRunning(true)
{

}

Application::~Application()
{

}

void Application::run()
{
    m_ui.displayMenu();

}

void Application::registerUser()
{
    if (m_config.getUserInfo())
    {
        m_ui.displayError("Registration not allowed. User already exists.\n");
        return;
    }
    //
    std::string username;
    std::cout << "Enter username: ";
    std::getline(std::cin >> std::ws, username);
    //
    if (isValidUsername(username))
    {
        m_ui.displayError("Invalid username.\n");
        return;
    }
    //
    // Send request to server TODO - add implementation
    m_ui.displayMessage("Registering user: " + username);
}
//
void Application::requestClientList()
{
    m_ui.displayMessage("Requesting client list...\n");
    // TODO- add logic here
}
//
void Application::requestPublicKey()
{
    std::string targetUsername;
    std::cout << "Enter target username: ";
    std::getline(std::cin >> std::ws, targetUsername);
    //
    if (isValidUsername(targetUsername))
    {
        m_ui.displayError("Invalid target username.\n");
        return;
    }
    //
    // Send request to server
    m_ui.displayMessage("Requesting public key for: " + targetUsername);
    // TODO - add logic
}
//
void Application::requestPendingMessages()
{

}
//
void Application::requestSymmetricKey()
{

}
//
void Application::sendTextMessage()
{

}
//
void Application::sendSymmetricKey()
{

}
//
void Application::sendFile()
{

}
//
void Application::exitProgram()
{

}
//
void Application::processUserInput(int choice)
{

}
