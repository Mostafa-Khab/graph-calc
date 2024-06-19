# C++ OpenGL function editor
This is a program that draws a certain function using c++. It loads this function from libplug.so
the function (func in my program) can be loaded using a hot-reload technique if you pressed R key. \\

### to use this program
- you need my graphics project [gfx](https://github.com/Mostafa-Khab/gfx-project.git) which depends on opengl, glfw.
- after compiling both gfx-project repo and this one. you can draw whatever function you want. if you modified this function. recompile libplug.so, and press R simply.

### NOTE:
- this project only works for linux. hot-reloading isn't available using windows. (because I lack experience with WIN32 api also POSIX as well) but I was able to achieve hot-reload for linux this time.

![smooth step function](imgs/smoothstep.png)
