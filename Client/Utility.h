#pragma once

#include <iostream>
#include <string>

#define MIN_USERNAME_LEN  3U
#define MAX_USERNAME_SIZE 255U


bool isValidUsername(const std::string& username);
std::string trim(const std::string& str);
