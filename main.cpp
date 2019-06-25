#include <iostream>
#include "WebServer.hpp"

int main()
{
    try
    {
        WebServer webServer(80);
    }catch(UniSocketException& e)
    {
        std:: cout << e << std::endl;
    }


}