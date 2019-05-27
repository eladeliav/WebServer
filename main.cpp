#include <iostream>
#include "ViperServer.hpp"

int main()
{
    std::string x = "GET /hello.htm HTTP/1.1\nUser-Agent: Mozilla/4.0 (compatible; MSIE5.01; Windows NT)\nHost: www.tutorialspoint.com\nAccept-Language: en-us\nAccept-Encoding: gzip, deflate\nConnection: Keep-Alive";
    std::cout << ViperServer::ExtractPath(x);
    return 0;
}