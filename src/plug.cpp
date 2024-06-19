#include <GFX/system/color.hpp>

#include <cmath>

#include "plug.hpp"

#define EPSILON 0.00001

// we have a color for each function. if you want to change the color of a function.
// define a color as in this example. use your function name appended by _color suffix.
/*
 * for example, if your function is call func:
 * gfx::rgb func_color = gfx::gruv::blue;
 * 
 * for example, if your function is call X:
 * gfx::rgb X_color = gfx::gruv::blue;
 * 
 * NOTE: this is a must in our case. or the app will crash!!!
 */

gfx::rgb func_color  = gfx::gruv::red;
gfx::rgb func2_color = gfx::gruv::aqua;

extern "C"
{


  float func(float x)
  {
    return std::sin( -1.5 + x * 4.f) / 2.f * 1.f / 0.8 + 0.5;
  }

  float func2(float x)
  {
    return std::tan(x);
  }

  float start_index = -5.0;
  float end_index   =  5.0;
}
