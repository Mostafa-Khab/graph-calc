#include <glad/gl.h>

#include <GFX/system.hpp>
#include <GFX/graphics.hpp>

#include <iostream>
#include <iomanip>
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
  int u_alpha_loc                            = default_prg.getUniformLocation("u_alpha");

  int default_resolution_loc       = default_prg.getUniformLocation("u_resolution");
  int u_size_loc                   = default_prg.getUniformLocation("u_size");

  LIB_HANDLE libplug = dlopen("libplug." SUFFIX/*, RTLD_NOW*/);
  CHECK_SHARED_LIB(libplug);

  float* start = reinterpret_cast<float*>(dlsym(libplug, "start")) ;
  CHECK_SHARED_LIB(start);
  float* finish = reinterpret_cast<float*>(dlsym(libplug, "finish")) ;
  CHECK_SHARED_LIB(finish);

  func_t* funcs = reinterpret_cast<func_t*>(dlsym(libplug, "funcs"));
  gfx::rgb* colors = reinterpret_cast<gfx::rgb*>(dlsym(libplug, "colors"));

  Integration* integrals = reinterpret_cast<Integration*>(dlsym(libplug, "integrals"));
  gfx::rgb* integrals_colors = reinterpret_cast<gfx::rgb*>(dlsym(libplug, "integrals_colors"));

  unsigned int funcs_num = 0;
  while(funcs[funcs_num ] != NULL)
  {
    ++funcs_num;
  }

  unsigned int integrals_num = 0;
  while(integrals[integrals_num].f != NULL)
  {
    ++integrals_num;
  }

  printf("%u functions are loaded", funcs_num); 

  gfx::vbuffer<gfx::vertex2d> buffs[funcs_num];
  gfx::vbuffer<gfx::vertex2d> integrals_buffs[integrals_num];

  for(int i = 0; i < funcs_num; ++i)
  {
    buffs[i].bind();
    for(float j = *start; j <= *finish + EPSILON; j += 0.05)
    { 
      buffs[i].append(gfx::vertex2d(j, funcs[i](j), colors[i]));
    }
    buffs[i].load_data();
  }

  for(int i = 0; i < integrals_num; ++i)
  {
    integrals_buffs[i].bind();
    for(float j = *integrals[i].a; j <= *integrals[i].b + EPSILON; j += 0.05)
    { 
      integrals_buffs[i].append(gfx::vertex2d(j, 0, colors[i]));
      integrals_buffs[i].append(gfx::vertex2d(j, integrals[i].f(j), colors[i]));
    }

    integrals_buffs[i].load_data();
  }

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
  glUniform1f(u_alpha_loc, 1.f);

  float s = 2.f;

  //s must be an even number to center the origin correctly.
  //for better experiece ratio 1 / 20 is the best.
  float max = (std::abs(*start) > std::abs(*finish))? std::abs(*start) : std::abs(*finish);
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
      std::cout << std::setprecision(5)
                << "x: " << std::setw(10) << std::left << mx << ' '
                << "y: " << std::setw(10) << std::left << my << '\n';

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

    if(handle_input(window, eye, s, dt))
      glUniform1f(u_size_loc, s);

    if(glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
    {
      if(libplug)
        dlclose(libplug);

      libplug = dlopen("libplug." SUFFIX); //NOTE: this is a macro in windows and POSIX(UNIX)
      CHECK_SHARED_LIB(libplug);

      funcs = reinterpret_cast<func_t*>(dlsym(libplug, "funcs"));
      colors =  reinterpret_cast<gfx::rgb*>(dlsym(libplug, "colors")); //get symbol using nm

      integrals = reinterpret_cast<func_t*>(dlsym(libplug, "integrals"));
      integrals_colors =  reinterpret_cast<gfx::rgb*>(dlsym(libplug, "integrals_colors"));

      start = reinterpret_cast<float*>(dlsym(libplug, "start")) ;
      finish   = reinterpret_cast<float*>(dlsym(libplug, "finish")) ;

      CHECK_SHARED_LIB(funcs);
      CHECK_SHARED_LIB(colors);
      CHECK_SHARED_LIB(integrals);
      CHECK_SHARED_LIB(integrals_colors);
      CHECK_SHARED_LIB(start);
      CHECK_SHARED_LIB(finish);

      funcs_num = 0;
      while(funcs[funcs_num ] != NULL)
      {
        ++funcs_num;
      }

      integrals_num = 0;
      while(integrals[integrals_num] != NULL)
      {
        ++integrals_num;
      }

      for(int i = 0; i < funcs_num; ++i)
      {

        buffs[i].clear();                                                 
        buffs[i].bind();                                                  
        for(float j = *start; j <= *finish + EPSILON; j += 0.025) 
        {                                                                 
          buffs[i].append(gfx::vertex2d(j, funcs[i](j), colors[i]));     
        }                                                                 
        buffs[i].load_data();
      }

      for(int i = 0; i < integrals_num; ++i)
      {

        integrals_buffs[i].clear();                                                 
        integrals_buffs[i].bind();                                                  
        for(float j = *start; j <= *finish + EPSILON; j += 0.025) 
        {                                                                 
          integrals_buffs[i].append(gfx::vertex2d(j, 0, colors[i]));     
          integrals_buffs[i].append(gfx::vertex2d(j, funcs[i](j), colors[i]));     
        }                                                                 
        integrals_buffs[i].load_data();
      }
    }

    default_prg.use();
    view.set_mvp(mvp_loc);

    context.clear(gfx::gruv::black, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      glLineWidth(1.f);
      grid.draw();

      glLineWidth(5.f);
      axis_buff.draw(GL_LINES);

      glLineWidth(4.f);

      glUniform1f(u_alpha_loc, 0.3);
      for(int i = 0; i < integrals_num; ++i)
        integrals_buffs[i].draw(GL_TRIANGLE_STRIP);
      glUniform1f(u_alpha_loc, 1.f);

      for(int i = 0; i < funcs_num; ++i)
        buffs[i].draw(GL_LINE_STRIP);

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
    pressed = true;
  } else if(glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS && fov < 3.f /*&& fov < 1.85*/)
  {
    fov += dt;
    pressed = true;
  }

  return pressed;
}
