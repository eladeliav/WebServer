//
// Created by Elad Eliav on 2019-05-27.
//

#include <ctime>
#include <string>
#include <thread>
#include <vector>
#include <iostream>
#include <map>
#include <fstream>
#include "ViperServer.hpp"

#define NEW_LINE "\r\n"
#define NO_PATH "NO_PATH_FOUND"
#define HTTP_PLAIN "text/plain"
#define WEBROOT_PATH "../webroot/"
#define DEFAULT_PATH "../webroot/index.html"


using std::string;
using std::vector;

static const vector<string> TEXT_TYPES = {"html", "txt"};
static const vector<string> IMAGE_TYPES = {"jpg", "ico", "gif", "png", "jfif", "svg"};
static const vector<string> JS_TYPES = {"js"};
static const vector<string> CSS_TYPES = {"css"};

static const std::map<vector<string>, string> TYPE_MAP = {
        {TEXT_TYPES, "text/html; charset=utf-8"},
        {IMAGE_TYPES, "image/jpeg"},
        {JS_TYPES, "application/javascript"},
        {CSS_TYPES, "text/css"}
};

template<typename T>
bool existsInVector(const vector<T> vec, T d)
{
    for(auto& val : vec)
    {
        if(d == val)
            return true;
    }
    return false;
}

std::string ViperServer::extractPath(const std::string &url)
{
    if (url.empty())
        return NO_PATH;

    std::string path = url;

    //remove method
    path = path.substr(path.find_first_of(' ') + 1, path.length());

    path = path.substr(0, path.find_first_of(' '));
    return path;
}

void ViperServer::getRequest(UniSocket &sock)
{
    char buf[1024];
    try
    {
        sock.recv(buf);
    }catch(UniSocketException& e)
    {
        std::cout << e << std::endl;
        return;
    }


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

    string path_to_file = ViperServer::extractPath(line);

    if(path_to_file == NO_PATH)
        path_to_file = DEFAULT_PATH;
    else
        path_to_file = WEBROOT_PATH + path_to_file;

    if(!validFile(path_to_file))
    {
        std::cout << "can't find: " << path_to_file << std::endl;
        return;
    }

    request.status = "202 OK";
    request.sock = sock;
    request.path = path_to_file;

    //request_handler(request);

    time_t ltime;
    time(&ltime);
    tm *gmt = gmtime(&ltime);

    char *asctime_remove_nl = asctime(gmt);
    asctime_remove_nl[24] = 0;

    int contentLength = 0;
    getFileData(path_to_file, &request.response, &contentLength);
    string contentType = getContentType(path_to_file);

    string response = "HTTP/1.1";
    response += request.status + NEW_LINE;
    response += std::string("Date: ") + asctime_remove_nl + " GMT" + NEW_LINE;
    response += std::string("Server: ") + "Viper WebServer 0.1" + NEW_LINE;
    response += std::string("Connection: close") + NEW_LINE;
    response += "Content-Type: " + contentType + NEW_LINE;
    response += "Content-Length: " + std::to_string(contentLength) + NEW_LINE + NEW_LINE;
    response += request.response;

    sock.send(response.c_str(), response.size());
    sock.close();
}

void ViperServer::shutdownServer()
{
    this->closeFlag = false;
}

ViperServer::ViperServer(unsigned int listenPort)
{
    UniSocket serverSock(listenPort, SOMAXCONN);

    UniSocket current;

    std::vector<std::thread> allThreads;
    while (!this->closeFlag)
    {
        try
        {
            current = serverSock.accept();
        }catch(UniSocketException& e)
        {
            std::cout << e << std::endl;
            continue;
        }

        allThreads.push_back(std::thread(getRequest, std::ref(current)));
    }

}

bool ViperServer::getFileData(const std::string &path, void *buf, int *size)
{
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    std::streamsize fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    if (file.read((char*)buf, fileSize))
    {
        *size = fileSize;
        return true;
    }
    return false;
}

std::string ViperServer::getFileExtension(const std::string &path)
{
    string extension = path.substr(path.rfind('.') + 1, path.length());
    return extension;
}

std::string ViperServer::getContentType(const std::string &path)
{
    string extension = getFileExtension(path);
    for(auto& pair : TYPE_MAP)
    {
        if(existsInVector(pair.first, extension))
        {
            return pair.second;
        }
    }
    return HTTP_PLAIN;
}
