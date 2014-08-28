Calling this a project seems wrong.

This is a fun experiment that scratches an itch with a particular retro game for which I could find no maps. It was also an excuse to play with C++. I am far from accomplished with C++. I know enough to get around a code base, as long as it doesn't go to far into advanced techniques; don't expect any Template Specification from me. I would not recommend using this as an example of how to write C++&mdash;that would surely be misguided.


### Dependencies

This "project" uses [ImageMagick](http://www.imagemagick.org) through the C++ wrapper [Magick++](http://www.imagemagick.org/script/magick++.php). Because it draws text, it needs to be compiled with X11 support. On OS X, this is easy to do with homebrew, but does require [XQuartz](https://xquartz.macosforge.org/landing/).

So:

* Install [XQuartz](https://xquartz.macosforge.org/landing/).
* Log out and log back in.
* Run ```brew install imagemagick --with-x11```.
* Run ```make```.
