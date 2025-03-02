#include "UI.h"


class UI
{
public:
    void displayMenu() const
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

    void displayError(const std::string& errMsg) const
    {
        std::cout << "Error: " << errMsg << std::endl;
    }

    void displayMessage(const std::string& msg) const
    {
        std::cout << "Message: " << msg << std::endl;
    }
};
