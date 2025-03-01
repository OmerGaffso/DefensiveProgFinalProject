#pragma once
#include "Utility.h"

class InputHandler
{
public:
	virtual std::string getInput() = 0;
	virtual ~InputHandler() = default;
};

