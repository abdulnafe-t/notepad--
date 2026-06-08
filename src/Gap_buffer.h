#ifndef GAP_BUFFER_H
#define GAP_BUFFER_H

#include <algorithm>
#include <cassert>
#include <iostream>
#include <iterator>
#include <optional>
#include <utility>
#include <vector>

class Gap_buffer {
private:
      static const std::size_t gap_size {300};

      std::vector<char> buffer {std::vector<char>(1000)};
      std::size_t       first_empty_char {0};  // Inclusive
      std::size_t       last_empty_char {299}; // Inclusive

      std::optional<std::size_t> mark;
      // Where the beginning of the active region (aka the mark) is. ‘std::nullopt’
      // means there is no active region.

public:
      Gap_buffer(std::size_t buffer_size, char initial = '\0');

      [[nodiscard]] std::size_t get_current_gap_size() const;

      [[nodiscard]] std::size_t get_gap_begin() const;
      [[nodiscard]] std::size_t get_gap_end() const;

      void insert_new_element(char element);
      void create_new_gap(std::size_t position);
      void move_gap(std::size_t position);
      void move_mark(std::size_t position);
      void grow_gap(int amount);
      void set_mark(std::optional<std::size_t> new_mark);

      [[nodiscard]] int get_number_of_lines() const;

      [[nodiscard]] std::optional<std::size_t> get_mark() const;

      [[nodiscard]] bool is_at_last_line() const;
      [[nodiscard]] bool is_at_last_char() const;

      [[nodiscard]] std::size_t get_buffer_size() const;
      [[nodiscard]] int         get_line_size_containing(std::size_t position) const;

      friend std::ostream& operator<<(std::ostream& out, const Gap_buffer& gap_buffer) {
            for (std::size_t index {0}; index < gap_buffer.buffer.size(); ++index) {
                  if (index < gap_buffer.get_gap_begin() ||
                      index > gap_buffer.get_gap_end()) {
                        char letter {gap_buffer.buffer[index]};
                        if (letter != '\0') {
                              out << gap_buffer.buffer[index];
                        }
                  }
            }

            return out;
      }

      friend class File_io;
};


#endif
