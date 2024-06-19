/*********************************
 * author     : mostafa khaled
 * date       : Wed Jun 19 02:34:58 AM EEST 2024
 * desc       : 
 ********************************/
#ifndef PLUG_HPP
#define PLUG_HPP

extern "C"
{
  float func(float x);

  typedef float (*func_t)(float);
}

#endif /* !PLUG_HPP */
