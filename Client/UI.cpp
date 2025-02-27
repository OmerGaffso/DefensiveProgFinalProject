#include "UI.h"

using namespace std;

void UI::displayMenu() const
{
	cout << "MessageU client at your service\n\n";
	cout << "110) Register\n"
		"120) Request for clients list\n"
		"130) Request for public key\n"
		"140) Request for waiting messages\n"
		"150) Send a text message\n"
		"151) Send a request for symmetric key\n"
		"152) Send your symmetric key\n"
		"0) Exit client\n"
		"\n>>";
}

string UI::getUserInput() const
{

	return "";
}