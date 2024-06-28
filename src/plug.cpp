#include <GFX/system/color.hpp>

#include <cmath>

#include "plug.hpp"

#define EPSILON 0.0001

// we have a color for each function. if you want to change the color of a function.
// define a color as in this example. use your function name appended by _color suffix.
/*
 * for example, if your function is called func:
 * gfx::rgb func_color = gfx::gruv::blue;
 * 
 * for example, if your function is called X:
 * gfx::rgb X_color = gfx::gruv::blue;
 * 
 * NOTE: this is a must in our case. or the app will crash!!!
 */

#ifdef WIN32
  #define EXPORTING __declspec(dllexport) __stdcall
#else
  #define EXPORTING
#endif

gfx::rgb EXPORTING func_color  = gfx::gruv::aqua;
gfx::rgb EXPORTING func2_color = gfx::gruv::red;

extern "C"
{


  float EXPORTING func(float x)
  {
    return std::cos( -1.5 + x * 4.f) / 2.f * 1.f / 0.8 + 0.5;
  }

  float EXPORTING func2(float x)
  {
    return std::sin( -1.5 + x * 4.f) / 2.f * 1.f / 0.8 + 0.5;
  }

  float EXPORTING start_index = -5.0;
  float EXPORTING end_index   =  5.0;
}
