// Main.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "Application.h"
#include <iostream>
#include <exception>
#include <boost/endian/conversion.hpp>

#define BOOST_ASIO_DISABLE_FUNCSIG

int main()
{
    try
    {
        Application app;
        app.run();
    }
    catch (const std::exception& e)
    {
        std::cerr << "Unhandled exception: " << e.what() << std::endl;
    }
    catch (...)
    {
        std::cerr << "Unknown exception occurred!" << std::endl;
    }
    return 0;
}
