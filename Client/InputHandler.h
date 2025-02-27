#pragma once
#include "GenDef.h"

class InputHandler
{
public:
	virtual std::string getInput() = 0;
	virtual ~InputHandler() = default;
};

