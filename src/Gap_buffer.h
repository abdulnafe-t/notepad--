#ifndef GAP_BUFFER_H
#define GAP_BUFFER_H

#include <algorithm>
#include <cassert>
#include <iostream>
#include <stdexcept>
#include <utility>
#include <vector>

template<typename T>
class Gap_buffer {
private:
      static const std::size_t gap_size {300};

      std::vector<T> buffer {std::vector<T>(1000)};
      std::size_t    first_empty_char {0};  // Inclusive
      std::size_t    last_empty_char {299}; // Inclusive

public:
      Gap_buffer(std::size_t buffer_size, char initial = '\0');

      [[nodiscard]] std::size_t get_current_gap_size() const;

      [[nodiscard]] std::size_t get_gap_begin() const;
      [[nodiscard]] std::size_t get_gap_end() const;

      void insert_new_element(T element);

      void create_new_gap(std::size_t position);

      void move_gap(std::size_t position);

      void grow_gap(int amount);

      [[nodiscard]] std::size_t get_buffer_size() const;
      [[nodiscard]] std::size_t get_line_size(std::size_t cursor_position) const;

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
  : buffer {std::vector<T>(buffer_size, initial)} {}

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
      if (this->first_empty_char >= this->last_empty_char) {
            this->create_new_gap(this->first_empty_char);
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

      if (position == this->get_gap_begin() || position >= this->get_buffer_size()) {
            return;
      }

      while (this->get_gap_begin() < position &&
             this->get_gap_end() < this->get_buffer_size() - 1) {

            std::swap(this->buffer[get_gap_end() + 1],
                      this->buffer[get_gap_end() + 1 - this->get_current_gap_size()]);

            ++this->last_empty_char;
            ++this->first_empty_char;
      }

      while (this->get_gap_begin() > position) {

            std::swap(
            this->buffer[this->get_gap_begin() - 1],
            this->buffer[this->get_gap_begin() - 1 + this->get_current_gap_size()]);

            --this->last_empty_char;
            --this->first_empty_char;
      }
}

template<typename T>
void Gap_buffer<T>::grow_gap(int amount) {

      if (amount == 0) {
            return;
      }

      if (amount < 0) {
            if (std::abs(amount) <= this->first_empty_char) {
                  this->first_empty_char += amount;
                  this->buffer[first_empty_char] = '\0';
            }
            else {
                  this->first_empty_char = 0;
            }
      }

      if (amount > 0) {
            if (amount < this->get_buffer_size() - this->last_empty_char) {
                  this->last_empty_char += amount;
                  this->buffer[last_empty_char] = '\0';
            }
            else {
                  this->create_new_gap(this->last_empty_char);
                  this->buffer[first_empty_char] = '\0';
            }
      }
}

template<typename T>
std::size_t Gap_buffer<T>::get_line_size(std::size_t cursor_position) const {
      /* Get the number of characters in the line containing cursor_position.
       * Specifically, count the characters backwards from cursor_position until either
       * '\n' or the beginning of the buffer, whichever comes first, then count the number
       * of characters forwards from cursor_position until either '\n' or the end of the
       * buffer, whichever comes first.
       * \param cursor_position: a std:size_t representing the position in the buffer
       * around which to measure the line_size. */
      std::size_t at_cursor {
      static_cast<std::size_t>(buffer[cursor_position] == '\n' ? 0 : 1)};

      int chars_before {}, chars_after {};

      if (cursor_position > 0) {
            for (std::size_t index {cursor_position - 1}; index >= 0; --index) {
                  T elt {buffer[index]};
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
            for (std::size_t index {cursor_position + 1}; index < get_buffer_size();
                 ++index) {
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

      std::size_t count_before {static_cast<size_t>(chars_before > 0 ? chars_before : 0)};
      std::size_t count_after {static_cast<size_t>(chars_after > 0 ? chars_after : 0)};

      return count_after + count_before + at_cursor;
}

#endif
