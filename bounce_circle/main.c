#include <GL/gl.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <math.h>

#define WINDOW_SIZE 800
#define OUTER_RADIUS 350
#define BALL_RADIUS 20
#define GRAVITY 510.0f
#define DAMPING 1.0f
#define NUM_SOUNDS 8
#define INIT_VELOCITY 700.0f

typedef struct {
  float x, y;
  float vx, vy;
} Ball;

Mix_Chunk *sounds[NUM_SOUNDS];
const char *sound_files[NUM_SOUNDS] = {"a.wav",  "b.wav",  "c.wav", "c2.wav",
                                       "d1.wav", "e1.wav", "f.wav", "g.wav"};

void draw_circle(float cx, float cy, float r, int segments) {
  glBegin(GL_TRIANGLE_FAN);
  glVertex2f(cx, cy);
  for (int i = 0; i <= segments; i++) {
    float angle = i * (2 * M_PI) / segments;
    glVertex2f(cx + cos(angle) * r, cy + sin(angle) * r);
  }
  glEnd();
}

void load_sounds() {
  char path_buffer[256]; // Buffer for full path
  const char *base_path = "./xylhophone/xylophone-";

  for (int i = 0; i < NUM_SOUNDS; i++) {
    // Use snprintf for safe path construction
    snprintf(path_buffer, sizeof(path_buffer), "%s%s", base_path,
             sound_files[i]);

    // __AUTO_GENERATED_PRINT_VAR_START__
    printf("Loading sound: %s\n",
           path_buffer); // __AUTO_GENERATED_PRINT_VAR_END__

    sounds[i] = Mix_LoadWAV(path_buffer);
    if (!sounds[i]) {
      printf("Failed to load %s: %s\n", path_buffer, Mix_GetError());
    }
  }
}

int get_sound_index(float angle) {
  float sector = 360.0f / NUM_SOUNDS;
  float adjusted = fmod(angle + 360.0f + sector / 2, 360.0f);
  return (int)(adjusted / sector) % NUM_SOUNDS;
}

void physics_update(Ball *ball, float dt) {
  // Apply gravity
  ball->vy += GRAVITY * dt;

  // Update position
  ball->x += ball->vx * dt;
  ball->y += ball->vy * dt;

  // Circular boundary collision
  float dx = ball->x - WINDOW_SIZE / 2;
  float dy = ball->y - WINDOW_SIZE / 2;
  float dist = sqrt(dx * dx + dy * dy);
  float max_dist = OUTER_RADIUS - BALL_RADIUS;

  if (dist > max_dist) {
    // Collision normal vector
    float nx = dx / dist;
    float ny = dy / dist;

    // Reflect velocity
    float dot = ball->vx * nx + ball->vy * ny;
    ball->vx = (ball->vx - 2 * dot * nx) * DAMPING;
    ball->vy = (ball->vy - 2 * dot * ny) * DAMPING;

    // Calculate collision angle
    float angle = atan2(-dy, -dx) * (180.0f / M_PI);
    int sound_idx = get_sound_index(angle);

    // Play corresponding sound
    if (sounds[sound_idx]) {
      Mix_PlayChannel(-1, sounds[sound_idx], 0);
    }

    // Reposition ball to boundary
    ball->x = WINDOW_SIZE / 2 + nx * max_dist;
    ball->y = WINDOW_SIZE / 2 + ny * max_dist;
  }
}

int main(int argc, char *argv[]) {
  SDL_Window *window;
  SDL_GLContext glContext;
  Ball ball = {WINDOW_SIZE / 2, WINDOW_SIZE / 2, INIT_VELOCITY,
               -1 * INIT_VELOCITY};
  Uint32 last_time = SDL_GetTicks();

  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
  window = SDL_CreateWindow("Musical Circle", SDL_WINDOWPOS_CENTERED,
                            SDL_WINDOWPOS_CENTERED, WINDOW_SIZE, WINDOW_SIZE,
                            SDL_WINDOW_OPENGL);
  glContext = SDL_GL_CreateContext(window);

  Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
  load_sounds();

  // OpenGL setup
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, WINDOW_SIZE, WINDOW_SIZE, 0, -1, 1);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  int running = 1;
  while (running) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT)
        running = 0;
    }

    // Calculate delta time
    Uint32 current_time = SDL_GetTicks();
    float dt = (current_time - last_time) / 1000.0f;
    last_time = current_time;

    physics_update(&ball, dt);

    // Rendering
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Draw outer circle
    glColor4f(1.0f, 1.0f, 1.0f, 0.2f);
    draw_circle(WINDOW_SIZE / 2, WINDOW_SIZE / 2, OUTER_RADIUS, 360);

    // Draw ball
    glColor3f(0.2f, 0.8f, 0.4f);
    draw_circle(ball.x, ball.y, BALL_RADIUS, 36);

    // Draw segments fro debug
    //    glColor3f(1.0f, 1.0f, 1.0f);
    // glBegin(GL_LINES);
    // for (int i = 0; i < NUM_SOUNDS; i++) {
    //   float angle = i * (360.0f / NUM_SOUNDS);
    //   float rad = angle * (M_PI / 180.0f);
    //   glVertex2f(WINDOW_SIZE / 2, WINDOW_SIZE / 2);
    //   glVertex2f(WINDOW_SIZE / 2 + cos(rad) * OUTER_RADIUS,
    //              WINDOW_SIZE / 2 + sin(rad) * OUTER_RADIUS);
    // }
    // glEnd();

    SDL_GL_SwapWindow(window);
    SDL_Delay(16);
  }

  // Cleanup
  for (int i = 0; i < NUM_SOUNDS; i++) {
    if (sounds[i])
      Mix_FreeChunk(sounds[i]);
  }
  Mix_CloseAudio();
  SDL_GL_DeleteContext(glContext);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
