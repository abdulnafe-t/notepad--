#ifndef GUI_H
#define GUI_H

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

#include "Cursor.h"

namespace GUI {
      static constexpr int font_size {30};
      static float         font_size_px {font_size * 96.0 / 72.0};
      static SDL_Window*   window;
      static SDL_Renderer* renderer;
      static SDL_Texture*  texture;
      static TTF_Font*     font;
      static int           advance;
      static SDL_Surface*  surface;
      static SDL_Color text_color = {.r {255}, .g {255}, .b {255}, .a {SDL_ALPHA_OPAQUE}};
      static SDL_Color background_color = {.r {0}, .g {0}, .b {0}, .a {SDL_ALPHA_OPAQUE}};
      static Cursor    cursor {
      0,
      0,
      static_cast<float>(font_size / 2.0),
      font_size,
      text_color.r,
      0,
      text_color.b,
      text_color.a,
      };

      static int window_height {600};
      static int window_width {800};
      static int col_count;
}

#endif
