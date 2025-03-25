/*
    UI class
    Handles user interaction: menu display, prompts, and input/output.
*/
#pragma once
#include <iostream>
#include <string>
#include "Utility.h"

class UI
{
public:
    /**
     * Displays the main menu to the user.
     */
    void displayMenu() const;
    /**
     * Displays an error message in a standardized format.
     * @param errMsg Error message to display.
     */
    void displayError(const std::string& errMsg) const;
    /**
     * Displays a general informational message.
     * @param msg Message to display.
     */
    void displayMessage(const std::string& msg) const;
    /**
     * Prompts and reads user menu selection.
     * @return Integer representing the selected option.
     */
    int  getUserInput();
    /**
     * Prompts the user to enter a target username.
     * @return Validated target username.
     * @throws std::runtime_error if username is invalid.
     */
    std::string getTargetUsername();
    /**
     * Prompts the user to enter their username.
     * @return Validated username.
     * @throws std::runtime_error if username is invalid.
     */
    std::string getUsername();
    /**
     * Prompts the user to enter a message.
     * @return Full message string (can contain spaces).
     */
    std::string getMesssage();
    /**
     * Prompts the user to enter a file path.
     * @return File path string.
     */
    std::string getFilePath();
};