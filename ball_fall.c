// C Program to illustrate
// OpenGL animation for revolution

#include <GL/gl.h>
#include <GL/glut.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>

#define pi 3.14159

// global declaration
int x, y;
int bounce_count = 0;

float i, j;
float double_pi = 2 * pi;
float ball_y = 300; // Initial y-coordinate of the ball
float ball_speed = 0;
float ball_a = 0.98;
float bounce_dampening = 0.8; // Adjust this value to control bounce height

bool falling = true; // Flag to control the falling animation

// Initialization function
void scene_defaults(void) {
  // Reset background color with black (since all three argument is 0.0)
  glClearColor(0.0, 0.0, 0.0, 1.0);

  // Set picture color to green (in RGB model)
  // as only argument corresponding to G (Green) is 1.0 and rest are 0.0
  glColor3f(0.0, 1.0, 0.0);

  // Set width of point to one unit
  glPointSize(10.0);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  // Set window size in X- and Y- direction
  gluOrtho2D(-780, 780, -420, 420);
}

// Function to display animation

void display(void) {
  glClear(GL_COLOR_BUFFER_BIT);
  // ball
  glBegin(GL_POINTS);
  glColor3f(0.9, 0.2, 0.1);
  glVertex2f(0, ball_y); // Draw the ball at its current position
  glEnd();
  glBegin(GL_LINES);

  glColor3f(0.9, 0.2, 0.1);
  // columns
  glVertex2f(-50, 200);
  glVertex2f(-50, 0);

  glVertex2f(50, 200);
  glVertex2f(50, 0);

  // ground
  glVertex2f(-50, 0);
  glVertex2f(50, 0);

  glEnd();
  // Display bounce count
  glColor3f(1.0, 1.0, 1.0); // Set text color to white
  glRasterPos2f(-50, -100); // Set position to display the count
  char count_str[50];       // String to hold the count
  sprintf(count_str, "Bounces: %d", bounce_count);
  for (int k = 0; count_str[k] != '\0'; k++) {
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, count_str[k]);
  }
  glFlush();
}

void keyboard_callback(unsigned char key, int x, int y) {
  if (key == 'r' || key == 'R') {
    ball_y = 300;
    ball_speed = 0;
    falling = true;
  }
}

/*
 * This is the main loop guys.
 * This will be called every 30ms, which is ~30 fps.
 */
void update(int value) {
  if (falling) {
    float prev_ball_y = ball_y; // Store previous ball y-coordinate
    ball_speed -= ball_a;
    ball_y += ball_speed;

    if (ball_y <= 5) {
      ball_y = 5;
      ball_speed *= -bounce_dampening;
      bounce_count++;

      // Check for minimal change in position
      if (fabs(ball_y - prev_ball_y) < 0.5) {
        falling = false;
      }
    }
  }
  glutPostRedisplay();
  glutTimerFunc(30, update, 0);
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
  scene_defaults();
  glutDisplayFunc(display);
  glutTimerFunc(30, update, 0);        // Start the animation
  glutKeyboardFunc(keyboard_callback); // Register the keyboard function
  glutMainLoop();
}
