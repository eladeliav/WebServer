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
#include <sstream>
#include <algorithm>
#include "ViperServer.hpp"

#define NEW_LINE "\r\n"
#define NO_PATH "/"
#define HTTP_PLAIN "text/plain"
#define WEBROOT_PATH "D:\\Programming\\Websites\\History Project 2017\\"
#define DEFAULT_PATH "D:\\Programming\\Websites\\History Project 2017\\index.html"


using std::string;
using std::vector;

#define BUFFER_LEN 2048

static const vector<string> TEXT_TYPES = {"html", "txt", "php"};
static const vector<string> IMAGE_TYPES = {"jpg", "ico", "gif", "png", "jfif", "svg"};
static const vector<string> JS_TYPES = {"js", "min.js"};
static const vector<string> CSS_TYPES = {"css", "sass"};

static const std::map<vector<string>, string> TYPE_MAP = {
        {TEXT_TYPES,  "text/html; charset=utf-8"},
        {IMAGE_TYPES, "image/jpeg"},
        {JS_TYPES,    "application/javascript"},
        {CSS_TYPES,   "text/css"}
};

std::ofstream ViperServer::logF = std::ofstream("log.txt");

template<typename T>
bool existsInVector(const vector<T> vec, T d)
{
    return std::find(vec.begin(), vec.end(), d) != vec.end();
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
    char buf[BUFFER_LEN];
    try
    {
        sock.raw_recv(buf, BUFFER_LEN);
    } catch (UniSocketException &e)
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

    if (path_to_file == NO_PATH)
        path_to_file = DEFAULT_PATH;
    else
        path_to_file = WEBROOT_PATH + path_to_file;

    if (!validFile(path_to_file))
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
    string contentType = getContentType(path_to_file);

    if (!getFileData(path_to_file, request.response, &contentLength))
    {
        std::cout << "Couldn't get file data" << std::endl;
        request.response = "Couldn't get file data";
        contentLength = std::string(request.response).length();
        contentType = HTTP_PLAIN;
    }

    std::stringstream response;
    response << "HTTP/1.1 ";
    response << request.status + NEW_LINE;
    response << std::string("Date: ") + asctime_remove_nl + " GMT" + NEW_LINE;
    response << std::string("Server: ") + "Viper WebServer 0.1" + NEW_LINE;
    response << std::string("Connection: close") + NEW_LINE;
    response << "Content-Type: " + contentType + NEW_LINE;
    response << "Content-Length: " + std::to_string(contentLength) + NEW_LINE + NEW_LINE;
    response << request.response;

    std::cout << "Sending over: " << path_to_file << " with content of type: " << contentType << std::endl;
    if (sock.raw_send(response.str().c_str(), response.str().length()) <= 0)
        std::cout << "had send error" << std::endl;

    ViperServer::logF << response.str();
}

void ViperServer::shutdownServer()
{
    this->closeFlag = false;
}

void joinThreads(vector<std::thread> &vec)
{
    for (auto &t : vec)
    {
        if (t.joinable())
        {
            t.join();
        }
    }
}

ViperServer::ViperServer(unsigned int listenPort)
{
    UniSocket serverSock(listenPort, SOMAXCONN);
    ViperServer::logF << "LOG";
    static UniSocket current;
    static std::vector<std::thread> allThreads;
    std::thread currentThread;
    while (!this->closeFlag)
    {
        try
        {
            current = serverSock.accept();
        } catch (UniSocketException &e)
        {
            std::cout << e << std::endl;
            continue;
        }
        currentThread = std::thread(getRequest, std::ref(current));
        currentThread.detach();
        allThreads.push_back(std::move(currentThread));
        //getRequest(current);
        //allThreads.push_back(std::thread(getRequest, std::ref(current)));
    }
    joinThreads(allThreads);
}

bool ViperServer::getFileData(const std::string &path, string &response, int *size)
{
    std::ifstream t(path, std::ifstream::binary);
    static std::stringstream buffer;
    buffer.str("");
    buffer << t.rdbuf();
    t.close();
    if (buffer.str().empty())
        return false;
    *size = buffer.str().length();
    response = buffer.str();
    return true;
}

std::string ViperServer::getFileExtension(const std::string &path)
{
    return path.substr(path.rfind('.') + 1, path.length());
}

std::string ViperServer::getContentType(const std::string &path)
{
    string extension = getFileExtension(path);
    for (auto &pair : TYPE_MAP)
    {
        if (existsInVector(pair.first, extension))
        {
            return pair.second;
        }
    }
    return HTTP_PLAIN;
}
