
#define STB_IMAGE_IMPLEMENTATION
#include "glad.h"
#include "stb_image.h"
#include <GLFW/glfw3.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define SCR_WIDTH 800
#define SCR_HEIGHT 600

// Matrix utility functions
void mat4_identity(float *mat) {
  for (int i = 0; i < 16; i++)
    mat[i] = 0.0f;
  mat[0] = mat[5] = mat[10] = mat[15] = 1.0f;
}

void mat4_rotate(float *mat, float angle, float x, float y, float z) {
  float c = cosf(angle);
  float s = sinf(angle);
  float inv_c = 1.0f - c;

  mat[0] = x * x * inv_c + c;
  mat[1] = y * x * inv_c + z * s;
  mat[2] = x * z * inv_c - y * s;
  mat[3] = 0.0f;

  mat[4] = x * y * inv_c - z * s;
  mat[5] = y * y * inv_c + c;
  mat[6] = y * z * inv_c + x * s;
  mat[7] = 0.0f;

  mat[8] = x * z * inv_c + y * s;
  mat[9] = y * z * inv_c - x * s;
  mat[10] = z * z * inv_c + c;
  mat[11] = 0.0f;

  mat[12] = 0.0f;
  mat[13] = 0.0f;
  mat[14] = 0.0f;
  mat[15] = 1.0f;
}

void mat4_translate(float *mat, float x, float y, float z) {
  mat[12] += x;
  mat[13] += y;
  mat[14] += z;
}

void mat4_perspective(float *mat, float fov, float aspect, float near,
                      float far) {
  float tanHalfFOV = tanf(fov / 2.0f);
  mat4_identity(mat);
  mat[0] = 1.0f / (aspect * tanHalfFOV);
  mat[5] = 1.0f / tanHalfFOV;
  mat[10] = -(far + near) / (far - near);
  mat[11] = -1.0f;
  mat[14] = -(2.0f * far * near) / (far - near);
  mat[15] = 0.0f;
}

void mat4_multiply(float *result, float *a, float *b) {
  for (int row = 0; row < 4; row++) {
    for (int col = 0; col < 4; col++) {
      result[col + row * 4] =
          a[0 + row * 4] * b[col + 0 * 4] + a[1 + row * 4] * b[col + 1 * 4] +
          a[2 + row * 4] * b[col + 2 * 4] + a[3 + row * 4] * b[col + 3 * 4];
    }
  }
}

// Shader compilation utilities
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

// Read file utility
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

// Texture loading
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

