#include <glad/gl.h>

#include <GFX/system.hpp>
#include <GFX/graphics.hpp>

void handle_input(GLFWwindow* window, gfx::vector3f& eye, float dt);

int main(int argc, const char* argv[])
{
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

  gfx::program program;
  program.create(
      "/home/sasa/shaders/shader3d.vert",
      "/home/sasa/shaders/shader3d.frag"
      );
  program.link();

  int mvp_loc                      = program.getUniformLocation("MVP");
  gfx::vertex3d::attributes::vpos_location() = program.getAttribLocation("vPos");
  gfx::vertex3d::attributes::vcol_location() = program.getAttribLocation("vCol");


  gfx::vbuffer<gfx::vertex3d> vbuff;
  vbuff.bind();
  vbuff.append(gfx::vertex3d(-0.5, -0.5, 1, 1.f , 0.4, 0.f, 0.5f));
  vbuff.append(gfx::vertex3d( 0.5, -0.5, 1, 1.f , 0.4, 0.f, 0.5f));
  vbuff.append(gfx::vertex3d( 0.5,  0.5, 1, 1.f , 0.4, 0.f, 0.5f));

  vbuff.load_data();

  gfx::clock timer;

  gfx::view view(width, height);
  float fov = 1.57;

  gfx::vector3f eye(0, 0, 4);
  gfx::vector3f centre(0, 0,  1);
  gfx::vector3f up(0, 1,  0);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_COLOR, GL_ONE_MINUS_DST_COLOR);
  glBlendEquation(GL_FUNC_ADD);

  while(!context.should_close())
  {
    float dt = timer.restart();

    view.update(window);
    view.data(eye, centre, up);
    view.perspective(fov, 0.f, 10.f);
    view.multiply();


    program.use();

    view.set_mvp(mvp_loc);

    handle_input(window, eye, dt);

    context.clear(gfx::gruv::white, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      vbuff.draw();
    context.display();

  }
  return 0;
}

void handle_input(GLFWwindow* window, gfx::vector3f& eye, float dt)
{
  float speed = 2.5; 

  if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
  {
    eye.x -= speed * dt;
  }

  if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
  {
    eye.x += speed * dt;
  }

  if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
  {
    eye.z -= speed * dt;
  }

  if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
  {
    eye.z += speed * dt;
  }

}
