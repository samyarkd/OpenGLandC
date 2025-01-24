

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "glad.h"
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

GLuint texture1, texture2;

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
}

void key_callback(GLFWwindow *window, int key, int scancode, int action,
                  int mods) {
  if (key == GLFW_KEY_Q && action == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, GLFW_TRUE);
  }
}

char *read_file(const char *filepath) {
  FILE *file = fopen(filepath, "rb");
  if (!file) {
    fprintf(stderr, "Failed to open file: %s\n", filepath);
    return NULL;
  }

  fseek(file, 0, SEEK_END);
  long length = ftell(file);
  fseek(file, 0, SEEK_SET);

  char *content = (char *)malloc(length + 1);
  if (!content) {
    fprintf(stderr, "Memory allocation failed for file: %s\n", filepath);
    fclose(file);
    return NULL;
  }

  size_t read_length = fread(content, 1, length, file);
  if (read_length != length) {
    fprintf(stderr, "Failed to read file: %s\n", filepath);
    free(content);
    fclose(file);
    return NULL;
  }

  content[length] = '\0';
  fclose(file);
  return content;
}

GLFWwindow *setup_window() {
  if (!glfwInit()) {
    printf("Failed to initialize GLFW\n");
    return NULL;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow *window =
      glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
  if (window == NULL) {
    printf("Failed to create GLFW window\n");
    glfwTerminate();
    return NULL;
  }

  glfwMakeContextCurrent(window);

  // Initialize GLAD
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    printf("Failed to initialize GLAD\n");
    glfwDestroyWindow(window);
    glfwTerminate();
    return NULL;
  }

  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  // Register the key callback
  glfwSetKeyCallback(window, key_callback);

  return window;
}

GLuint compile_shader(GLenum type, const char *source) {
  GLuint shader = glCreateShader(type);
  glShaderSource(shader, 1, &source, NULL);
  glCompileShader(shader);

  // Check for compilation errors
  int success;
  char infoLog[512];
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(shader, 512, NULL, infoLog);
    fprintf(stderr, "ERROR::SHADER::COMPILATION_FAILED\n%s\n", infoLog);
  }

  return shader;
}

GLuint create_shader_program(const char *vertexSrc, const char *fragmentSrc) {
  GLuint vertexShader = compile_shader(GL_VERTEX_SHADER, vertexSrc);
  GLuint fragmentShader = compile_shader(GL_FRAGMENT_SHADER, fragmentSrc);
  GLuint shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glLinkProgram(shaderProgram);

  // Check for linking errors
  int success;
  char infoLog[512];
  glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
    fprintf(stderr, "ERROR::PROGRAM::LINKING_FAILED\n%s\n", infoLog);
  }

  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  return shaderProgram;
}

GLuint setup_vertex_data(float *vertices, size_t size) {
  GLuint VBO, VAO;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);

  // Position attribute
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  // Texture Coord attribute
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);
  return VAO;
}

GLuint load_texture(const char *path) {
  GLuint texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);

  // Set texture wrapping/filtering options
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  // Load and generate the texture
  int width, height, nrChannels;
  unsigned char *data = stbi_load(path, &width, &height, &nrChannels, 0);
  if (data) {
    GLenum format = GL_RGB;
    if (nrChannels == 1)
      format = GL_RED;
    else if (nrChannels == 3)
      format = GL_RGB;
    else if (nrChannels == 4)
      format = GL_RGBA;

    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format,
                 GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
  } else {
    fprintf(stderr, "Failed to load texture: %s\n", path);
  }
  stbi_image_free(data);
  return texture;
}

GLuint render_shape(GLuint VAO, GLuint shaderProgram, int vertexCount,
                    const char *imagePath) {
  glUseProgram(shaderProgram);

  if (imagePath) {
    glActiveTexture(GL_TEXTURE0);
  }

  GLfloat timeValue = glfwGetTime();
  GLint timeLocation = glGetUniformLocation(shaderProgram, "iTime");
  if (timeLocation != -1) {
    glUniform1f(timeLocation, timeValue);
  }

  glBindVertexArray(VAO);
  glDrawArrays(GL_TRIANGLES, 0, vertexCount);

  return 0;
}
int main(int argc, char **argv) {
  GLFWwindow *window = setup_window();
  if (window == NULL) {
    return -1;
  }

  // Define paths to shader files
  const char *vertexPath = "src/shaders/vert.glsl";
  const char *fragmentPath = "src/shaders/frag.glsl";
  texture1 = load_texture("src/assets/cosmic.jpeg");
  texture2 = load_texture("src/assets/cosmic.jpeg");

  // Load shader sources from files
  char *vertexSrc = read_file(vertexPath);
  char *fragmentSrc = read_file(fragmentPath);

  float vertices1[] = {
      // Triangle vertices
      -1.0f, -0.5f, 0.0f, // Bottom left
      0.5f,  -0.5f, 0.0f, // Bottom right
      0.0f,  0.5f,  0.0f  // Top
  };

  float vertices2[] = {// positions       // texture coords
                       -0.5f, -0.5f, 0.0f,  0.0f, 0.0f,  0.5f, -0.5f, 0.0f,
                       1.0f,  0.0f,  0.5f,  0.5f, 0.0f,  1.0f, 1.0f,  0.5f,
                       0.5f,  0.0f,  1.0f,  1.0f, -0.5f, 0.5f, 0.0f,  0.0f,
                       1.0f,  -0.5f, -0.5f, 0.0f, 0.0f,  0.0f};

  if (!vertexSrc || !fragmentSrc) {
    fprintf(stderr, "Failed to load shader files.\n");
    // Free any successfully loaded shaders
    free(vertexSrc);
    free(fragmentSrc);
    glfwTerminate();
    return -1;
  }

  GLuint shaderProgram1 = create_shader_program(vertexSrc, fragmentSrc);
  GLuint VAO1 = setup_vertex_data(vertices1, sizeof(vertices1));

  GLuint shaderProgram2 = create_shader_program(vertexSrc, fragmentSrc);
  GLuint VAO2 = setup_vertex_data(vertices2, sizeof(vertices2));

  glUseProgram(shaderProgram1);
  glUniform1i(glGetUniformLocation(shaderProgram1, "ourTexture"), 0);
  glUseProgram(shaderProgram2);
  glUniform1i(glGetUniformLocation(shaderProgram2, "ourTexture"), 0);

  // Free shader source memory after creating programs
  free(vertexSrc);
  free(fragmentSrc);

  while (!glfwWindowShouldClose(window)) {
    glClear(GL_COLOR_BUFFER_BIT);

    // Render each shape with existing textures
    render_shape(VAO1, shaderProgram1, 3, NULL); // Triangle

    render_shape(VAO2, shaderProgram2, 6, NULL); // Rectangle

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glDeleteVertexArrays(1, &VAO1);
  glDeleteVertexArrays(1, &VAO2);
  glDeleteProgram(shaderProgram1);
  glDeleteProgram(shaderProgram2);

  glfwTerminate();
  return 0;
}
