cmake_minimum_required(VERSION 3.14)
project(WebServer)

set(TARGET_NAME ${PROJECT_NAME})
set(${TARGET_NAME}_SRC
        main.cpp
        ViperServer.cpp
        ViperServer.hpp
        )
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-std=c++11")

if (WIN32)
    set(UniSockets_DIR "D:/Programming/VisualStudioProjects/CPP/UniSockets/build/")
else ()
    set(UniSockets_DIR "/Users/eladeliav/Documents/Programming/C++/UniSockets/build")
endif ()
find_package(UniSockets REQUIRED)

add_executable(${PROJECT_NAME} ${${PROJECT_NAME}_SRC})
target_link_libraries(${PROJECT_NAME} UniSockets::UniSockets)