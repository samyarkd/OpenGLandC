// C program to demonstrate
// drawing a circle using
// OpenGL
#include <GL/glut.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>

#define pi 3.142857

int refreshMills = 15;

// function to initialize
void myInit(void) {
  // making background color black as first
  // 3 arguments all are 0.0
  glClearColor(0.0, 0.0, 0.0, 1.0);

  // making picture color green (in RGB mode), as middle argument is 1.0
  glColor3f(1.0, 1.0, 0.0);

  // breadth of picture boundary is 1 pixel
  glPointSize(5.0);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  // setting window dimension in X- and Y- direction
  gluOrtho2D(-780, 780, -420, 420);
}

float v_alt = 0;
float alignment = 1;

void display(void) {
  glClear(GL_COLOR_BUFFER_BIT);
  glBegin(GL_POINTS);
  float x, y, xalt, yalt, i;

  // iterate y up to 2*pi, i.e., 360 degree
  // with small increment in angle as
  // glVertex2i just draws a point on specified co-ordinate
  for (i = 0; i < (2 * pi); i += 0.1) {
    // let 200 is radius of circle and as,
    // circle is defined as x=r*cos(i) and y=r*sin(i)
    x = 200 * cos(i);
    y = 200 * sin(i);

    xalt = 130 * v_alt * cos(i);
    yalt = 198 * v_alt * sin(i);

    v_alt = v_alt + 0.00005 * alignment;

    glVertex2i(x, y);
    glVertex2i(xalt, yalt);

    if (v_alt >= 1) {
      alignment = -1;
    }

    if (v_alt <= -1) {
      alignment = 1;
    }
  }

  glEnd();
  glFlush();
}

void timer() {
  // repaint the display
  glutPostRedisplay();
  // call a callback function in ms
  glutTimerFunc(refreshMills, timer, 0);
}

int main(int argc, char **argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);

  // giving window size in X- and Y- direction
  glutInitWindowSize(1366, 768);
  glutInitWindowPosition(0, 0);

  // Giving name to window
  glutCreateWindow("Drawing Circles");
  myInit();

  // glutFullScreen();
  glutDisplayFunc(display);

  // refresh the display each refreshMills
  timer();

  glutMainLoop();
}
