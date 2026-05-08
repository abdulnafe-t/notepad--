#ifndef KEYS_H
#define KEYS_H

#include "File_io.h"

#include <SDL3/SDL_clipboard.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_keyboard.h>
#include <string_view>

namespace Keys {
      bool handle_key(SDL_Event event, File_io& file);
      void insert_text(std::string_view text, File_io& file);
}
#endif
