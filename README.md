lighting-controller [![Build Status](https://travis-ci.org/stumpjumper/lighting-controller.svg?branch=master)](https://travis-ci.org/stumpjumper/lighting-controller)
===================

Aurduino code written in C++ to [control lights via the "Lucky7" custom shield](http://hackaday.com/2014/11/14/powering-your-f-16-with-an-arduino/). 
The code is currently being used at [The National Museum of Nuclear Science and History](http://nuclearmuseum.org) in Albuquerque, NM to control the aircraft lights on the Museum's [F-16](http://www.nuclearmuseum.org/support/operation-preservation-the-campaign-to-restore-the-planes/completed-f-16-fighting-falcon), [B-29](http://www.nuclearmuseum.org/support/operation-preservation-the-campaign-to-restore-the-planes/b-29-restoration-project) and [B-52B](http://www.nuclearmuseum.org/support/operation-preservation-the-campaign-to-restore-the-planes/b-52b-stratofortress-restoration-project) in [Heritage Park](http://www.nuclearmuseum.org/see/exhibits/heritage-park/).

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
