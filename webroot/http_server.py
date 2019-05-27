# HTTP Server Shell
# Author: Barak Gonen
# Purpose: Provide a basis for Ex. 4.4
# Note: The code is written in a simple way, without classes,
# log files or other utilities, for educational purpose
# Usage: Fill the missing functions and constants

import socket
import os.path

IP = "0.0.0.0"
PORT = 8080
SOCKET_TIMEOUT = 10  # write in seconds
DEFAULT_URL = "index.html"

REDIRECTION_DICTIONARY = {"js/box1.js": "box.js"}


def get_file_data(filename):
    """ Get data from file """
    data = None
    try:
        with open(filename, "rb") as file:
            data = file.read(1024)
            temp = data
            while temp:
                temp = file.read(1024)
                data += temp
    except Exception as e:
        exit()
    return data


def handle_client_request(resource, client_socket):
    """
    Check the required resource,
    generate proper HTTP response and send to client
    """
    # TO DO : add code that given a resource (URL and parameters)
    # generates the proper response

    # URL and PARAMETERS
    resource = resource.replace("%20", " ")
    url = ""
    parameters = ""
    if "?" in resource:
        url = resource[:resource.find("?")]
        if resource.find("?") < len(resource):
            parameters = resource[resource.find("?") + 1:]
        else:
            parameters = ""
    else:
        url = resource
    if url == '':
        url = DEFAULT_URL

    # TO DO: check if URL had been redirected,
    # not available or other error code. For example:
    print url
    if url in REDIRECTION_DICTIONARY:
        client_socket.send(three02(REDIRECTION_DICTIONARY[url]))
        return False
        # TO DO: send 302 redirection response

    if url == "calculate-next":
        print parameters
        if len(parameters) < 5:
            return False
        if parameters[0:4] == "num=" and parameters[4:].isdigit():
            client_socket.send(calculate_next(parameters[4:]))
            return False
        return False

    if url == "calculate-area":
        split_params = parameters.split("&")
        if len(split_params) < 2:
            return False
        if split_params[0][:7] == "height=" and split_params[0][7:].isdigit() and \
                split_params[1][:6] == "width=" and split_params[1][6:].isdigit():
            client_socket.send(calculate_area(split_params[0][7:], split_params[1][6:]))
            return False
        return False

    if not os.path.isfile(url):
        print "Not A File!"
        client_socket.send(four04())
        return True

    data = get_file_data(url)

    http_header = ""

    # FILETYPE
    filetype = url[url.rfind(".") + 1:]
    print filetype
    # HEADER
    http_header = get_http_header(filetype, data)

    # TO DO: read the data from the file
    http_response = http_header + data
    print http_header
    # exit()
    client_socket.send(http_response)
    return True


def validate_http_request(request):
    """6
    Check if request is a valid HTTP request
    and returns TRUE / FALSE and the requested URL
    """
    return True, request[5:request[5:].find(" ") + 5]
    # TO DO: write function


def handle_client(client_socket):
    """
    Handles client requests: verifies client's requests are legal HTTP,
    calls function to handle the requests
    """
    print 'Client connected\n' # <-- TODO UNCOMMENT
    stay_on_client = True
    while stay_on_client:
        try:
            client_request = client_socket.recv(1024)
        except socket.timeout as e:
            print e
            return
        valid_http, resource = validate_http_request(client_request)
        if valid_http:
            print 'Got a valid HTTP request'
            stay_on_client = handle_client_request(resource, client_socket)
        else:
            print 'Error: Not a valid HTTP request'
            break
        # break   # <-- TODO COMMENT
    print 'Closing connection\n' # <-- TODO UNCOMMENT
    client_socket.close()


def main():
    global DEFAULT_URL
    port = raw_input("Enter Port: ")
    homepage = raw_input("Enter Index File (i.e. 'index.html'): ")

    if not port.isdigit():
        print port + " is not a valid port, using default port ({})".format(PORT)
        port = PORT
    if homepage:
        DEFAULT_URL = homepage

    # Open a socket and loop forever while waiting for clients
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_socket.bind((IP, int(port)))
    server_socket.listen(10)
    print "Listening for connections on port {}\n".format(port)

    while True:
        client_socket, client_address = server_socket.accept()
        print 'New connection received' #<-- TODO UNCOMMENT
        client_socket.settimeout(SOCKET_TIMEOUT)
        try:
            handle_client(client_socket)
        except socket.timeout as e:
            print "timed out"
        except socket.error as e:
            print "Socket Error: " + str(e)


def get_http_header(filetype, data):
    http_header = ""
    if filetype in ['html', 'js', 'txt']:
        http_header = """HTTP/1.1 200 ok
Content-Length:  {}
content-Type: {}

""".format(len(data), "text/html; charset=utf-8")

    elif filetype in ["jpg", "ico", "gif", "png", "jfif", "svg"]:
        http_header = """HTTP/1.1 200 ok
Content-Length:  {}
content-Type: {}

""".format(len(data), "image/jpeg")

    elif filetype == 'css':
        http_header = """HTTP/1.1 200 ok
Content-Length:  {}
content-Type: {}

""".format(len(data), "text/css")
    return http_header


def four04():
    data = """<!DOCTYPE html>
    <html>
      <head>
      </head>
      <body>
        <h1>404 Not Found</h1>
      </body>
    </html>"""
    return ("""HTTP/1.1 404 Not Found
    Content-Length:  {}
    content-Type: {}

    """.format(len(data), "text/html; charset=utf-8") + data)


def calculate_next(parameters):
    data = str(int(parameters) + 1)
    return ("""HTTP/1.1 200 ok
        Content-Length:  {}
        content-Type: {}

        """.format(len(data), "text/html; charset=utf-8") + data)


def calculate_area(param1, param2):
    data = str(float(int(param1) * int(param2)) / 2.0)
    return ("""HTTP/1.1 200 ok
        Content-Length:  {}
        content-Type: {}

        """.format(len(data), "text/html; charset=utf-8") + data)


def three02(location):
    return ("""HTTP/1.1 302 Moved Temporarily
location: {}

        """.format(location))


if __name__ == "__main__":
    # Call the main handler function
    main()
