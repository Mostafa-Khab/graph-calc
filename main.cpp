#include <glad/gl.h>

#include <GFX/system.hpp>
#include <GFX/graphics.hpp>

#include <iostream>
#include <cmath>

#ifdef WIN32
  #include <windows.h>
#else
  #include <dlfcn.h>
#endif

#include "src/plug.hpp"

/*
 * what leart from this project. 
 * attributes' locations can be the same across different shader programs.
 * we don't need to override attributes for the the same vertex type if they have similar shaders in the input.
 * gfx::vertex2d can be used in different programs without reassigning attributes of it.
 * 
 * I used X_macro style for the first time.
 */

bool handle_input(GLFWwindow* window, gfx::vector3f& eye, float& fov, float dt);

#ifdef WIN32
  #define dlerror() "some dll shit failed to load"
#endif

#ifdef WIN32
  #define LIB_HANDLE HINSTANCE
  #define dlopen LoadLibrary
  #define dlsym GetProcAddress
  #define dlclose FreeLibrary
  #define SUFFIX "dll"
#else
  #define LIB_HANDLE void*
  #define dlopen(X) dlopen((X), RTLD_NOW)
  #define SUFFIX "so"
#endif

//THIS IS NOT A GOOD PRACTICE!!
#define print(X) \
  std::cout << #X << " : " << X << '\n';

