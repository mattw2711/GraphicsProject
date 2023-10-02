#include <GL/glew.h> // include GLEW and new version of GL on Windows
#include <GLFW/glfw3.h> // GLFW helper library
#include <glm/glm.hpp>
#include <stdio.h>
#include <fstream>
#include <string>
#include <iostream>

static std::string parseShader(const std::string& path) {
    std::ifstream file(path);

    std::string fileContents;
    std::string line;

    while (std::getline(file, line)) {
        fileContents += line + '\n';
    }
    return fileContents;
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

  glm::mat3x3 points1 = {
   0.5f,  0.5f,  0.0f,
   -0.5f, 0.5f,  0.0f,
   0.5f,  -0.5f,  0.0f
  };

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

  GLuint points_vbo1;
  glGenBuffers(1, &points_vbo1);
  glBindBuffer(GL_ARRAY_BUFFER, points_vbo1);
  glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(float), &points1, GL_STATIC_DRAW);

  GLuint points_vbo2;
  glGenBuffers(1, &points_vbo2);
  glBindBuffer(GL_ARRAY_BUFFER, points_vbo2);
  glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(float), &points2, GL_STATIC_DRAW);

  GLuint colours_vbo1 = 0;
  glGenBuffers(1, &colours_vbo1);
  glBindBuffer(GL_ARRAY_BUFFER, colours_vbo1);
  glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(float), &colours, GL_STATIC_DRAW);

  GLuint vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);
  glBindBuffer(GL_ARRAY_BUFFER, points_vbo1);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
  glBindBuffer(GL_ARRAY_BUFFER, colours_vbo1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);

  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);

  GLuint vao1;
  glGenVertexArrays(1, &vao1);
  glBindVertexArray(vao1);
  glBindBuffer(GL_ARRAY_BUFFER, points_vbo2);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
  glBindBuffer(GL_ARRAY_BUFFER, colours_vbo1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);

  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);

  std::string src = parseShader("res/shaders/vertex.shader");
  const char *vertex_shader = src.c_str();
  
  GLuint vs = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vs, 1, &vertex_shader, NULL);
  glCompileShader(vs);

  src = parseShader("res/shaders/fragment.shader");
  const char *fragment_shader = src.c_str();

  GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fs, 1, &fragment_shader, NULL);
  glCompileShader(fs);

  src = parseShader("res/shaders/fragmentY.shader");
  const char *fragmentY_shader = src.c_str();

  GLuint fsY = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fsY, 1, &fragmentY_shader, NULL);
  glCompileShader(fsY);

  GLuint shader_programme = glCreateProgram();
  glAttachShader(shader_programme, fs);
  glAttachShader(shader_programme, vs); 
  glBindAttribLocation(shader_programme, 0, "vertex_position");
  glBindAttribLocation(shader_programme, 1, "vertex_colour");
  glLinkProgram(shader_programme);

  GLuint shader_programme1 = glCreateProgram();
  glAttachShader(shader_programme1, fsY);
  glAttachShader(shader_programme1, vs); 
  glBindAttribLocation(shader_programme1, 0, "vertex_position");
  glBindAttribLocation(shader_programme1, 1, "vertex_colourYellow");
  glLinkProgram(shader_programme1);
  
  while(!glfwWindowShouldClose(window)) {
    // wipe the drawing surface clear
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(shader_programme);
    glBindVertexArray(vao);
    // draw points 0-3 from the currently bound VAO with current in-use shader
    glDrawArrays(GL_TRIANGLES, 0, 3);

    glUseProgram(shader_programme1);
    glBindVertexArray(vao1);
    // draw points 0-3 from the currently bound VAO with current in-use shader
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