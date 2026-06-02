#include "Keys.h"

#include "GUI.h"

#include <SDL3/SDL_clipboard.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_keyboard.h>
#include <algorithm> // For std::max, std::min
#include <optional>
#include <string_view>

void Keys::insert_text(std::string_view text, File_io& file) {
      using namespace std::string_view_literals;
      if (text == ""sv) {
            return;
      }
      if (std::optional<std::size_t> mark {file.get_mark()}; mark) {
            // The mark is active
            if (file.get_mark().value() < file.get_cursor_position()) {
                  GUI::cursor = GUI::mark;
            }
      }
      for (const auto& letter : text) {

            file.insert_letter(letter);
            if (letter == '\n') {
                  GUI::cursor.set_column(0);
                  GUI::cursor.set_row(GUI::cursor.get_row() + 1);
            } else {
                  GUI::cursor.set_column(GUI::cursor.get_column() + 1);
            }
      }
}

bool Keys::handle_key(SDL_Event e, File_io& file) {

      bool running {true};
      switch (e.key.key) {
      case SDLK_BACKSPACE: {
            if (GUI::cursor.get_column() <= 0 && GUI::cursor.get_row() <= 0) {
                  break;
            }

            if (GUI::cursor.get_column() <= 0 && GUI::cursor.get_row() > 0) {

                  file.move(-1);
                  GUI::cursor.set_row((GUI::cursor.get_row() - 1));

                  std::size_t cursor_position_in_buffer {file.get_cursor_position()};
                  GUI::cursor.set_column(file.get_line_size(cursor_position_in_buffer));

                  file.move(1);
                  file.backwards_delete_letter();

                  break;
            }

            file.backwards_delete_letter();
            GUI::cursor.set_column((GUI::cursor.get_column() - 1));

            break;
      }

      case SDLK_ESCAPE: {
            running = false;
            break;
      }

      case SDLK_LEFT: {
            if (GUI::cursor.get_column() <= 0 && GUI::cursor.get_row() <= 0) {
                  break;
            }

            if (SDL_GetModState() &
                (SDL_KMOD_LSHIFT |
                 SDL_KMOD_RSHIFT)) { // The user is trying to highlight text.

                  if (!file.get_mark()) {
                        file.set_mark(file.get_cursor_position());
                        GUI::mark = GUI::cursor;
                  }
            } else {
                  file.set_mark(std::nullopt);
            }

            file.move(-1);

            if (GUI::cursor.get_column() <= 0 && GUI::cursor.get_row() > 0) {
                  GUI::cursor.set_row((GUI::cursor.get_row() - 1));
                  std::size_t cursor_position_in_buffer {file.get_cursor_position()};
                  GUI::cursor.set_column(file.get_line_size(cursor_position_in_buffer));
                  break;
            }
            GUI::cursor.set_column((GUI::cursor.get_column() - 1));
            break;
      }

      case SDLK_RIGHT: {
            if (file.is_at_last_char()) {
                  break;
            }
            if (SDL_GetModState() &
                (SDL_KMOD_LSHIFT |
                 SDL_KMOD_RSHIFT)) { // The user is trying to highlight text.

                  if (!file.get_mark()) {
                        file.set_mark(file.get_cursor_position());
                        GUI::mark = GUI::cursor;
                  }
            } else {
                  file.set_mark(std::nullopt);
            }

            if (file.get_current_char() == '\n') {
                  GUI::cursor.set_row(GUI::cursor.get_row() + 1);
                  GUI::cursor.set_column(0);
                  file.move(1);
                  break;
            }

            GUI::cursor.set_column((GUI::cursor.get_column() + 1));
            file.move(1);
            break;
      }

      case SDLK_UP: {
            if (GUI::cursor.get_row() == 0) {
                  break;
            }

            if (SDL_GetModState() &
                (SDL_KMOD_LSHIFT |
                 SDL_KMOD_RSHIFT)) { // The user is trying to highlight text.

                  if (!file.get_mark()) {
                        file.set_mark(file.get_cursor_position());
                        GUI::mark = GUI::cursor;
                  }
            } else {
                  file.set_mark(std::nullopt);
            }

            file.move(-GUI::cursor.get_column() - 1); /* Move back to the end of the
                                                         previous line */

            std::size_t cursor_position_in_buffer {file.get_cursor_position()};
            if (file.get_line_size(cursor_position_in_buffer) >
                GUI::cursor.get_column()) { /* The previous line
                                               extends farther than the
                                               horizontal position of
                                               the (visible) cursor */
                  file.move(-file.get_line_size(cursor_position_in_buffer) +
                            GUI::cursor.get_column());
            } else {
                  GUI::cursor.set_column(file.get_line_size(cursor_position_in_buffer));
            }

            GUI::cursor.set_row(GUI::cursor.get_row() - 1);
            break;
      }

      case SDLK_DOWN: {

            if (file.is_at_last_line()) {
                  break;
            }

            if (SDL_GetModState() &
                (SDL_KMOD_LSHIFT |
                 SDL_KMOD_RSHIFT)) { // The user is trying to highlight text.

                  if (!file.get_mark()) {
                        file.set_mark(file.get_cursor_position());
                        GUI::mark = GUI::cursor;
                  }
            } else {
                  file.set_mark(std::nullopt);
            }

            std::size_t cursor_position_in_buffer {file.get_cursor_position()};
            int         line_size {file.get_line_size(cursor_position_in_buffer)};

            file.move(line_size - GUI::cursor.get_column() +
                      1); /* Move to beginning of next line */

            std::size_t new_cursor_position_in_buffer {file.get_cursor_position()};
            int         new_line_size {file.get_line_size(new_cursor_position_in_buffer)};

            if (new_line_size < GUI::cursor.get_column()) { /* The next line doesn't
                                                               extend as far as the
                                                               current position of the
                                                               (visible) cursor*/
                  file.move(new_line_size);
                  GUI::cursor.set_column(new_line_size);
            } else {
                  file.move(GUI::cursor.get_column());
            }

            GUI::cursor.set_row(GUI::cursor.get_row() + 1);
            break;
      }

      case SDLK_C: {
            // Handle copy (Ctrl+C)
            if (std::optional<std::size_t> mark {file.get_mark()};
                (SDL_GetModState() & SDL_KMOD_CTRL) && (mark)) {

                  std::size_t highlighted_text_beg_index {
                  std::min(mark.value(), file.get_cursor_position())};

                  std::size_t highlighted_text_end_index {
                  std::max(mark.value(), file.get_cursor_position())};

                  std::string highlighted_text {
                  file.get_text(highlighted_text_beg_index, highlighted_text_end_index)};
                  SDL_SetClipboardText(highlighted_text.c_str());
            }
            break;
      }

      case SDLK_X: {
            // Handle cut (Ctrl+X)
            if (std::optional<std::size_t> mark {file.get_mark()};
                (SDL_GetModState() & SDL_KMOD_CTRL) && (mark)) {

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
            }
            break;
      }

      case SDLK_V: {
            // Handle paste (Ctrl+V)
            if (SDL_GetModState() & SDL_KMOD_CTRL) {
                  std::string clip_board_text {SDL_GetClipboardText()};
                  if (!clip_board_text.empty()) {
                        Keys::insert_text(clip_board_text, file);
                  } else {
                        std::cout << "Failed to paste text! SDL error: " << SDL_GetError()
                                  << '\n';
                  }
            }
            break;
      }

      case SDLK_S: {
            // Handle save (Ctrl+s)
            if (SDL_GetModState() & SDL_KMOD_CTRL) {
                  file.save_file();
            }
            break;
      }

      case SDLK_O: {
            // Handle open (Ctrl+o)
            if (SDL_GetModState() & SDL_KMOD_CTRL) {
                  file.open_file();
            }
            break;
      }

      case SDLK_RETURN: {
            file.insert_letter('\n');
            GUI::cursor.set_row(GUI::cursor.get_row() + 1);
            GUI::cursor.set_column(0);
            break;
      }

      case SDLK_DELETE: {
            file.forwards_delete_char();
            break;
      }
      }
      return running;
}
