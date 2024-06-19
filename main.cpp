#include <glad/gl.h>

#include <GFX/system.hpp>
#include <GFX/graphics.hpp>

#include <iostream>

#include <dlfcn.h>

#include "src/plug.hpp"

bool handle_input(GLFWwindow* window, gfx::vector3f& eye, float& fov, float dt);

#define print(X) \
  std::cout << #X << " : " << X << '\n';

/*
 * what leart from this project. 
 * attributes' locations can be the same across different programs.
 * we don't need to override attributes for the the same vertex type if they have similar shaders in the input.
 * gfx::vertex2d can be used in different programs with reassigning attributes of it.
 */

#define CHECK_SHARED_LIB(X) \
 if(!X) { \
    Log::error("failed to load: " + std::string(#X)); \
    Log::error(dlerror()); \
    return 5; \
 }

#define EPSILON 0.00001f

int main(int argc, const char* argv[])
{
  Log::level(Log::Level::all);

  const int width  = 700;
  const int height = 700;

  gfx::context context;
  context.setVersion({2, 0});
  context.setWindowData(width, height, "gfx-test");

  bool result = context.init();
  GLFW_ASSERT(result, "failed to create gfx context");

  GLFWwindow* window;
  context.getWindow(window);

  glfwSetKeyCallback(window, gfx::callback::key);

  gfx::program default_prg;
  default_prg.create(
      "../shaders/default.vert",
      "../shaders/default.frag"
      );
  default_prg.link();

  gfx::program bg_prg;
  bg_prg.create(
      "../shaders/background.vert",
      "../shaders/background.frag"
      );
  bg_prg.link();

  int mvp_loc                                = default_prg.getUniformLocation("MVP");
  gfx::vertex2d::attributes::vpos_location() = default_prg.getAttribLocation("vPos");
  gfx::vertex2d::attributes::vcol_location() = default_prg.getAttribLocation("vCol");

  int bg_resolution_loc       = bg_prg.getUniformLocation("u_resolution");

  void* libplug = dlopen("libplug.so", RTLD_NOW);
  CHECK_SHARED_LIB(libplug);
  func_t func        = reinterpret_cast<func_t>(dlsym(libplug, "func"));
  CHECK_SHARED_LIB(func);
  float* start_index = reinterpret_cast<float*>(dlsym(libplug, "start_index")) ;
  CHECK_SHARED_LIB(start_index);
  float* end_index = reinterpret_cast<float*>(dlsym(libplug, "end_index")) ;
  CHECK_SHARED_LIB(end_index);

  gfx::vbuffer<gfx::vertex2d> vbuff;
  vbuff.bind();
  for(float i = *start_index; i <= *end_index + EPSILON; i += 0.05)
  {
    vbuff.append(gfx::vertex2d(i, func(i), gfx::gruv::red));
  }
  vbuff.load_data();

  gfx::vbuffer<gfx::vertex2d> bg_buff;
  bg_buff.bind();
  bg_buff.append(gfx::vertex2d(-8.0, -8.0, gfx::gruv::black));
  bg_buff.append(gfx::vertex2d( 8.0, -8.0, gfx::gruv::black));
  bg_buff.append(gfx::vertex2d(-8.0,  8.0, gfx::gruv::black));
  bg_buff.append(gfx::vertex2d( 8.0,  8.0, gfx::gruv::black));
  bg_buff.load_data();

  gfx::clock timer;

  gfx::view view(width, height);
  float fov = 1.57;

  gfx::vector3f eye(0, 0, 2);
  gfx::vector3f centre(0, 0,  1);
  gfx::vector3f up(0, 1,  0);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_DST_ALPHA);
  glBlendEquation(GL_FUNC_ADD);

  auto resolution = gfx::vector2f(width, height);
  bg_prg.use();
  glUniform2fv(bg_resolution_loc, 1, reinterpret_cast<const float*>(&resolution));

  while(!context.should_close())
  {
    float dt = timer.restart();

    view.update(window);
    view.data(eye, centre, up);
    view.perspective(fov, 0.f, 10.f);
    view.multiply();

    if(handle_input(window, eye, fov, dt))
    {
      centre.x = eye.x;
      centre.y = eye.y;
    } 

    print(fov);

    if(glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
    {
      if(libplug)
        dlclose(libplug);

      libplug = dlopen("libplug.so", RTLD_NOW);
      CHECK_SHARED_LIB(libplug);
      func    = reinterpret_cast<func_t>(dlsym(libplug, "func"));
      CHECK_SHARED_LIB(func);
      float* start_index = reinterpret_cast<float*>(dlsym(libplug, "start_index")) ;
      CHECK_SHARED_LIB(start_index);
      float* end_index = reinterpret_cast<float*>(dlsym(libplug, "end_index")) ;
      CHECK_SHARED_LIB(end_index);

      vbuff.clear();
      vbuff.bind();
      for(float i = *start_index; i <= *end_index + EPSILON; i += 0.05)
      {
        vbuff.append(gfx::vertex2d(i, func(i), gfx::gruv::red));
      }
      vbuff.load_data();

    }

    context.clear(gfx::gruv::white, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    {
      view.set_mvp(mvp_loc);
      bg_prg.use();
      bg_buff.draw(GL_TRIANGLE_STRIP);
      bg_buff.unbind();
    }

    {
      view.set_mvp(mvp_loc);
      default_prg.use();
      glLineWidth(8.f);
      vbuff.draw(GL_LINE_STRIP);
      vbuff.unbind();
    }


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

  if(glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS && fov > 1.22)
  {
    fov -= dt;
  } else if(glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS && fov < 1.85)
  {
    fov += dt;
  }

  return pressed;
}
