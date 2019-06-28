<!--
*** Thanks for checking out this README Template. If you have a suggestion that would
*** make this better please fork the repo and create a pull request or simple open
*** an issue with the tag "enhancement".
*** Thanks again! Now go create something AMAZING! :D
-->





<!-- PROJECT SHIELDS -->
[![Build Status][build-shield]]()
[![Contributors][contributors-shield]]()


<!-- PROJECT LOGO -->
<br />
<p align="center">
  <a href="https://github.com/eladeliav/WebServer">
    <img src="https://cdn0.iconfinder.com/data/icons/network-and-security-3-1/512/793-21-512.png" alt="Logo" width="80" height="80">
  </a>

  <h1 align="center">WebServer</h1>

  <p align="center">
    A relatively simple web server written in C++ with my own UniSockets library.
    <br />
    This project was made as part of my 12th grade software engineering project.
    See my <a href="https://github.com/eladeliav/">github profile</a> for the full project.
    <br />
    <!--<a href="https://github.com/othneildrew/Best-README-Template"><strong>Explore the docs »</strong></a>
    <br />
    <br />
    <a href="https://github.com/othneildrew/Best-README-Template">View Demo</a>
    ·
    <a href="https://github.com/othneildrew/Best-README-Template/issues">Report Bug</a>
    ·
    <a href="https://github.com/othneildrew/Best-README-Template/issues">Request Feature</a>
    -->
  </p>
</p>



<!-- TABLE OF CONTENTS -->
## Table of Contents

* [About the Project](#about-the-project)
  * [Built With](#built-with)
* [Getting Started](#getting-started)
* [Usage](#usage)
* [Goals](#goals)
* [License](#license)
* [Contact](#contact)
* [Acknowledgements](#acknowledgements)



<!-- ABOUT THE PROJECT -->
## About The Project
<!--[![Product Name Screen Shot][product-screenshot]](https://example.com))-->

This project is basically a library I wrote in c++ as a wrapper for winsock and BSD sockets. 
My WebServer library basically works as a cross-platform sockets wrapper which is very easy to use and
even includes a decent amount of exception handling. As stated before, this project was made
as part of my 12th grade software engineering project which includes a lot more than just this library.
Please feel free to check out the other parts of the project on my <a href="https://github.com/eladeliav/">github</a>
profile.
### Built With
* [CLion](https://www.jetbrains.com/clion/)
* [CMake](https://cmake.org/)
* [C++](http://www.cplusplus.com/doc/tutorial/)
* [UniSockets](https://github.com/eladeliav/UniSockets)



<!-- GETTING STARTED -->
## Getting Started

This is an example of how you may give instructions on setting up your project locally.
To get a local copy up and running follow these simple example steps.



### Installation

1. Clone the repo
<br />```git clone https:://github.com/eladeliav/WebServer.git```
2. Use cmake to generate a build folder with either of the following
    * Unix Make:
        - Navigate to ```WebServer/build``` and run cmake with your needed settings
            ```bash
            cmake ../
            make
            ```
    * Mingw:
        - Navigate to ```WebServer/build``` and run cmake with your needed settings
            ```bash
            cmake ../
            mingw32-make
            ```


<!-- USAGE EXAMPLES -->
## Usage

This application is currently still under development so usage of it isn't very friendly. In order
to have it run your own website, you have to enter the code and change the WEBROOT_PATH variable
to the path of your website. After that, simple run ```build/WebServer``` and you're good to go.

##### Commands:
Currently there is only one command:
* ```quit``` - to quit the program

<!-- GOALS -->
## Goals

As stated before, this library is a work-in-progress for my 12th grade project. So I still have some
goals to accomplish.

- [ ] Maybe make a class called http_connection that handles all send/recv for the http protocol 
- [ ] General code clean up
- [ ] Documentation



<!-- LICENSE -->
## License

Distributed under the MIT License. See LICENSE for more information.


<!-- CONTACT -->
## Contact

Elad Eliav - elad02.eliav@gmail.com

Project Link: [https://github.com/eladeliav/WebServer](https://github.com/eladeliav/WebServer)



<!-- ACKNOWLEDGEMENTS -->
## Acknowledgements
* [GitHub Emoji Cheat Sheet](https://www.webpagefx.com/tools/emoji-cheat-sheet)
* [Img Shields](https://shields.io)
* [Choose an Open Source License](https://choosealicense.com)





<!-- MARKDOWN LINKS & IMAGES -->
[build-shield]: https://img.shields.io/badge/build-passing-brightgreen.svg?style=flat-square
[contributors-shield]: https://img.shields.io/badge/contributors-1-orange.svg?style=flat-square
[license-url]: https://choosealicense.com/licenses/mit
[linkedin-shield]: https://img.shields.io/badge/-LinkedIn-black.svg?style=flat-square&logo=linkedin&colorB=555
[linkedin-url]: https://linkedin.com/in/othneildrew
[product-screenshot]: https://raw.githubusercontent.com/othneildrew/Best-README-Template/master/screenshot.png
