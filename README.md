FishAnnotator                                                  {#mainpage}
============

Introduction
============

**FishAnnotator** is a standalone, cross-platform application for creating 
and visualizing video annotations.  It currently supports annotations in 
the form of bounding boxes, lines, and dots.  Annotations are saved in the 
widely supported JSON format, which can be imported separately for use in 
algorithms or other tools.  Species (annotation labels) can be specified by 
the user with the flexible graphical user interface.  It includes a single 
frame increment and decrement, and guarantees frame level accuracy for 
annotations, even after seeking.  Videos are decoded using FFMPEG, so a 
wide variety of codecs are supported; there is no need to convert your 
videos to imagery or other format before annotating.

![fishannotatorscreenshot][Screenshot]

Getting started
===============

* [Download the latest release][LatestRelease]
* [Build from source](doc/build.md)
* [Developer info](doc/developer.md)

[LatestRelease]: http://github.com/BGWoodward/FishAnnotator/releases
[Screenshot]: https://user-images.githubusercontent.com/7937658/29241637-a113a9de-7f4b-11e7-8a94-93fc9226546a.png

