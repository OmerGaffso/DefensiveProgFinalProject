#include "UI.h"


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
    std::cout << "Error: " << errMsg << std::endl << std::endl;
}
//
void UI::displayMessage(const std::string& msg) const
{
    std::cout << msg << std::endl << std::endl;
}
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
    if (invalidUsername(targetUsername))
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
    if (invalidUsername(username))
    {
        throw std::runtime_error("Invalid username format.\n");
        return "";
    }
    return username;
}
//
std::string UI::getMesssage()
{
    std::cout << "Enter message: ";
    std::string message;
    //
    // Clear newline
    if (std::cin.rdbuf()->in_avail() > 0)
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    //
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