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
gfx::rgb EXPORTING func3_color = gfx::gruv::green;

extern "C"
{

    float ease_out(float t)
    {
      const float n1 = 7.5625;
      const float d1 = 2.75;

      if(t < 1.f / d1)
      {
        return n1 * t * t;
      } else if(t < 2.f / d1)
      {
        return n1 * (t -= 1.5f / d1) * t +  0.75f;
      } else if(t < 2.5f / d1)
      {
        return n1 * (t -= 2.25f / d1) * t + 0.9375f;
      } 

      return n1 * (t -= 2.625f / d1) * t + 0.984375f;
    }

    float ease_in(float x)
    {
      return 1 - ease_out(1 - x);
    }

    float ease_inout(float x)
    {
      return (x < 0.5)? (1 - ease_out(1 - 2 * x)) / 2
                      : (1 + ease_out(2 * x - 1)) / 2;
    }

  float EXPORTING func(float x)
  {
    return x;
    //return ease_inout(x);
  }

  float EXPORTING func2(float x)
  {
    return ease_in(x);
  }

  float EXPORTING func3(float x)
  {
    return ease_out(x);
  }

  float EXPORTING start_index = -5.0;
  float EXPORTING end_index   =  5.0;
}
