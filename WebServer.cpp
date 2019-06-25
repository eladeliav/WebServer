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
#include "WebServer.hpp"

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

#define BUFFER_LEN 2048

// file extensions for different content-types
const std::vector<std::string> TEXT_TYPES = {"html", "txt", "php"};
const std::vector<std::string> IMAGE_TYPES = {"jpg", "ico", "gif", "png", "jfif", "svg"};
const std::vector<std::string> JS_TYPES = {"js"};
const std::vector<std::string> JSON_TYPES = {"json", "map"};
const std::vector<std::string> CSS_TYPES = {"css", "sass"};

// hashmap to link between extensions and content-type
const std::map<std::vector<std::string>, std::string> TYPE_MAP = {
        {TEXT_TYPES,  "text/html; charset=utf-8"},
        {IMAGE_TYPES, "image/jpeg"},
        {JS_TYPES,    "application/javascript"},
        {CSS_TYPES,   "text/css"},
        {JSON_TYPES,  "application/json"}
};

//initializing static logger variable
std::ofstream WebServer::logF = std::ofstream("log.log", std::ios::out);

/**
 * Checks if a given object exists in a vector
 * @tparam T type of data in vector
 * @param vec vector to check
 * @param d data to check
 * @return true/false if found or not
 */
template<typename T>
bool existsInVector(const std::vector<T> vec, T d)
{
    return std::find(vec.begin(), vec.end(), d) != vec.end();
}

std::string WebServer::extractPath(const std::string &url)
{
    if (url.empty())
        return NO_PATH; // return "/" if url empty

    std::string path = url;

    path = path.substr(path.find_first_of(' ') + 1); // slice until path

    path = path.substr(0, path.find_first_of(' ')); // slice after path

    return path;
}

WebServer::http_response WebServer::generateResponse(const WebServer::http_request &req)
{
    WebServer::http_response response;
    time_t ltime;
    time(&ltime);
    tm *gmt = gmtime(&ltime);

    char *asctime_remove_nl = asctime(gmt);
    asctime_remove_nl[24] = 0;
    response.date = std::string(asctime_remove_nl) + " GMT"; // setting date
    response.content_type = getContentType(req.path); // content type
    response.version = VERSION; // version
    response.server = SERVER_NAME; // server name
    response.close = req.close; // whether to close connection or not
    response.status = req.status; // response status

    // get file data
    if (!getFileData(req.path, response.content, &response.content_length))
    {
        // if couldn't get data return 404
        std::cout << "Couldn't get file data" << std::endl;
        response.status = "404 Not Found";
        response.content = "Couldn't get file data";
        response.content_length = response.content.length();
        response.content_type = HTTP_PLAIN;
    }

    return response;
}

WebServer::http_request WebServer::parseRequest(const std::string &raw_req)
{
    //TODO: throw exception instead
    if (raw_req.empty())
        return WebServer::http_request();

    http_request request;

    // get method of request
    if (raw_req.find(GET) == 0)
    {
        request.method = GET;
    } else if (raw_req.find(POST) == 0)
    {
        request.method = POST;
    } else //TODO: Throw exception instead
        return WebServer::http_request();

    //TODO: make cases for other methods

    std::string path_to_file = WebServer::extractPath(raw_req); // extract path

    // setup path
    if (path_to_file == NO_PATH)
        path_to_file = DEFAULT_PATH; // index
    else
        path_to_file = WEBROOT_PATH + path_to_file; // website folder + path

    if (!validFile(path_to_file)) // check if file exists
    {
        std::cout << "can't find: " << path_to_file << std::endl;
        return WebServer::http_request();
    }

    // extract connection header
    std::string connectionLine = raw_req.substr(raw_req.find("Connection: ") + 12);
    connectionLine = connectionLine.substr(0, connectionLine.find('\n'));
    request.close = connectionLine == "Close"; // set connection header
    request.status = "200 OK"; // set status
    request.path = path_to_file; // set path
    return request;
}

// return string of response
std::string WebServer::http_response::str()
{
    std::string headers = strHeaders();
    headers += this->content;
    return headers;
}