// GLFW error callback
void glfw_error_callback(int error, const char *description) {
  fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

int main() {
  // Initialize GLFW
  glfwSetErrorCallback(glfw_error_callback);
  if (!glfwInit()) {
    fprintf(stderr, "Failed to initialize GLFW\n");
    return -1;
  }

  // Set GLFW context version to 3.3 and core profile
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // Create window
  GLFWwindow *window =
      glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Rotating Cube", NULL, NULL);
  if (window == NULL) {
    fprintf(stderr, "Failed to create GLFW window\n");
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);

  // Initialize GLAD
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    fprintf(stderr, "Failed to initialize GLAD\n");
    return -1;
  }

  // Set viewport
  glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

  // Enable depth testing
  glEnable(GL_DEPTH_TEST);

  // Define vertex and fragment shader sources
  const char *vertexShaderSource =
      "#version 330 core\n"
      "layout(location = 0) in vec3 position;\n"
      "layout(location = 1) in vec2 texCoord;\n"
      "out vec2 TexCoord;\n"
      "uniform mat4 model;\n"
      "uniform mat4 view;\n"
      "uniform mat4 projection;\n"
      "void main()\n"
      "{\n"
      "    gl_Position = projection * view * model * vec4(position, 1.0);\n"
      "    TexCoord = texCoord;\n"
      "}";

  const char *fragmentShaderSource =
      "#version 330 core\n"
      "out vec4 color;\n"
      "in vec2 TexCoord;\n"
      "uniform sampler2D ourTexture;\n"
      "uniform float iTime;\n"
      "void main()\n"
      "{\n"
      "    vec4 texColor = texture(ourTexture, TexCoord);\n"
      "    float red = sin(iTime);\n"
      "    float green = sin(iTime + 2.0);\n"
      "    float blue = sin(iTime + 4.0);\n"
      "    color = vec4(red, green, blue, 1.0) * texColor;\n"
      "}";

  // Create shader program
  GLuint shaderProgram =
      create_shader_program(vertexShaderSource, fragmentShaderSource);

  // Define cube vertices (positions and texture coordinates)
  float vertices[] = {
      // Front face
      -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.5f, 0.5f,
      0.5f, 1.0f, 1.0f, 0.5f, 0.5f, 0.5f, 1.0f, 1.0f, -0.5f, 0.5f, 0.5f, 0.0f,
      1.0f, -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,

      // Back face
      -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, -0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 0.5f,
      0.5f, -0.5f, 0.0f, 1.0f, 0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.5f, -0.5f,
      -0.5f, 0.0f, 0.0f, -0.5f, -0.5f, -0.5f, 1.0f, 0.0f,

      // Left face
      -0.5f, 0.5f, 0.5f, 1.0f, 0.0f, -0.5f, 0.5f, -0.5f, 1.0f, 1.0f, -0.5f,
      -0.5f, -0.5f, 0.0f, 1.0f, -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, -0.5f, -0.5f,
      0.5f, 0.0f, 0.0f, -0.5f, 0.5f, 0.5f, 1.0f, 0.0f,

      // Right face
      0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.5f, -0.5f, 0.5f, 1.0f, 1.0f, 0.5f, -0.5f,
      -0.5f, 0.0f, 1.0f, 0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.5f, 0.5f, -0.5f,
      0.0f, 0.0f, 0.5f, 0.5f, 0.5f, 1.0f, 0.0f,

      // Top face
      -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 0.5f, 0.5f,
      0.5f, 1.0f, 0.0f, 0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.5f, 0.5f, -0.5f, 1.0f,
      1.0f, -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,

      // Bottom face
      -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.5f,
      -0.5f, 0.5f, 0.0f, 0.0f, 0.5f, -0.5f, 0.5f, 0.0f, 0.0f, -0.5f, -0.5f,
      0.5f, 1.0f, 0.0f, -0.5f, -0.5f, -0.5f, 1.0f, 1.0f};

  // Create VAO and VBO
  GLuint VAO, VBO;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);

  // Bind VAO
  glBindVertexArray(VAO);

  // Bind and set VBO data
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  // Position attribute
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  // Texture Coord attribute
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  // Load texture
  GLuint texture = load_texture("src/assets/cosmic.jpeg");

  // Use shader program and set texture uniform
  glUseProgram(shaderProgram);
  glUniform1i(glGetUniformLocation(shaderProgram, "ourTexture"), 0);

  // Define transformation matrices
  float model[16], view[16], projection[16];
  mat4_identity(model);
  mat4_identity(view);
  mat4_identity(projection);

  // Set up view matrix (camera)
  mat4_translate(view, 0.0f, 0.0f, -3.0f);

  // Set up projection matrix
  mat4_perspective(projection, 45.0f * (M_PI / 180.0f),
                   (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

  // Pass view and projection matrices to the shader
  glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE,
                     view);
  glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1,
                     GL_FALSE, projection);

  // Render loop
  while (!glfwWindowShouldClose(window)) {
    // Calculate time
    float timeValue = glfwGetTime();

    // Clear color and depth buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Activate texture unit and bind texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    // Calculate rotation angles
    mat4_identity(model);
    mat4_rotate(model, timeValue * 1.0f, 1.0f, 0.0f,
                0.0f); // Rotate around X-axis
    float temp[16];
    mat4_rotate(temp, timeValue * 0.5f, 0.0f, 1.0f,
                0.0f); // Rotate around Y-axis
    mat4_multiply(model, temp, model);

    // Pass model matrix to shader
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1,
                       GL_FALSE, model);

    // Render the cube
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // Swap buffers and poll events
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  // Cleanup
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteProgram(shaderProgram);
  glDeleteTextures(1, &texture);

  glfwTerminate();
  return 0;
}
