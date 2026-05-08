#ifndef GAP_BUFFER_H
#define GAP_BUFFER_H

#include <algorithm>
#include <cassert>
#include <iostream>
#include <iterator>
#include <optional>
#include <utility>
#include <vector>

template<typename T>
class Gap_buffer {
private:
      static const std::size_t gap_size {300};

      std::vector<T> buffer {std::vector<T>(1000)};
      std::size_t    first_empty_char {0};  // Inclusive
      std::size_t    last_empty_char {299}; // Inclusive

      std::optional<std::size_t> mark;
      // Where the beginning of the active region (aka the mark) is. ‘std::nullopt’
      // means there is no active region.

public:
      Gap_buffer(std::size_t buffer_size, char initial = '\0');

      [[nodiscard]] std::size_t get_current_gap_size() const;

      [[nodiscard]] std::size_t get_gap_begin() const;
      [[nodiscard]] std::size_t get_gap_end() const;

      void insert_new_element(T element);
      void create_new_gap(std::size_t position);
      void move_gap(std::size_t position);
      void move_mark(std::size_t position);
      void grow_gap(int amount);
      void set_mark(std::optional<std::size_t> new_mark);

      [[nodiscard]] std::optional<std::size_t> get_mark() const;

      [[nodiscard]] bool is_at_last_line() const;
      [[nodiscard]] bool is_at_last_char() const;

      [[nodiscard]] std::size_t get_buffer_size() const;
      [[nodiscard]] int         get_line_size_containing(std::size_t position) const;

