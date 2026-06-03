#include "Edit.h"

#include "File_io.h"
#include "GUI.h"
#include "Keys.h"

#include <SDL3/SDL_clipboard.h>
#include <algorithm> // For std::max, std::min
#include <optional>

void Edit::cut(const std::optional<std::size_t>& mark, File_io& file) {
      std::size_t highlighted_text_beg_index {
      std::min(mark.value(), file.get_cursor_position())};

      std::size_t highlighted_text_end_index {
      std::max(mark.value(), file.get_cursor_position())};

      std::string highlighted_text {
      file.get_text(highlighted_text_beg_index, highlighted_text_end_index)};
      SDL_SetClipboardText(highlighted_text.c_str());

      if (mark.value() < file.get_cursor_position()) {
            file.delete_text_backwards(highlighted_text_beg_index);
            GUI::cursor = GUI::mark;
      } else {
            file.delete_text_forwards(highlighted_text_end_index);
      }
      file.set_mark(std::nullopt);
};

void Edit::copy(const std::optional<std::size_t>& mark, File_io& file) {
      std::size_t highlighted_text_beg_index {
      std::min(mark.value(), file.get_cursor_position())};

      std::size_t highlighted_text_end_index {
      std::max(mark.value(), file.get_cursor_position())};

      std::string highlighted_text {
      file.get_text(highlighted_text_beg_index, highlighted_text_end_index)};
      SDL_SetClipboardText(highlighted_text.c_str());
};

void Edit::paste(File_io& file) {
      std::string clip_board_text {SDL_GetClipboardText()};
      if (!clip_board_text.empty()) {
            Keys::insert_text(clip_board_text, file);
      } else {
            std::cout << "Failed to paste text! SDL error: " << SDL_GetError() << '\n';
      }
};

void Edit::select_all(File_io& file) {
      file.move(static_cast<int>(file.get_last_char_index()) -
                static_cast<int>(file.get_cursor_position()) + 1);
      file.set_mark(0);
      GUI::cursor.set_row(file.get_number_of_lines());
      GUI::cursor.set_column(file.get_line_size(file.get_cursor_position()));
};
