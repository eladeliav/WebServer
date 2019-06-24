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

#define GET "GET"
#define POST "POST"
#define NEW_LINE "\r\n"
#define NO_PATH "/"
#define HTTP_PLAIN "text/html"
#define WEBROOT_PATH "D:\\Programming\\Websites\\elastic-search-viewer\\build"
#define DEFAULT_PATH "D:\\Programming\\Websites\\elastic-search-viewer\\build\\index.html"
#define LOG(x) std::cout << x << std::endl
#define VERSION "HTTP/1.1"
#define SERVER_NAME "Viper WebServer V1.0"

using std::string;
using std::vector;

#define BUFFER_LEN 2048

static const vector<string> TEXT_TYPES = {"html", "txt", "php"};
static const vector<string> IMAGE_TYPES = {"jpg", "ico", "gif", "png", "jfif", "svg"};
static const vector<string> JS_TYPES = {"js"};
static const vector<string> JSON_TYPES = {"json", "map"};
static const vector<string> CSS_TYPES = {"css", "sass"};

static const std::map<vector<string>, string> TYPE_MAP = {
        {TEXT_TYPES,  "text/html; charset=utf-8"},
        {IMAGE_TYPES, "image/jpeg"},
        {JS_TYPES,    "application/javascript"},
        {CSS_TYPES,   "text/css"},
        {JSON_TYPES,  "application/json"}
};

std::ofstream ViperServer::logF = std::ofstream("log.log", std::ios::out);

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

ViperServer::http_response ViperServer::generateResponse(const ViperServer::http_request &req)
{
    ViperServer::http_response response;
    time_t ltime;
    time(&ltime);
    tm *gmt = gmtime(&ltime);

    char *asctime_remove_nl = asctime(gmt);
    asctime_remove_nl[24] = 0;
    response.date = std::string(asctime_remove_nl) + " GMT";
    response.content_type = getContentType(req.path);
    response.version = VERSION;
    response.server = SERVER_NAME;
    response.close = req.close;

    LOG("TRYING TO GET " << req.path);
    if (!getFileData(req.path, response.content, &response.content_length))
    {
        std::cout << "Couldn't get file data" << std::endl;
        response.content = "Couldn't get file data";
        response.content_length = response.content.length();
        response.content_type = HTTP_PLAIN;
    }

    return response;
}

ViperServer::http_request ViperServer::parseRequest(const std::string &raw_req)
{
    //TODO: throw exception instead
    if (raw_req.empty())
        return ViperServer::http_request();

    http_request request;

    if (raw_req.find(GET) == 0)
    {
        request.method = GET;
    } else if (raw_req.find(POST) == 0)
    {
        request.method = POST;
    } else //TODO: Throw exception instead
        return ViperServer::http_request();

    //TODO: make case for other methods

    string path_to_file = ViperServer::extractPath(raw_req);

    if (path_to_file == NO_PATH)
        path_to_file = DEFAULT_PATH;
    else
        path_to_file = WEBROOT_PATH + path_to_file;

    if (!validFile(path_to_file))
    {
        std::cout << "can't find: " << path_to_file << std::endl;
        return ViperServer::http_request();
    }

    std::string connectionLine = raw_req.substr(raw_req.find("Connection: ") + 12);
    connectionLine = connectionLine.substr(0, connectionLine.find('\n'));
    request.close = connectionLine == "Close";
    request.status = "202 OK";
    request.path = path_to_file;
    return request;
}

std::string ViperServer::http_response::str()
{
    std::stringstream response;
    response << this->version;
    response << this->status + NEW_LINE;
    response << std::string("Date: ") + this->date + NEW_LINE;
    response << std::string("Server: ") + this->server + NEW_LINE;
    if (this->close)
        response << std::string("Connection: Close") + NEW_LINE;
    response << "Content-Type: " + this->content_type + NEW_LINE;
    response << "Content-Length: " + std::to_string(this->content_length) + NEW_LINE + NEW_LINE;
    response << this->content;

    return response.str();
}

void ViperServer::handleClient(UniSocket sock)
{
    while (true)
    {
        char buf[BUFFER_LEN] = {0};
        try
        {
            sock.raw_recv(buf, BUFFER_LEN);
        } catch (UniSocketException &e)
        {
            std::cout << e << std::endl;
            break;
        }
        string request_string = buf;
        LOG("New Request");
        ViperServer::logF << "REQUEST:\n" << request_string << "\n";
        ViperServer::http_request request = parseRequest(request_string);
        ViperServer::http_response response = generateResponse(request);
        std::string response_str = response.str();

        ViperServer::logF << "RESPONSE:\n" << response_str << "\n";
        if (sock.raw_send(response_str.c_str(), response_str.length()) <= 0)
        {
            std::cout << "had send error" << std::endl;
            break;
        }
    }
    //sock.close();
    LOG("Closing connection");
}

void ViperServer::shutdownServer()
{
    this->closeFlag = false;
}

ViperServer::ViperServer(unsigned int listenPort)
{
    UniSocket serverSock(listenPort, SOMAXCONN); // declaring listening socket
    LOG("Listening for connections on port: " << listenPort);
    LOG("Running on: http://localhost:" << listenPort);
    std::vector<std::thread> allThreads; // empty vector for threads
    while (!this->closeFlag) // while running
    {
        UniSocket current;
        try
        {
            current = serverSock.accept(); // use current to save new accepted socket
        } catch (UniSocketException &e)
        {
            std::cout << e << std::endl;
            continue;
        }
        LOG("New Client");
        std::thread newThread = std::thread(handleClient, current);
        newThread.detach();
        allThreads.push_back(std::move(newThread));
        //handleClient(current);
    }
    serverSock.close();
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
