#define _XOPEN_SOURCE 500 /* usleep */

#include <stdio.h>
#include <unistd.h>

#include <SDL/SDL.h>
#if 0
#include <SFML/Graphics.h>
#endif

#include "controller.h"
#include "controller_state.h"
#include "debug.h"
#include "display.h"
#include "ending.h"

/* - - - - - - - - - - - - - - - - - - - - */

struct {
  SDL_Surface *surface;
} g;

#if 0
struct {
  sfRenderWindow *window;
} g;
#endif

/* - - - - - - - - - - - - - - - - - - - - */

static void graphics_init(void);
static void graphics_deinit(void);

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

    usleep(10000);
  }

  graphics_deinit();

  return NULL;
}

/* - - - - - - - - - - - - - - - - - - - - */

void graphics_init(void)
{
  SDL_Init(SDL_INIT_VIDEO);
  g.surface = SDL_SetVideoMode(640, 480, 8, SDL_OPENGL);
}

void graphics_deinit(void)
{
  SDL_Quit();
}

#if 0
void graphics_init(void)
{
  sfVideoMode video_mode = {
    .Width        = 320,
    .Height       = 480,
    .BitsPerPixel = 32
  };

  sfWindowSettings window_settings = {
    .DepthBits         = 24,
    .StencilBits       = 8,
    .AntialiasingLevel = 2
  };

  g.window = sfRenderWindow_Create(video_mode, "quiz", sfResize | sfClose, window_settings);
  sfRenderWindow_Clear(g.window, sfBlack);
}

void graphics_deinit(void)
{
  sfRenderWindow_Close(g.window);
  sfRenderWindow_Destroy(g.window);
}
#endif
