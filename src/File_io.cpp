#include "File_io.h"

#include "GUI.h"
#include "Gap_buffer.h"

#include <SDL3/SDL_dialog.h>
#include <algorithm>
#include <array>
#include <cstdio>
#include <fstream>
#include <string>
#include <string_view>

File_io::File_io()
  : gap_buffer {Gap_buffer<char>(1000)}
  , filename {std::tmpnam(nullptr)}
  , file_io_stream {filename, std::ios::in | std::ios::out}
  , is_temp {true} {}

File_io::File_io(const std::string& file_name, std::size_t buffer_size)
  : gap_buffer {Gap_buffer<char>(buffer_size)}
  , filename {file_name}
  , file_io_stream {file_name, std::ios::in | std::ios::out}
  , is_temp {false} {}

File_io::~File_io() {
      if (file_io_stream) {
            this->file_io_stream.flush();
            this->write_to_file();
            this->file_io_stream.close();
      }
}

void File_io::read_file_content(std::size_t start_pos, std::size_t end_pos) {
      if (end_pos == 0) { // Avoid SDL "Text has zero width" error
            this->gap_buffer.insert_new_element('\n');
      }

      this->file_io_stream.seekg(static_cast<long>(start_pos), std::ios::beg);
      for (std::size_t next_char_position {start_pos}; next_char_position < end_pos;
           ++next_char_position) {
            char next;
            this->file_io_stream >> std::noskipws >> next;

            this->gap_buffer.insert_new_element(next);
      }

      this->gap_buffer.move_gap(0);
}

void SDLCALL File_io::open_file_callback(void* userdata, const char* const* filelist,
                                         [[maybe_unused]] int filter) {

      if (filelist == nullptr) {
            SDL_Log("An error occured: %s", SDL_GetError());
            return;
      }

      if (*filelist == nullptr) {
            SDL_Log("The user did not select any file.");
            SDL_Log("Most likely, the dialog was canceled.");
            return;
      }

      auto* self = static_cast<File_io*>(userdata);

      SDL_Log("Full path to selected file: '%s'", *filelist);
      self->file_io_stream.close();
      self->filename = *filelist;
      self->file_io_stream.open(self->filename, std::ios::in | std::ios::out);
      self->reset_gap_buffer();
      self->read_file_content(0, self->get_filesize());
      self->is_temp = false;
};

void File_io::open_file() {
      SDL_ShowOpenFileDialog(open_file_callback, this, GUI::window, file_filters.data(),
                             0, nullptr, false);
};

void File_io::new_file() {
      this->reset_gap_buffer();
      this->file_io_stream.close();
      this->filename = std::tmpnam(nullptr);
      this->file_io_stream.open(this->filename, std::ios::in | std::ios::out);
      this->read_file_content(0, this->get_filesize());
      this->is_temp = true;
};

void SDLCALL File_io::save_file_callback(void* userdata, const char* const* filelist,
                                         [[maybe_unused]] int filter) {

      if (filelist == nullptr) {
            SDL_Log("An error occured: %s", SDL_GetError());
            return;
      }

      if (*filelist == nullptr) {
            SDL_Log("The user did not select any file.");
            SDL_Log("Most likely, the dialog was canceled.");
            return;
      }

      auto* self = static_cast<File_io*>(userdata);

      SDL_Log("Full path to selected file: '%s'", *filelist);
      self->file_io_stream.close();
      self->filename = *filelist;
      self->file_io_stream.open(self->filename, std::ios::in | std::ios::out);
      self->write_to_file();
      self->is_temp = false;
};

void File_io::save_file() {
      if (this->is_temp) {
            SDL_ShowSaveFileDialog(save_file_callback, this, GUI::window,
                                   file_filters.data(), 1, nullptr);
      } else {
            this->write_to_file();
      }
};

void File_io::save_file_as() {
      SDL_ShowSaveFileDialog(save_file_callback, this, GUI::window, file_filters.data(),
                             1, nullptr);
};

void File_io::write_to_file() {
      this->file_io_stream.flush();
      this->file_io_stream.close();
      this->file_io_stream.open(this->filename, std::ios::out | std::ios::trunc);
      this->file_io_stream << this->gap_buffer;
      this->file_io_stream.close();
      this->file_io_stream.open(this->filename, std::ios::in | std::ios::out);
}

