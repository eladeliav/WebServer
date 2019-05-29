#include <iostream>
#include "ViperServer.hpp"

int main()
{
    try
    {
        ViperServer webServer(7777);
    } catch (UniSocketException &e)
    {
        std::cout << e << std::endl;
    }
    return 0;
}