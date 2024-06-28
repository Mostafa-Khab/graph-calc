/*********************************
 * author     : mostafa khaled
 * date       : Wed Jun 19 02:34:58 AM EEST 2024
 * desc       : adding set of functions to draw
 ********************************/
#ifndef PLUG_HPP
#define PLUG_HPP

extern "C"
{

#define LIST_OF_FUNCS  \
  PLUG_FUNC(func);     \
  PLUG_FUNC(func2);    \

#ifdef WIN32
  #define PLUG_FUNC(X)            \
    float X(float);               \
    typedef float (__stdcall *X##_t)(float);
#else
  #define PLUG_FUNC(X)            \
    float X(float);               \
    typedef float (*X##_t)(float);
#endif

  LIST_OF_FUNCS;
#undef PLUG_FUNC

}

#endif /* !PLUG_HPP */
