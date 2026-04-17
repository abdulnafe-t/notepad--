#ifndef KEYS_H
#define KEYS_H

#include <SDL3/SDL_keyboard.h>

#include "File_io.h"

namespace Keys {
      bool handle_key(SDL_Keycode key, File_io& file);
}

#endif
