/*********************************
 * author     : mostafa khaled
 * date       : Wed Jun 19 02:34:58 AM EEST 2024
 * desc       : adding set of functions to draw
 ********************************/
#ifndef PLUG_HPP
#define PLUG_HPP

  typedef float (*func_t)(float);

  struct Integration
  {
    func_t f;
    int a = 0, b = 0;
  };

#endif /* !PLUG_HPP */
