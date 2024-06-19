#include "plug.hpp"

#define EPSILON 0.0001

extern "C"
{
  float smoothstep(float x)
  {
    return x * x * (3 - 2 * x);
  }

  float func(float x)
  {
    if(x < EPSILON)
      return 0;
    else if(x > 1.f - EPSILON)
      return 1.f;

    return smoothstep(x);
  }

  float start_index = -2.0;
  float end_index   =  2.0;
}
