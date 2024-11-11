#include <GFX/system/color.hpp>
#include <cmath>

typedef float (*func_t)(float);


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

gfx::rgb colors[] = {
  gfx::gruv::yellow,
};

gfx::rgb integrals_colors[] = {
  gfx::gruv::red,
};

extern "C"
{

  struct Integration
  {
    func_t f = NULL;
    int a = 0, b = 0;
  };

  float f(float x)
  {
    return x * x * x;
  }

  func_t funcs[] = 
  {
    f,
    NULL,                        //should exits to know the count of the functions
  };

  Integration integrals[] = 
  {
    {f, 0, 10},
    {NULL, 0, 0},                //should exits to know the count of the integrals functions
  };

  float start  = -5;
  float finish =  5;
}
