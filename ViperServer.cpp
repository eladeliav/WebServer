//
// Created by Elad Eliav on 2019-05-27.
//

#include <ctime>
#include <string>
#include <thread>
#include <vector>
#include "ViperServer.hpp"

#define NO_PATH "PATH_NOT_FOUND"

using std::string;

std::string ViperServer::ExtractPath(const std::string &url)
{
    if(url.empty())
        return NO_PATH;

    std::string path = url;

    //remove method
    path = path.substr(path.find_first_of(' ') + 1, path.length());

    path = path.substr(0, path.find_first_of(' '));
    return path;
}

void ViperServer::GetRequest(UniSocket &sock)
{
    char buf[1024];
    sock.recv(buf);
    string line = buf;

    if (line.empty())
        return;

    http_request request;

    if (line.find("GET") == 0)
    {
        request.method = "GET";
    } else
        return;

    //TODO: make case for other methods

    string path_to_file;

    request.status = "202 OK";
    request.sock = sock;
    request.path = path_to_file;

}

void ViperServer::shutdownServer()
{
    this->closeFlag = false;
}

ViperServer::ViperServer(unsigned int listenPort, ViperServer::request_handler_func r)
{
    UniSocket serverSock(listenPort, SOMAXCONN);

    this->request_handler = r;

    UniSocket current;

    std::vector<std::thread> allThreads;
    std::thread curr;
    while (!this->closeFlag)
    {
        current = serverSock.accept();
//        curr = std::thread(GetRequest, std::ref(current));
//        allThreads.push_back(curr);
    }

}