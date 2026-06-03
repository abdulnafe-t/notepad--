#ifndef KEYS_H
#define KEYS_H

#include "Cursor.h"
#include "File_io.h"
#include "GUI.h"

#include <SDL3/SDL_clipboard.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_keyboard.h>
#include <string_view>

namespace Keys {
      bool handle_key(SDL_Event event, File_io& file);
      void insert_text(std::string_view text, File_io& file);
      void scroll_maybe(Cursor& cursor, SDL_Rect& camera,
                        int x_amount = GUI::x_scrolling_step,
                        int y_amount = GUI::y_scrolling_step);
}
#endif
