cmake_minimum_required(VERSION 3.25)
project(MY_MAIN_PROJ)

set(DEPS_PATH /home/sasa/glfw-3.3.9/deps/)
include_directories(${DEPS_PATH} /usr/include/freetype2)

if(UNIX)
  find_package(glfw3 3.3 REQUIRED)
  #find_package(OpenGL REQUIRED)
elseif(WIN32)
  include_directories(/usr/x86_64-w64-mingw32/include/ /usr/x86_64-w64-mingw32/local/include/) 
  link_directories(/usr/x86_64-w64-mingw32/local/lib/) 
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

set(SOURCES
    main.cpp
    ${DEPS_PATH}/glad_gl.c
)

add_executable(main ${SOURCES})

add_library(plug SHARED src/plug.cpp src/plug.hpp)
if(WIN32)
  target_link_libraries(plug PUBLIC gfx)
endif()

if(UNIX)
  target_link_options(main PUBLIC "-Wl,-rpath,/usr/local/lib/" "-Wl,-rpath,.")
  target_link_libraries(main PUBLIC
      gfx
      glfw
     #OpenGL::GL
     #Xrandr
     #Xi
     #X11
     #pthread
     #dl
     #Xinerama
     #Xcursor
      freetype
      logger
  )
elseif(WIN32)
  target_link_libraries(main PUBLIC
    gfx
   #glfw3dll
    glfw3
    opengl32
    gdi32
    freetype
    logger
  )
endif()
