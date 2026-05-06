#ifndef FILE_IO_H
#define FILE_IO_H

#include "Gap_buffer.h"

#include <cstdio>
#include <fstream>
#include <string>

class File_io {
private:
      Gap_buffer<char> gap_buffer;
      std::string      filename;
      std::fstream     file_io;

public:
      File_io(const std::string& file_name, std::size_t buffer_size);

      ~File_io();

      // File_io(File_io&)           = delete;
      // File_io(const File_io&)     = delete;
      // File_io operator=(File_io&) = delete;

      void                 read_file_content(std::size_t start_pos, std::size_t end_pos);
      void                 write_to_file();
      friend std::ostream& operator<<(std::ostream& out, const File_io& file) {
            return out << file.gap_buffer;
      }

      void insert_letter(char letter);

      std::size_t get_buffer_size() const;

      std::string get_file_name() const;

      void backwards_delete_letter();
      void forwards_delete_char();

      std::string get_text() const;

      void move(int amount);

      std::size_t get_filesize() const;

      char get_current_char() const;

      std::size_t get_cursor_position() const;

      int get_line_size(std::size_t cursor_position) const;

      std::optional<std::size_t> get_mark() const;

      void set_mark(std::optional<std::size_t> new_mark);

      bool is_at_last_line() const;
      bool is_at_last_char() const;

};

#endif
