#include "GUI.h"

namespace GUI {
      int           font_size {30};
      float         font_size_px {static_cast<float>(font_size * 96.0 / 72.0)};
      SDL_Window*   window;
      SDL_Renderer* renderer;
      SDL_Texture*  texture;
      TTF_Font*     font;
      int           advance;
      SDL_Surface*  surface;
      SDL_Color     text_color = {.r {255}, .g {255}, .b {255}, .a {SDL_ALPHA_OPAQUE}};
      SDL_Color     background_color = {.r {0}, .g {0}, .b {0}, .a {SDL_ALPHA_OPAQUE}};
      Cursor        cursor {
      0,
      0,
      static_cast<float>(font_size / 2.0),
      static_cast<float>(font_size),
      text_color.r,
      0,
      text_color.b,
      text_color.a,
      };

      int window_height {600};
      int window_width {800};
      int col_count;
}
