/*
Application.h class
This class will 
*/
#pragma once
#include <unordered_map>
#include <functional>
#include "UI.h"
#include "InputHandler.h"
#include "ConsoleInput.h"
#include "FileInput.h"

class Application
{
private:
	InputHandler* inputHandler;
	UI ui;
	bool appRunning;
	std::unordered_map<int, std::function<void()>> commandMap;

	void initCommands();
public:
	Application();
	~Application();

	void run();
	void processChoice(int choice);

	// Command functions:
	void registerUser();
	void requestClientList();
	void requestPublicKey();

};

