#include "Keys.h"

#include <SDL3/SDL_keyboard.h>

#include "GUI.h"

bool Keys::handle_key(SDL_Keycode key, File_io& file) {

      bool running {true};
      switch (key) {
      case SDLK_BACKSPACE: {
            if (GUI::cursor.get_column() <= 0 && GUI::cursor.get_row() <= 0) {
                  break;
            }

            int mark {file.get_mark()};

            if (GUI::cursor.get_column() <= 0 && GUI::cursor.get_row() > 0) {

                  file.move(-1);

                  GUI::cursor.set_row((GUI::cursor.get_row() - 1));

                  std::size_t cursor_position_in_buffer {file.get_cursor_position()};

                  GUI::cursor.set_column(
                  static_cast<int>(file.get_line_size(cursor_position_in_buffer)));

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

                  if (file.get_mark() < 0) {
                        file.set_mark(static_cast<int>(file.get_cursor_position()));
                  }
            }

            else {
                  file.set_mark(-1);
            }

            file.move(-1);

            if (GUI::cursor.get_column() <= 0 && GUI::cursor.get_row() > 0) {

                  GUI::cursor.set_row((GUI::cursor.get_row() - 1));

                  std::size_t cursor_position_in_buffer {file.get_cursor_position()};

                  GUI::cursor.set_column(
                  static_cast<int>(file.get_line_size(cursor_position_in_buffer)));
                  break;
            }

            GUI::cursor.set_column((GUI::cursor.get_column() - 1));

            break;
      }

      case SDLK_RIGHT: {

            if (SDL_GetModState() &
                (SDL_KMOD_LSHIFT |
                 SDL_KMOD_RSHIFT)) { // The user is trying to highlight text.

                  if (file.get_mark() < 0) {
                        file.set_mark(static_cast<int>(file.get_cursor_position()));
                  }
            }

            else {
                  file.set_mark(-1);
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

                  if (file.get_mark() < 0) {
                        file.set_mark(static_cast<int>(file.get_cursor_position()));
                  }
            }

            else {
                  file.set_mark(-1);
            }

            file.move(-GUI::cursor.get_column() - 1); /* Move back to the end of the
                                                         previous line */

            std::size_t cursor_position_in_buffer {file.get_cursor_position()};

            if (file.get_line_size(cursor_position_in_buffer) >
                GUI::cursor.get_column()) { /* The previous line
                                               extends farther than the
                                               horizontal position of
                                               the (visible) cursor */
                  file.move(
                  -static_cast<int>(file.get_line_size(cursor_position_in_buffer)) +
                  GUI::cursor.get_column());
            }

            else {
                  GUI::cursor.set_column(file.get_line_size(cursor_position_in_buffer));
            }

            GUI::cursor.set_row(GUI::cursor.get_row() - 1);
            break;
      }

      case SDLK_DOWN: {

            if (SDL_GetModState() &
                (SDL_KMOD_LSHIFT |
                 SDL_KMOD_RSHIFT)) { // The user is trying to highlight text.

                  if (file.get_mark() < 0) {
                        file.set_mark(static_cast<int>(file.get_cursor_position()));
                  }
            }

            else {
                  file.set_mark(-1);
            }

            std::size_t cursor_position_in_buffer {file.get_cursor_position()};

            std::size_t line_size {file.get_line_size(cursor_position_in_buffer)};

            file.move(line_size - GUI::cursor.get_column() +
                      1); /* Move to beginning of next line */

            std::size_t new_cursor_position_in_buffer {file.get_cursor_position()};

            std::size_t new_line_size {file.get_line_size(new_cursor_position_in_buffer)};

            if (new_line_size < GUI::cursor.get_column()) { /* The next line doesn't
                                                               extend as far as the
                                                               current position of the
                                                               (visible) cursor*/
                  file.move(new_line_size);

                  GUI::cursor.set_column(new_line_size);
            }

            else {
                  file.move(GUI::cursor.get_column());
            }

            GUI::cursor.set_row(GUI::cursor.get_row() + 1);
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