void File_io::reset_gap_buffer() {
      std::ranges::fill(this->gap_buffer.buffer, '\0');
      this->gap_buffer.move_gap(0);
      this->gap_buffer.mark = std::nullopt;
}

void File_io::insert_letter(char letter) {
      this->gap_buffer.insert_new_element(letter);
}

void File_io::insert_text(std::string_view str) {
      for (const char& letter : str) {
            this->gap_buffer.insert_new_element(letter);
      }
}

std::size_t File_io::get_buffer_size() const {
      return this->gap_buffer.buffer.size();
}

std::string File_io::get_file_name() const {
      return this->filename;
}

void File_io::backwards_delete_letter() {
      this->gap_buffer.grow_gap(-1);
}

void File_io::forwards_delete_char() {
      this->gap_buffer.grow_gap(1);
}

std::string File_io::get_text(std::size_t begin, std::size_t end) const {
      std::string text_content;

      end = end == 0 ? this->gap_buffer.get_buffer_size() : end;

      if (begin > gap_buffer.last_empty_char) { // The gap is outside the requested range
            for (std::size_t index {begin}; index < end; ++index) {
                  char letter {gap_buffer.buffer[index]};
                  if (letter != '\0') {
                        text_content.push_back(letter);
                  }
            }
      } else {

            for (std::size_t index {begin}; index < gap_buffer.first_empty_char;
                 ++index) {
                  char letter {gap_buffer.buffer[index]};
                  if (letter != '\0') {
                        text_content.push_back(letter);
                  }
            }

            for (std::size_t index {gap_buffer.last_empty_char + 1}; index < end;
                 ++index) {
                  char letter {gap_buffer.buffer[index]};
                  if (letter != '\0') {
                        text_content.push_back(letter);
                  }
            }
      }

      return text_content;
}

void File_io::move(int amount) {
      std::size_t target {};
      if (amount >= 0) {
            target = this->gap_buffer.get_gap_begin() + static_cast<std::size_t>(amount);
      } else {
            target =
            this->gap_buffer.get_gap_begin() - static_cast<std::size_t>(std::abs(amount));
      }

      this->gap_buffer.move_gap(target);
}

std::size_t File_io::get_filesize() const {
      std::ifstream           in(filename, std::ifstream::ate | std::ifstream::binary);
      std::ifstream::pos_type end_position {in.tellg()};
      return (end_position == -1 ? 0 : static_cast<std::size_t>(end_position));
}

std::size_t File_io::get_cursor_position() const {
      /* Return the index of the character currently highlighted by the cursor.*/

      if (gap_buffer.last_empty_char + 1 == gap_buffer.get_buffer_size()) {
            return gap_buffer.first_empty_char - 1;
      }

      return (gap_buffer.last_empty_char + 1);
}

char File_io::get_current_char() const {
      return gap_buffer.buffer[get_cursor_position()];
}

int File_io::get_line_size(std::size_t position) const {
      return gap_buffer.get_line_size_containing(position);
}

std::optional<std::size_t> File_io::get_mark() const {
      return this->gap_buffer.get_mark();
}

void File_io::set_mark(std::optional<size_t> new_mark) {
      this->gap_buffer.set_mark(new_mark);
};

bool File_io::is_at_last_line() const {
      return gap_buffer.is_at_last_line();
}

bool File_io::is_at_last_char() const {
      return gap_buffer.is_at_last_char();
}

void File_io::delete_text_backwards(std::size_t begin) {
      gap_buffer.grow_gap(-static_cast<int>(gap_buffer.get_gap_begin() - begin));
}

void File_io::delete_text_forwards(std::size_t end) {
      gap_buffer.grow_gap(static_cast<int>(end - gap_buffer.get_gap_end()));
}

std::ptrdiff_t File_io::get_last_char_index() const {
      auto last_non_null {std::ranges::find_last_if(
      this->gap_buffer.buffer, [](const char& letter) { return letter != '\0'; })};

      return std::distance(this->gap_buffer.buffer.begin(), last_non_null.begin());
}

int File_io::get_number_of_lines() const {
      return this->gap_buffer.get_number_of_lines();
}
