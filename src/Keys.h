#ifndef KEYS_H
#define KEYS_H

#include "File_io.h"

#include <SDL3/SDL_keyboard.h>

namespace Keys {
      bool handle_key(SDL_Keycode key, File_io& file);
}

#endif
