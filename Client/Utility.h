#pragma once

#include <iostream>
#include <string>

uint8_t constexpr USERNAME_MAX_LENGTH = 254; // leaving place for null termination. 

bool isValidUsername(const std::string& username);
std::string trim(const std::string& str);
