# Rasterizer
a simple software rasterizer written in C++ with a helper script in python to convert image files (cause opencv for c++ was broken on my system)

My friend made a cube in the desmos graphing calculator (the 2d one, not the 3d one), so i made a cube in desmos, it was horrible so i made a cube in sdl, and i then decided to take it to the next level
<br/><br/>
this was also inspired by Sebastian Lague's [Software Rasterizer](https://github.com/SebLague/Software-Rasterizer). He truly is in a league of his own, that Sebastian Lague guy
<br/><br/>
also, it relies on finding the executable path, which is a function i only made for linux, to make it work for other systems either provide a hard-coded absolute path and remove my getExecutablePath() in raster.hpp, or actually implement getExecutablePath() for your system

to compile:
```bash
g++ main.cpp -O3 -lSDL2
```
i have also made it somewhat easy to make different scenes or change the background colour or add animations by just adding models and stuff to the scene object
