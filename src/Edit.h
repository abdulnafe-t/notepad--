#ifndef EDIT_H
#define EDIT_H

#include "File_io.h"

#include <SDL3/SDL_clipboard.h>
#include <optional>

namespace Edit {
      void cut(const std::optional<std::size_t>& mark, File_io& file);
      void copy(const std::optional<std::size_t>& mark, File_io& file);
      void paste(File_io& file);
      void select_all(File_io& file);
}

#endif
