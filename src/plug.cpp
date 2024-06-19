#include <cmath>

#include "plug.hpp"

#define EPSILON 0.0001

extern "C"
{
  float smoothstep(float x)
  {
    return x * x * (3 - 2 * x);
  }

  float quart_pump_out(float x)
  {
    if (x <= 0.8f)
      return 2.4414f * x * x * x * x;

    return 1.25f - 25.f * (0.9 - x) * (0.9 - x);
  }

  float func(float x)
  {
    //if(x < EPSILON)
    //  return 0;
    //else if(x > 1.f - EPSILON)
    //  return 1.f;

    return quart_pump_out(x);
  }


  float start_index = -5.0;
  float end_index   =  5.0;
}
