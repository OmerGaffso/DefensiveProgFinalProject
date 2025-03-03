#include "UI.h"
#include "Utility.h"


void UI::displayMenu() const
{
    std::cout << "MessageU client at your service\n\n";
    std::cout << "110) Register\n"
        "120) Request for clients list\n"
        "130) Request for public key\n"
        "140) Request for waiting messages\n"
        "150) Send a text message\n"
        "151) Send a request for symmetric key\n"
        "152) Send your symmetric key\n"
        "153) Send file\n"
        "0) Exit client\n"
        "\n>>";
}
//
void UI::displayError(const std::string& errMsg) const
{
    std::cout << "Error: " << errMsg << std::endl;
}
//
void UI::displayMessage(const std::string& msg) const
{
    std::cout << msg << std::endl;
}
//
// TODO - VALIDATE INPUT
//
int UI::getUserInput()
{
    int choice;
    std::cin >> choice;
    return choice;
}
//
std::string UI::getTargetUsername()
{
    std::string targetUsername;
    std::cout << "Enter target username: ";
    std::getline(std::cin >> std::ws, targetUsername);
    //
    if (isValidUsername(targetUsername))
    {
        throw std::runtime_error("Invalid username format.\n");
        return "";
    }
    //
    return targetUsername;
}
//
std::string UI::getUsername()
{
    std::cout << "Enter username: ";
    std::string username;
    std::cin >> username;
    //
    if (!isValidUsername(username))
    {
        throw std::runtime_error("Invalid username format.\n");
        return "";
    }
    return username;
}
//
std::string UI::getMessasge()
{
    std::cout << "Enter message: ";
    std::string message;
    std::cin.ignore();
    std::getline(std::cin, message);
    return message;
}
//
std::string UI::getFilePath()
{
    std::cout << "Enter file path: ";
    std::string path;
    std::cin >> path;
    return path;
}