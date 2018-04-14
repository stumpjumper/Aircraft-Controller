lighting-controller [![Build Status](https://travis-ci.org/stumpjumper/lighting-controller.svg?branch=master)](https://travis-ci.org/stumpjumper/lighting-controller)
===================

Aurduino code written in C++ to [control lights via the "Lucky7" custom shield](http://hackaday.com/2014/11/14/powering-your-f-16-with-an-arduino/). 
The code is currently being used at [The National Museum of Nuclear Science and History](http://nuclearmuseum.org) in Albuquerque, NM to control the aircraft lights on the Museum's [F-16](http://www.nuclearmuseum.org/support/operation-preservation-the-campaign-to-restore-the-planes/completed-f-16-fighting-falcon), [B-29](http://www.nuclearmuseum.org/support/operation-preservation-the-campaign-to-restore-the-planes/b-29-restoration-project) and [B-52B](http://www.nuclearmuseum.org/support/operation-preservation-the-campaign-to-restore-the-planes/b-52b-stratofortress-restoration-project) in [Heritage Park](http://www.nuclearmuseum.org/see/exhibits/heritage-park/).

How it works
============

Before loading onto an Aurduino, you may first want to test the code by doing the following.

    $ git clone https://github.com/stumpjumper/lighting-controller.git
    $ cd lighting-controller
    # Get the needed Google Test files
    $ git submodule update --init --recursive
    $ cd controller
    $ touch depend
    # To be able to compile stand alone and to be used as an Aurdiono sketch, some file location
    # gymnastics are neccessary via symbolic links
    # On a Mac you are going to need to first install XCode, then it will ask you to install the command line tools
    # the first time you run make
    $ make cpp_links
    # Get all the paths so dependencies can be resolved
    # If you need makedepend on your Mac, you get install using HomeBrew: http://macappstore.org/makedepend/
    $ make depends
    # Copile, then test
    $ make
    $ make test

To load the code as a Sketch in the Aurduino IDE, do the following.

    #If you havn't already cloned the git repository
    $ git clone https://github.com/stumpjumper/lighting-controller.git
    # Then you need to set up symbolic links so that all needed files are in the right place for the Aurduino IDE
    $ cd lighting-controller
    $ cd controller
    $ make arduino_links
    
Now you can run the Arduino IDE and go to the directory of whatever controller you want to load and simply open the .ino file.  For example open lighting-controller/controllers/B-29/B-29.ino from the IDE to create the B-29 sketch