#define CHECK_SHARED_LIB(X) \
 if(!X) { \
    Log::error("failed to load: " + std::string(#X)); \
    Log::error(dlerror()); \
    return 5; \
 }

#define CHECK_SHARED_LIB_COLOR(X) \
 if(!X) { \
    Log::error("make sure to define " + std::string(#X) + " in src/plug.cpp"); \
    Log::error("failed to load: " + std::string(#X)); \
    Log::error(dlerror()); \
    return 5; \
 }

#define EPSILON 0.00001f

class Grid
{
  public:
    Grid(gfx::rgb lcolor = gfx::gruv::white, gfx::rgb bgcolor = gfx::gruv::black)
      :line_color(lcolor), bg_color(bgcolor)
    {
    }

   ~Grid() = default;

   void fill(float max, float s)
   {
     if(s < EPSILON)
       Log::error("division by zero isn't valid: " + std::string(__FILE__));

     for(float i = -max; i < max + EPSILON; i += 2.f * max / s)
     {
        buff.append(gfx::vertex2d(i, max, line_color));
        buff.append(gfx::vertex2d(i,-max, line_color));
     }

     for(float i = -max; i < max + EPSILON; i += 2.f * max / s)
     {
        buff.append(gfx::vertex2d( max, i   , line_color));
        buff.append(gfx::vertex2d(-max, i   , line_color));
     }
     buff.bind();
     buff.load_data();


     bg_buff.bind();
     bg_buff.append(gfx::vertex2d(-max, -max, bg_color));
     bg_buff.append(gfx::vertex2d( max, -max, bg_color));
     bg_buff.append(gfx::vertex2d(-max,  max, bg_color));
     bg_buff.append(gfx::vertex2d( max,  max, bg_color));
     bg_buff.load_data();
   }

   void draw()
   {
      bg_buff.draw(GL_TRIANGLE_STRIP);
      buff.draw(GL_LINES);
   }

  private:
    gfx::vbuffer<gfx::vertex2d> buff, bg_buff;
    gfx::rgb line_color, bg_color;
};

int main(int argc, const char* argv[])
{
  Log::level(Log::Level::all);

  const int width  = 700;
  const int height = 700;

  gfx::context context;
  context.setVersion({2, 0});
  context.setWindowData(width, height, "gfx-test", false);

  bool result = context.init();
  GLFW_ASSERT(result, "failed to create gfx context");

  GLFWwindow* window;
  context.getWindow(window);

  glfwSetKeyCallback(window, gfx::callback::key);
  glfwSetMouseButtonCallback(window, gfx::callback::button);

  gfx::program default_prg;
  default_prg.create(
      "../shaders/default.vert",
      "../shaders/default.frag"
      );
  default_prg.link();

  int mvp_loc                                = default_prg.getUniformLocation("MVP");
  gfx::vertex2d::attributes::vpos_location() = default_prg.getAttribLocation("vPos");
  gfx::vertex2d::attributes::vcol_location() = default_prg.getAttribLocation("vCol");

  int default_resolution_loc       = default_prg.getUniformLocation("u_resolution");

  LIB_HANDLE libplug = dlopen("libplug." SUFFIX/*, RTLD_NOW*/);
  CHECK_SHARED_LIB(libplug);

#define PLUG_FUNC(X) \
  X##_t X = reinterpret_cast<X##_t>(dlsym(libplug, #X)); \
  CHECK_SHARED_LIB(X);

  LIST_OF_FUNCS;
#undef PLUG_FUNC

  float* start_index = reinterpret_cast<float*>(dlsym(libplug, "start_index")) ;
  CHECK_SHARED_LIB(start_index);
  float* end_index = reinterpret_cast<float*>(dlsym(libplug, "end_index")) ;
  CHECK_SHARED_LIB(end_index);

#define PLUG_FUNC(X) \
  gfx::rgb* X##_color =  reinterpret_cast<gfx::rgb*>(dlsym(libplug, #X"_color")); \
  CHECK_SHARED_LIB_COLOR(X##_color);

  LIST_OF_FUNCS;
#undef PLUG_FUNC

#define PLUG_FUNC(X) \
  gfx::vbuffer<gfx::vertex2d> X##_buff; \
  X##_buff.bind();                      \
  for(float i = *start_index; i <= *end_index + EPSILON; i += 0.05) \
  { \
    X##_buff.append(gfx::vertex2d(i, X(i), *X##_color)); \
  } \
  X##_buff.load_data();

  LIST_OF_FUNCS;
#undef PLUG_FUNC

  gfx::clock timer;

  gfx::view view(width, height);

  gfx::vector3f eye(0, 0, 2);
  gfx::vector3f centre(0, 0,  1);
  gfx::vector3f up(0, 1,  0);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glBlendEquation(GL_FUNC_ADD);

  glEnable(GL_PROGRAM_POINT_SIZE);

  default_prg.use();
  //glUniform2fv(default_resolution_loc, 1, reinterpret_cast<const float*>(&resolution));
  glUniform2f(default_resolution_loc, (float)width, (float)height);

  float s = 2.f;

  //s must be an even number to center the origin correctly.
  //for better experiece ratio 1 / 20 is the best.
  float max = (std::abs(*start_index) > std::abs(*end_index))? std::abs(*start_index) : std::abs(*end_index);
  Grid grid;
  grid.fill(max, max * 10.f );

  gfx::vbuffer<gfx::vertex2d> axis_buff;
  axis_buff.append(gfx::vertex2d( 0, max, gfx::gruv::white));
  axis_buff.append(gfx::vertex2d( 0,-max, gfx::gruv::white));
  axis_buff.append(gfx::vertex2d( max, 0, gfx::gruv::white));
  axis_buff.append(gfx::vertex2d(-max, 0, gfx::gruv::white));
  axis_buff.bind();
  axis_buff.load_data();

  gfx::vbuffer<gfx::vertex2d> points;

  bool updated = false;
  float pop_cooldown = 0.f;
  while(!context.should_close())
  {
    float dt = timer.restart();
    gfx::mouse::update(window);

    if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS && !updated)
    {
      auto mx = gfx::mouse::x * s  + eye.x;
      auto my = gfx::mouse::y * s  + eye.y;
      std::cout << "x: " << mx << ' '
                << "y: " << my << '\n';

      points.append(gfx::vertex2d(mx, my, gfx::gruv::yellow));
      updated = true;
    }

    if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE && updated)
    {
      points.bind();
      points.load_data();
      updated = false;
    }

    pop_cooldown += dt;
    if(glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS && pop_cooldown > 0.25)
    {
      if(points.size() > 0)
      {
        points.pop_back();
        points.bind();
        points.load_data();
        pop_cooldown = 0;
      }
    }

    view.update(window);
    mat4x4_ortho(view.p(),
        -s + eye.x,
         s + eye.x,
        -s + eye.y, 
         s + eye.y,
        -1, 1);
    view.multiply();

    handle_input(window, eye, s, dt);

    if(glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
    {
      if(libplug)
        dlclose(libplug);

      libplug = dlopen("libplug." SUFFIX); //NOTE: this is a macro in windows and POSIX(UNIX)
      CHECK_SHARED_LIB(libplug);
#define PLUG_FUNC(X) \
      X = reinterpret_cast<X##_t>(dlsym(libplug, #X)); \
      CHECK_SHARED_LIB(X);

      LIST_OF_FUNCS;
#undef  PLUG_FUNC
      //func               = reinterpret_cast<func_t>(dlsym(libplug, "func"));
      //CHECK_SHARED_LIB(func);

      float* start_index = reinterpret_cast<float*>(dlsym(libplug, "start_index")) ;
      CHECK_SHARED_LIB(start_index);
      float* end_index   = reinterpret_cast<float*>(dlsym(libplug, "end_index")) ;
      CHECK_SHARED_LIB(end_index);

#define PLUG_FUNC(X)                                                \
  X##_buff.clear();                                                 \
  X##_buff.bind();                                                  \
  for(float i = *start_index; i <= *end_index + EPSILON; i += 0.025) \
  {                                                                 \
    X##_buff.append(gfx::vertex2d(i, X(i), *X##_color));     \
  }                                                                 \
  X##_buff.load_data();

  LIST_OF_FUNCS;
#undef PLUG_FUNC

    }

    default_prg.use();
    view.set_mvp(mvp_loc);

    context.clear(gfx::gruv::black, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      glLineWidth(1.f);
      grid.draw();

      glLineWidth(5.f);
      axis_buff.draw(GL_LINES);

      glLineWidth(4.f);
#define PLUG_FUNC(X)                        \
      X##_buff.draw(GL_LINE_STRIP);

      LIST_OF_FUNCS;
#undef PLUG_FUNC
      points.draw(GL_POINTS);
    context.display();

  }
  if(libplug)
    dlclose(libplug);
  return 0;
}

bool handle_input(GLFWwindow* window, gfx::vector3f& eye, float& fov, float dt)
{
  float speed = 1.5; 
  bool pressed = false;

  if(glfwGetKey(window, GLFW_KEY_EQUAL))
  {
    eye.x = 0.f;
    eye.y = 0.f;
    return true;
  }

  if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
  {
    eye.x -= speed * dt;
    pressed = true;
  }

  if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
  {
    eye.x += speed * dt;
    pressed = true;
  }

  if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
  {
    eye.y += speed * dt;
    pressed = true;
  }

  if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
  {
    eye.y -= speed * dt;
    pressed = true;
  }

  if(glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS && fov > 0.2  /*&& fov > 1.22*/)
  {
    fov -= dt;
  } else if(glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS && fov < 3.f /*&& fov < 1.85*/)
  {
    fov += dt;
  }

  return pressed;
}
