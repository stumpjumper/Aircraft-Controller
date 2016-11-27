lighting-controller [![Build Status](https://travis-ci.org/stumpjumper/lighting-controller.svg?branch=master)](https://travis-ci.org/stumpjumper/lighting-controller)
===================

Aurduino code written in C++ to [control lights via the "Lucky7" custom shield](http://hackaday.com/2014/11/14/powering-your-f-16-with-an-arduino/). 
The code is currently being used at The National Museum of Nuclear Science and History in Albuquerque, NM. 

How it works
============

Before loading onto an Aurduino, you may first want to test the code by doing the following.

    $ git clone https://github.com/stumpjumper/lighting-controller.git
    $ cd lighting-controller
    $ git submodule update --init --recursive
    $ cd controller
    $ touch depend
    $ make cpp_links
    $ make
    $ make test
