#define _XOPEN_SOURCE 500 /* usleep */

#include <stdio.h>
#include <unistd.h>

#include <SDL/SDL.h>
#include <GL/gl.h>

#include "controller.h"
#include "controller_state.h"
#include "debug.h"
#include "display.h"
#include "ending.h"

/* - - - - - - - - - - - - - - - - - - - - */

static struct {
  SDL_Surface *surface;
} g;

/* - - - - - - - - - - - - - - - - - - - - */

static void graphics_init(void);
static void graphics_deinit(void);
static void draw_display(void);

/* - - - - - - - - - - - - - - - - - - - - */

void *
display_run(void *v)
{
  struct display_run_args *d_args = v;
  ending_t ending                 = d_args->ending;
  controller_state_t cs           = d_args->cs;

  (void)cs;

  graphics_init();

  while (1) {
    if (ending_get(ending)) {
      break;
    }

    SDL_Delay(100);
    draw_display();
    print_info("display looping");
  }

  graphics_deinit();

  print_info("display exiting");

  return NULL;
}

/* - - - - - - - - - - - - - - - - - - - - */

static void draw_display(void) {
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
  SDL_GL_SwapBuffers();
}

/* - - - - - - - - - - - - - - - - - - - - */

void graphics_init(void)
{
  SDL_Init(SDL_INIT_VIDEO);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  g.surface = SDL_SetVideoMode(640, 480, 24, SDL_OPENGL);
}

void graphics_deinit(void)
{
  SDL_Quit();
}
