// C Program to illustrate
// OpenGL animation for revolution

#include <GL/glut.h>
#include <math.h>
#include <stdbool.h>

#define pi 3.14159

// global declaration
int x, y;
float i, j;
float double_pi = 2 * pi;

// Initialization function
void myInit(void) {
  // Reset background color with black (since all three argument is 0.0)
  glClearColor(0.0, 0.0, 0.0, 1.0);

  // Set picture color to green (in RGB model)
  // as only argument corresponding to G (Green) is 1.0 and rest are 0.0
  glColor3f(0.0, 1.0, 0.0);

  // Set width of point to one unit
  glPointSize(1.0);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  // Set window size in X- and Y- direction
  gluOrtho2D(-780, 780, -420, 420);
}

// Function to display animation
void display(void) {
  for (j = 0; true; j += 0.01) {
    glClear(GL_COLOR_BUFFER_BIT);
    glBegin(GL_POINTS);

    glColor3f(0.9, 0.2, 0.1);
    // Iterate i up to 2*pi, i.e., 360 degree
    // plot point with slight increment in angle,
    // so, it will look like a continuous figure

    // Loop is to draw outer circle
    for (i = 0; i < double_pi; i += 0.0001) {
      x = 200 * cos(i);
      y = 200 * sin(i);
      glVertex2i(x, y);

      // For every loop, 2nd glVertex function is
      // to make smaller figure in motion
      glVertex2i((float)x / 2 - 600 * cos(j), (float)y / 2 - 100 * sin(j));
    }

    // 7 loops to draw parallel latitude
    for (i = 1.17; i < 1.97; i += 0.001) {
      x = 400 * cos(i);
      y = -150 + 300 * sin(i);
      glVertex2i(x, y);
      glVertex2i((float)x / 2 - 600 * cos(j), (float)y / 2 - 100 * sin(j));
    }

    for (i = 1.07; i < 2.07; i += 0.001) {
      x = 400 * cos(i);
      y = -200 + 300 * sin(i);
      glVertex2i(x, y);
      glVertex2i((float)x / 2 - 600 * cos(j), (float)y / 2 - 100 * sin(j));
    }

    for (i = 1.05; i < 2.09; i += 0.001) {
      x = 400 * cos(i);
      y = -250 + 300 * sin(i);
      glVertex2i(x, y);
      glVertex2i((float)x / 2 - 600 * cos(j), (float)y / 2 - 100 * sin(j));
    }

    for (i = 1.06; i < 2.08; i += 0.001) {
      x = 400 * cos(i);
      y = -300 + 300 * sin(i);
      glVertex2i(x, y);
      glVertex2i((float)x / 2 - 600 * cos(j), (float)y / 2 - 100 * sin(j));
    }

    for (i = 1.10; i < 2.04; i += 0.001) {
      x = 400 * cos(i);
      y = -350 + 300 * sin(i);
      glVertex2i(x, y);
      glVertex2i((float)x / 2 - 600 * cos(j), (float)y / 2 - 100 * sin(j));
    }

    for (i = 1.16; i < 1.98; i += 0.001) {
      x = 400 * cos(i);
      y = -400 + 300 * sin(i);
      glVertex2i(x, y);
      glVertex2i((float)x / 2 - 600 * cos(j), (float)y / 2 - 100 * sin(j));
    }

    for (i = 1.27; i < 1.87; i += 0.001) {
      x = 400 * cos(i);
      y = -450 + 300 * sin(i);
      glVertex2i(x, y);
      glVertex2i((float)x / 2 - 600 * cos(j), (float)y / 2 - 100 * sin(j));
    }

    // Loop is to draw vertical line
    for (i = 200; i >= -200; i--) {
      glVertex2i(0, i);
      glVertex2i(-600 * cos(j), i / 2 - 100 * sin(j));
    }

    // 3 loops to draw vertical ellipse (similar to longitude)
    for (i = 0; i < 6.29; i += 0.001) {
      x = 70 * cos(i);
      y = 200 * sin(i);
      glVertex2i(x, y);
      glVertex2i((float)x / 2 - 600 * cos(j), (float)y / 2 - 100 * sin(j));
    }

    for (i = 0; i < 6.29; i += 0.001) {
      x = 120 * cos(i);
      y = 200 * sin(i);
      glVertex2i(x, y);
      glVertex2i((float)x / 2 - 600 * cos(j), (float)y / 2 - 100 * sin(j));
    }

    for (i = 0; i < 6.29; i += 0.001) {
      x = 160 * cos(i);
      y = 200 * sin(i);
      glVertex2i(x, y);
      glVertex2i((float)x / 2 - 600 * cos(j), (float)y / 2 - 100 * sin(j));
    }

    // Loop to make orbit of revolution
    for (i = 0; i < 6.29; i += 0.001) {
      x = 600 * cos(i);
      y = 100 * sin(i);
      glVertex2i(x, y);
    }
    glEnd();
    glFlush();
  }
}

// Driver Program
int main(int argc, char **argv) {
  glutInit(&argc, argv);

  // Display mode which is of RGB (Red Green Blue) type
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);

  // Declares window size
  glutInitWindowSize(1360, 768);

  // Declares window position which is (0, 0)
  // means lower left corner will indicate position (0, 0)
  glutInitWindowPosition(0, 0);

  // Name to window
  glutCreateWindow("Revolution");

  // Call to myInit()
  myInit();
  glutDisplayFunc(display);
  glutMainLoop();
}
