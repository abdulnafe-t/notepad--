#ifndef GUI_H
#define GUI_H

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

#include "Cursor.h"

namespace GUI {
      extern int           font_size;
      extern float         font_size_px;
      extern SDL_Window*   window;
      extern SDL_Renderer* renderer;
      extern SDL_Texture*  texture;
      extern TTF_Font*     font;
      extern int           advance;
      extern SDL_Surface*  surface;
      extern SDL_Color     text_color;
      extern SDL_Color     background_color;
      extern Cursor        cursor;
      extern int           window_height;
      extern int           window_width;
      extern Cursor        mark;
}

#endif
