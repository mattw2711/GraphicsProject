#include <GL/glew.h> // include GLEW and new version of GL on Windows
#include <GLFW/glfw3.h> // GLFW helper library
#include <glm/glm.hpp>
#include <stdio.h>
#include <fstream>
#include <string>
#include <iostream>
#include "Shader.h"
#include "VertexBuffer.h"

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
  std::cout << "Pressing Key" << std::endl;
    if (action == GLFW_PRESS) {
        switch (key) {
            case GLFW_KEY_LEFT:
                glClearColor(1.0,  0.0, 0.0, 1.0);
                break;
            case GLFW_KEY_RIGHT:
                glClearColor(0.0,  1.0, 0.0, 1.0);
                break;
            case GLFW_KEY_UP:
                glClearColor(1.0,  0.0, 1.0, 1.0);
                break;
            case GLFW_KEY_DOWN:
                glClearColor(0.0,  1.0, 1.0, 1.0);
                break;
        }
    }
}

int main() {
  // start GL context and O/S window using the GLFW helper library
  if (!glfwInit()) {
    fprintf(stderr, "ERROR: could not start GLFW3\n");
    return 1;
  } 

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow* window = glfwCreateWindow(640, 480, "Hello Triangle", NULL, NULL);
  if (!window) {
    fprintf(stderr, "ERROR: could not open window with GLFW3\n");
    glfwTerminate();
    return 1;
  }
  glfwMakeContextCurrent(window);
  glfwSetKeyCallback(window, keyCallback);
                                  
  // start GLEW extension handler
  glewExperimental = GL_TRUE;
  glewInit();

  // get version info
  const GLubyte* renderer = glGetString(GL_RENDERER); // get renderer string
  const GLubyte* version = glGetString(GL_VERSION); // version as a string
  printf("Renderer: %s\n", renderer);
  printf("OpenGL version supported %s\n", version);

  // tell GL to only draw onto a pixel if the shape is closer to the viewer
  glEnable(GL_DEPTH_TEST); // enable depth-testing
  glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"

  //Top
  glm::mat3x3 points1 = {
   0.5f,  0.5f,  0.0f,
   -0.5f, 0.5f,  0.0f,
   0.5f,  -0.5f,  0.0f
  };

  //Bottom
  glm::mat3x3 points2 = {
   -0.5f,  0.5f,  0.0f,
   -0.5f, -0.5f,  0.0f,
   0.5f,  -0.5f,  0.0f
  };

  glm::mat3x3 colours = {
  1.0f, 0.0f,  0.0f,
  0.0f, 1.0f,  0.0f,
  0.0f, 0.0f,  1.0f
  };

  VertexBuffer points_vbo1(9 * sizeof(float), &points1);
  VertexBuffer points_vbo2(9 * sizeof(float), &points2);
  VertexBuffer colours_vbo1(9 * sizeof(float), &colours);

  //TopColor
  GLuint vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);
  points_vbo1.Bind();
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
  colours_vbo1.Bind();
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);

  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);

  
  //BottomYellow
  GLuint vao1;
  glGenVertexArrays(1, &vao1);
  glBindVertexArray(vao1);
  points_vbo2.Bind();
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

  glEnableVertexAttribArray(0);

  Shader colorShader("res/shaders/vertex.shader", "res/shaders/fragment.shader");
  Shader yellowShader("res/shaders/vertex.shader", "res/shaders/fragmentY.shader");
  Shader blueShader("res/shaders/vertex.shader", "res/shaders/fragmentB.shader");

  Shader toggle = yellowShader;

  while(!glfwWindowShouldClose(window)) {
    // wipe the drawing surface clear
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    toggle.use();
    glBindVertexArray(vao1);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    colorShader.use();
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    // update other events like input handling 
    glfwPollEvents();
    // put the stuff we've been drawing onto the display
    glfwSwapBuffers(window);
    if(glfwGetKey(window, GLFW_KEY_ESCAPE)) {
    glfwSetWindowShouldClose(window, 1);
  }
  }
  
  // close GL context and any other GLFW resources
  glfwTerminate();
  return 0;
}