// return string of all the headers in response
std::string WebServer::http_response::strHeaders()
{
    std::stringstream response;
    response << this->version + " ";
    response << this->status + NEW_LINE;
    response << std::string("Date: ") + this->date + NEW_LINE;
    response << std::string("Server: ") + this->server + NEW_LINE;
    if (this->close)
        response << std::string("Connection: Close") + NEW_LINE;
    else
        response << std::string("Connection: keep-alive") + NEW_LINE;
    response << "Content-Type: " + this->content_type + NEW_LINE;
    response << "Content-Length: " + std::to_string(this->content_length) + NEW_LINE + NEW_LINE;

    return response.str();
}

// handle a single client
void WebServer::handleClient(UniSocket sock)
{
    WebServer::http_request request; // request and response empty structs
    WebServer::http_response response;
    while (!request.close) // while the connection close flag is off
    {
        char buf[BUFFER_LEN] = {0}; // zeroing out a new buffer
        try
        {
            sock.raw_recv(buf, BUFFER_LEN); // receiving message
        } catch (UniSocketException& e)
        {
            LOG(e << " on sock # " << sock.getSockId());
            WebServer::logF << "Sock # " << sock.getSockId() << " had error of type: " << e.getError() << "\n";
            break;
        }
        std::string request_string = buf; // stringify buffer
        LOG("New Request");
        WebServer::logF << "REQUEST on sock # " << sock.getSockId() << ":\n" << request_string << "\n"; // logging request
        request = parseRequest(request_string); // parsing request
        response = generateResponse(request); // generating response for request
        std::string response_str = response.str(); // getting string value of response

        WebServer::logF << "RESPONSE on sock # " << sock.getSockId() << ":\n" << response.strHeaders() << "\n"; // logging response
        LOG("TRYING TO GET " << request.path << " on sock # " << sock.getSockId());

        try
        {
            sock.raw_send(response_str.c_str(), response_str.length()); // sending response
        }
        catch(UniSocketException& e)
        {
            LOG(e << " on sock # " << sock.getSockId());
            WebServer::logF << "Sock # " << sock.getSockId() << " had error of type: " << e.getError() << "\n";
            break;
        }
    }
    // closing sock since either timed out, disconnected, close flag was on, or had an error
    LOG("Closing connection on sock # " << sock.getSockId());
    WebServer::logF << "Closing socket # " << sock.getSockId() << "\n";
    sock.close();
}

void WebServer::shutdownServer()
{
    this->closeFlag = false;
}

bool WebServer::getFileData(const std::string &path, std::string &response, int *size)
{
    std::ifstream t(path, std::ifstream::binary); // open file
    static std::stringstream buffer;
    buffer.str(""); // init buffer
    buffer << t.rdbuf();  // get data into buffer
    t.close(); // close file
    if (buffer.str().empty())
        return false; // check if empty
    *size = buffer.str().length(); // get size
    response = buffer.str(); // get data
    return true;
}

std::string WebServer::getFileExtension(const std::string &path)
{
    return path.substr(path.rfind('.') + 1, path.length()); // extract extension of a given file
}

std::string WebServer::getContentType(const std::string &path)
{
    // use the TYPE_MAP to get the needed content-type for a file
    std::string extension = getFileExtension(path);
    for (auto &pair : TYPE_MAP)
    {
        if (existsInVector(pair.first, extension))
        {
            return pair.second;
        }
    }
    return HTTP_PLAIN;
}

WebServer::WebServer(unsigned int listenPort)
{
    UniSocket serverSock(listenPort, SOMAXCONN); // declaring listening socket
    LOG("Listening for connections on port: " << listenPort);
    LOG("Running on: http://localhost:" << listenPort);
    std::vector<std::thread> allThreads; // empty vector for threads
    UniSocket current;
    while (!this->closeFlag) // while running
    {
        try
        {
            current = serverSock.accept(); // use current to save new accepted socket
        } catch (UniSocketException &e)
        {
            std::cout << e << std::endl;
            break;
        }
        LOG("New Client " << current.getSockId());
        std::thread newThread = std::thread(handleClient, current); // start new thread for handling new client
        newThread.detach(); // detach thread
        allThreads.push_back(std::move(newThread)); // save thread
        //handleClient(current);
    }
    current.close();
    serverSock.close();
}
