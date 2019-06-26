#include <iostream>
#include "WebServer.hpp"

//TODOS HERE
//TODO: separate to more classes like http_connection with send/recv made for it

int main()
{
    try
    {
        WebServer webServer(7777);
    } catch (UniSocketException &e)
    {
        std::cout << e << std::endl;
    }
}