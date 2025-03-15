/*
    This class will handle the user interface - the menu and display messages
    and errors.
*/
#pragma once
#include <iostream>
#include <string>
#include "Utility.h"

class UI
{
public:
    void displayMenu() const;
    void displayError(const std::string& errMsg) const;
    void displayMessage(const std::string& msg) const;
    int  getUserInput();
    std::string getTargetUsername();
    std::string getUsername();
    std::string getMessasge();
    std::string getFilePath();
};