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
        http_request() : close(false) { }
        std::string method;
        std::string path;
        std::string status;
        bool close;
    };

    struct http_response
    {
        http_response() : close(false) { }
        std::string version;
        std::string status;
        std::string date;
        std::string server;
        std::string content_type;
        int content_length;
        std::string content;
        bool close;
        std::string str();
        std::string strHeaders();
    };

    ViperServer(unsigned int listenPort);

    void shutdownServer();

private:

    static void handleClient(UniSocket sock);

    bool closeFlag = false;

    static http_request parseRequest(const std::string& raw_req);

    static ViperServer::http_response generateResponse(const http_request& req);

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
