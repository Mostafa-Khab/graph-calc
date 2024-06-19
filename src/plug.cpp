#include <cmath>

#include "plug.hpp"

#define EPSILON 0.0001

extern "C"
{

  float func(float x)
  {
    return std::sin( -1.5 + x * 4.f) / 2.f * 1.f / 0.8 + 0.5;
  }

  float start_index = -1.0;
  float end_index   =  1.0;
}