      friend std::ostream& operator<<(std::ostream&        out,
                                      const Gap_buffer<T>& gap_buffer) {
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

template<typename T>
Gap_buffer<T>::Gap_buffer(std::size_t buffer_size, char initial)
  : buffer {std::vector<T>(buffer_size, initial)}
  , mark {std::nullopt} {}

template<typename T>
std::size_t Gap_buffer<T>::get_current_gap_size() const {
      return last_empty_char - first_empty_char + 1;
}

template<typename T>
std::size_t Gap_buffer<T>::get_gap_begin() const {
      return this->first_empty_char;
}

template<typename T>
std::size_t Gap_buffer<T>::get_gap_end() const {
      return this->last_empty_char;
}

template<typename T>
std::size_t Gap_buffer<T>::get_buffer_size() const {
      return this->buffer.size();
}

template<typename T>
void Gap_buffer<T>::insert_new_element(T element) {
      if (element == T {}) {
            return;
      }

      if (this->first_empty_char >= this->last_empty_char) { // Gap is full
            this->create_new_gap(this->first_empty_char);
      }

      if (std::optional<std::size_t> current_mark {this->get_mark()}; current_mark) {
            // if current_mark is not std::nullopt, i.e. if the mark is active
            int current_mark_int {static_cast<int>(current_mark.value())};
            int current_gap_end_int {static_cast<int>(this->get_gap_end())};
            this->grow_gap(current_mark_int - current_gap_end_int - 1);
            this->set_mark(std::nullopt);
      }
      this->buffer[this->first_empty_char++] = element;
}

template<typename T>
void Gap_buffer<T>::create_new_gap(std::size_t position) {
      std::size_t old_size {this->buffer.size()};
      this->buffer.resize(old_size + Gap_buffer<T>::gap_size);
      this->first_empty_char = old_size;
      this->last_empty_char  = this->first_empty_char + Gap_buffer<T>::gap_size - 1;
      this->move_gap(position);
}

template<typename T>
void Gap_buffer<T>::move_gap(std::size_t position) {
      /** Move the gap so its beginning is at position.*/
      if (position == this->get_gap_begin() || position >= this->get_buffer_size()) {
            return;
      }

      while (this->get_gap_begin() < position &&
             this->get_gap_end() < this->get_buffer_size() - 1) {

            if (mark && (get_gap_end() + 1 == mark.value())) {
                  move_mark(get_gap_end() + 1 - get_current_gap_size());
            } else if (mark &&
                       (get_gap_end() + 1 - get_current_gap_size() == mark.value())) {
                  move_mark(get_gap_end() + 1);
            }

            std::swap(this->buffer[get_gap_end() + 1],
                      this->buffer[get_gap_end() + 1 - this->get_current_gap_size()]);

            ++this->last_empty_char;
            ++this->first_empty_char;
      }

      while (this->get_gap_begin() > position) {

            if (mark && (get_gap_begin() - 1 == mark.value())) {
                  move_mark(get_gap_begin() - 1 + get_current_gap_size());
            } else if (mark &&
                       (get_gap_begin() - 1 + get_current_gap_size() == mark.value())) {
                  move_mark(get_gap_begin() - 1);
            }
            std::swap(
            this->buffer[this->get_gap_begin() - 1],
            this->buffer[this->get_gap_begin() - 1 + this->get_current_gap_size()]);

            --this->last_empty_char;
            --this->first_empty_char;
      }
}

template<typename T>
void Gap_buffer<T>::move_mark(std::size_t position) {
      if (!get_mark() || get_mark().value() == position) {
            return;
      }
      set_mark(position);
}

template<typename T>
std::optional<std::size_t> Gap_buffer<T>::get_mark() const {
      return this->mark;
}

template<typename T>
void Gap_buffer<T>::set_mark(std::optional<std::size_t> new_mark) {
      this->mark = new_mark;
}

template<typename T>
void Gap_buffer<T>::grow_gap(int amount) {

      if (amount == 0) {
            return;
      }

      if (amount < 0) {
            std::size_t old_first_empty_char {this->first_empty_char};
            if (std::abs(amount) <= old_first_empty_char) {
                  this->first_empty_char -= static_cast<std::size_t>(std::abs(amount));
                  for (std::size_t index {this->first_empty_char};
                       index <= old_first_empty_char; ++index) {
                        this->buffer[index] = '\0';
                  }
            } else {
                  this->first_empty_char = 0;
            }
      }

      if (amount > 0) {
            std::size_t old_last_empty_char {this->last_empty_char};
            if (static_cast<std::size_t>(amount) <
                this->get_buffer_size() - old_last_empty_char) {
                  this->last_empty_char += static_cast<std::size_t>(amount);
                  for (std::size_t index {old_last_empty_char};
                       index <= this->last_empty_char; ++index)
                        this->buffer[index] = '\0';
            } else {
                  this->create_new_gap(this->last_empty_char);
                  this->buffer[first_empty_char] = '\0';
            }
      }
}

template<typename T>
bool Gap_buffer<T>::is_at_last_line() const {
      auto last_new_line {std::ranges::find_last_if(
      buffer, [](const T& element) { return element == '\n'; })};

      bool is_single_line {last_new_line.begin() == last_new_line.end()};
      auto last_new_line_index {std::distance(buffer.begin() + 1, last_new_line.begin())};

      return (is_single_line ||
              static_cast<std::size_t>(last_new_line_index) < first_empty_char);
}

template<typename T>
bool Gap_buffer<T>::is_at_last_char() const {
      auto last_non_null {std::ranges::find_last_if(
      buffer, [](const T& element) { return element != '\0'; })};
      auto last_non_null_index {std::distance(buffer.begin() + 1, last_non_null.begin())};

      return (static_cast<std::size_t>(last_non_null_index) < first_empty_char);
}

template<typename T>
int Gap_buffer<T>::get_line_size_containing(std::size_t position) const {
      /** Get the number of characters in the line containing position.
       * Specifically, count the characters backwards from position until
       * either '\n' or the beginning of the buffer, whichever comes first, then
       * count the number of characters forwards from position until either '\n' or
       * the end of the buffer, whichever comes first.
       * \param position: a std:size_t representing the position in the
       * buffer around which to measure the line_size. */

      int at_cursor {buffer[position] == '\n' ? 0 : 1};
      int chars_before {}, chars_after {};

      if (position > 0) {
            for (int index {static_cast<int>(position) - 1}; index >= 0; --index) {
                  T elt {buffer[static_cast<std::size_t>(index)]};
                  if (elt == '\0') {
                        continue;
                  }

                  if (elt == '\n') {
                        break;
                  }
                  ++chars_before;
                  if (index == 0) {
                        break;
                  }
            }
      }

      if (at_cursor == 1) {
            for (std::size_t index {position + 1}; index < get_buffer_size(); ++index) {
                  T elt {buffer[index]};
                  if (elt == '\0') {
                        continue;
                  }
                  if (elt == '\n') {
                        break;
                  }
                  ++chars_after;
            }
      }
      return chars_after + chars_before + at_cursor;
}

#endif
