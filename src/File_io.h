#ifndef FILE_IO_H
#define FILE_IO_H

#include "Gap_buffer.h"

#include <SDL3/SDL_dialog.h>
#include <array>
#include <cstdio>
#include <fstream>
#include <string>
#include <string_view>

class File_io {
private:
      Gap_buffer<char> gap_buffer;
      std::string      filename;
      std::fstream     file_io_stream;
      bool             is_temp;

public:
      File_io();

      File_io(const std::string& file_name, std::size_t buffer_size);

      ~File_io();

      void read_file_content(std::size_t start_pos, std::size_t end_pos);
      void write_to_file();

      void save_file();
      void save_file_as();
      void open_file();

      static void SDLCALL save_file_callback(void* userdata, const char* const* filelist,
                                             int filter);

      friend std::ostream& operator<<(std::ostream& out, const File_io& file) {
            return out << file.gap_buffer;
      }

      std::size_t get_buffer_size() const;
      std::size_t get_filesize() const;
      std::size_t get_cursor_position() const;

      void insert_letter(char letter);
      void insert_text(std::string_view str);

      void forwards_delete_char();
      void backwards_delete_letter();
      void delete_text_forwards(std::size_t end);
      void delete_text_backwards(std::size_t begin);

      void move(int amount);
      void set_mark(std::optional<std::size_t> new_mark);

      std::string get_file_name() const;
      std::string get_text(std::size_t begin = 0, std::size_t end = 0) const;

      char get_current_char() const;

      int get_line_size(std::size_t cursor_position) const;

      std::optional<std::size_t> get_mark() const;

      bool is_at_last_line() const;
      bool is_at_last_char() const;
};

static const SDL_DialogFileFilter open_file_filter {.name {"All"}, .pattern {"*"}};
static const SDL_DialogFileFilter save_file_filter {.name {"All"}, .pattern {"*"}};

static const std::array<SDL_DialogFileFilter, 2> file_filters {open_file_filter,
                                                               save_file_filter};

#endif
