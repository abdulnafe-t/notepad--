#include "File_io.h"

#include <cstdio>
#include <fstream>
#include <stdexcept>

#include "Gap_buffer.h"

File_io::File_io(const std::string& file_name, std::size_t buffer_size)
  : gap_buffer {Gap_buffer<char>(buffer_size)}
  , filename {file_name}
  , file_io {file_name, std::ios::in | std::ios::out} {}

File_io::~File_io() {
      if (file_io) {
            this->file_io.flush();
            this->write_to_file();
            this->file_io.close();
      }
}

void File_io::read_file_content(std::size_t start_pos, std::size_t end_pos) {
      this->file_io.seekg(static_cast<long>(start_pos), std::ios::beg);
      for (std::size_t next_char_position {start_pos}; next_char_position < end_pos;
           ++next_char_position) {
            char next;
            this->file_io >> std::noskipws >> next;

            this->gap_buffer.insert_new_element(next);
      }

      this->gap_buffer.move_gap(0);
}

void File_io::write_to_file() {
      this->file_io.flush();
      this->file_io.close();
      this->file_io.open(this->filename, std::ios::out | std::ios::trunc);
      this->file_io << this->gap_buffer;
      this->file_io.close();
      this->file_io.open(this->filename, std::ios::in | std::ios::out);
}

void File_io::insert_letter(char letter) {
      this->gap_buffer.insert_new_element(letter);
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

std::string File_io::get_text() const {
      std::string text_content;

      for (std::size_t index {0}; index < this->gap_buffer.get_gap_begin(); ++index) {
            char letter {gap_buffer.buffer[index]};
            if (letter != '\0') {
                  text_content.push_back(gap_buffer.buffer[index]);
            }
      }

      for (std::size_t index {gap_buffer.get_gap_end()};
           index < this->gap_buffer.get_buffer_size(); ++index) {
            char letter {gap_buffer.buffer[index]};
            if (letter != '\0') {
                  text_content.push_back(gap_buffer.buffer[index]);
            }
      }

      return text_content;
}

void File_io::move(int amount) {
      this->gap_buffer.move_gap(this->gap_buffer.get_gap_begin() + amount);
}

std::size_t File_io::get_filesize() const {
      std::ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
      return static_cast<std::size_t>(in.tellg());
}

std::size_t File_io::get_cursor_position() const {
      if (gap_buffer.first_empty_char == 0) {
            return 0;
      }

      if (gap_buffer.last_empty_char + 1 == gap_buffer.get_buffer_size()) {
            return gap_buffer.first_empty_char - 1;
      }
      return (gap_buffer.last_empty_char + 1);
}

char File_io::get_current_char() const {
      return gap_buffer.buffer[get_cursor_position()];
}

std::size_t File_io::get_line_size(std::size_t cursor_position) const {
      return gap_buffer.get_line_size(cursor_position);
}

int File_io::get_mark() const {
      return this->gap_buffer.get_mark();
}

void File_io::set_mark(int new_mark) {
      this->gap_buffer.set_mark(new_mark);
};
