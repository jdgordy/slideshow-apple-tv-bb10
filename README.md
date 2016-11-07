# slideshow-apple-tv-bb10
Slideshow for Apple TV source code for BlackBerry 10

## Overview
This project contains source for my "Slideshow for Apple TV" BlackBerry 10
app on BlackBerry world.  This app provides the ability to send a slideshow
of images from BB10 device to an Apple TV via the AirPlay protocol for
display on the user's TV.  It supports device discovery on the local network
using Bonjour, slideshow transitions, and basic password support via digest
authentication.
I've decided to make the source publicly available because I don't plan to
maintain the app going forward.

## Getting Started
In order to build the source, you'll need to install the BlackBerry 10
developer tools (Momentics IDE) and BlackBerry 10.3 SDK available
[here](http://developer.blackberry.com/native).  I've only developed and
tested this app on physical BB10 devices, but the app should be able to run
on a BB10 device simulator available in the developer tools.

## Credits
This app leverages efforts by others to reverse engineer parts of the AirPlay
protocol, and also uses the "Bonjour for Qt" github project, with a few key
changes specific to a BB10 implementation.  More information can be found at
the following:
- [Bonjour for Qt](https://github.com/kuro/qtbonjour)
- [Open AirPlay](https://github.com/jamesdlow/open-airplay/)
- [Unofficial AirPlay Protocol Specification](http://nto.github.io/AirPlay.html)

Application icons for non-commercial use were sourced from the following:
- [www.visualpharm.com](http://www.visualpharm.com)
- [www.dailyoverview.com](http://www.dailyoverview.com)

Please note that Apple TV and AirPlay are trademarks of Apple Inc., registered
in the U.S. and other countries.  This application and source code make no claim
to be either authorized or approved by Apple Inc.
