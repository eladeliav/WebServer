//
// Created by Elad Eliav on 2019-05-27.
//

#ifndef WEBSERVER_VIPERSERVER_HPP
#define WEBSERVER_VIPERSERVER_HPP

#include <string>
#include <fstream>
#include <UniSockets/UniSocket.hpp>

class ViperServer
{
public:
    struct http_request
    {
        UniSocket sock;
        std::string method;
        std::string path;
        std::string status;
        std::string response;
    };

    ViperServer(unsigned int listenPort);

    void shutdownServer();

private:

    static void getRequest(UniSocket sock);

    bool closeFlag = false;

    static std::string extractPath(const std::string &url);

    static bool getFileData(const std::string &path, std::string &response, int *size);

    static std::string getFileExtension(const std::string &path);

    static std::string getContentType(const std::string &path);

    static inline bool validFile(const std::string &name)
    {
        std::ifstream f(name.c_str());
        return f.good();
    }

    static std::ofstream logF;

};


#endif //WEBSERVER_VIPERSERVER_HPP
