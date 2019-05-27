//
// Created by Elad Eliav on 2019-05-27.
//

#ifndef WEBSERVER_VIPERSERVER_HPP
#define WEBSERVER_VIPERSERVER_HPP

#include <string>
#include <UniSockets/UniSocket.hpp>

using std::string;

class ViperServer
{
public:
    struct http_request
    {
        UniSocket sock;
        string method;
        string path;
        string status;
        string response;
    };
    typedef void(*request_handler_func) (http_request);
    ViperServer(unsigned int listenPort, request_handler_func r);
    void shutdownServer();
    static std::string ExtractPath(const std::string& url);
private:

    static void GetRequest(UniSocket& sock);
    request_handler_func request_handler;
    bool closeFlag = false;
};


#endif //WEBSERVER_VIPERSERVER_HPP
