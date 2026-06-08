#include "Gap_buffer.h"

Gap_buffer::Gap_buffer(std::size_t buffer_size, char initial)
  : buffer {std::vector<char>(buffer_size, initial)}
  , mark {std::nullopt} {}

std::size_t Gap_buffer::get_current_gap_size() const {
      return last_empty_char - first_empty_char + 1;
}

std::size_t Gap_buffer::get_gap_begin() const {
      return this->first_empty_char;
}

std::size_t Gap_buffer::get_gap_end() const {
      return this->last_empty_char;
}

std::size_t Gap_buffer::get_buffer_size() const {
      return this->buffer.size();
}

void Gap_buffer::insert_new_element(char element) {
      if (element == char {}) {
            return;
      }

      if (this->first_empty_char >= this->last_empty_char) { // Gap is full
            this->create_new_gap(this->first_empty_char);
      }

      if (std::optional<std::size_t> current_mark {this->get_mark()}; current_mark) {
            // if current_mark is not std::nullopt, i.e. if the mark is active
            int current_mark_int {static_cast<int>(current_mark.value())};
            int current_gap_end_int {static_cast<int>(this->get_gap_end())};
            int current_gap_begin_int {static_cast<int>(this->get_gap_begin())};

            if (current_mark_int > current_gap_end_int) {
                  this->grow_gap(current_mark_int - current_gap_end_int - 1);
            } else {
                  this->grow_gap(current_mark_int - current_gap_begin_int);
            }
            this->set_mark(std::nullopt);
      }

      this->buffer[this->first_empty_char++] = element;
}

void Gap_buffer::create_new_gap(std::size_t position) {
      std::size_t old_size {this->buffer.size()};
      this->buffer.resize(old_size + Gap_buffer::gap_size);
      this->first_empty_char = old_size;
      this->last_empty_char  = this->first_empty_char + Gap_buffer::gap_size - 1;
      this->move_gap(position);
}

void Gap_buffer::move_gap(std::size_t position) {
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

void Gap_buffer::move_mark(std::size_t position) {
      if (!get_mark() || get_mark().value() == position) {
            return;
      }
      set_mark(position);
}

std::optional<std::size_t> Gap_buffer::get_mark() const {
      return this->mark;
}

void Gap_buffer::set_mark(std::optional<std::size_t> new_mark) {
      this->mark = new_mark;
}

void Gap_buffer::grow_gap(int amount) {

      if (amount == 0) {
            return;
      }

      if (amount < 0) {
            std::size_t old_first_empty_char {this->first_empty_char};
            if (static_cast<std::size_t>(std::abs(amount)) <= old_first_empty_char) {
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

bool Gap_buffer::is_at_last_line() const {
      auto last_new_line {std::ranges::find_last_if(
      buffer, [](const char& element) { return element == '\n'; })};

      // The buffer contains no newlines
      if (last_new_line.begin() >= last_new_line.end()) {
            return true;
      }

      auto first_new_line {
      std::ranges::find_if(buffer, [](const char& element) { return element == '\n'; })};

      // The buffer contains exactly one newline with nothing after it
      if (first_new_line == last_new_line.begin()) {
            auto after_last_new_line {
            std::ranges::find_if(last_new_line.begin() + 1, buffer.end(),
                                 [](const char& element) { return element != '\0'; })};
            if (after_last_new_line == buffer.end()) {
                  return true;
            }
      }

      auto last_new_line_index {std::distance(buffer.begin(), last_new_line.begin())};
      // The buffer contains more than one newline, and the cursor is past the last one
      return static_cast<std::size_t>(last_new_line_index) < first_empty_char;
}

bool Gap_buffer::is_at_last_char() const {

      auto last_non_null {std::ranges::find_last_if(
      buffer, [](const char& element) { return element != '\0'; })};

      if (last_non_null.begin() == last_non_null.end()) { // No non-null elements found
            return true;
      }

      auto last_non_null_index {std::distance(buffer.begin(), last_non_null.begin())};

      return (static_cast<std::size_t>(last_non_null_index) < first_empty_char);
}

int Gap_buffer::get_number_of_lines() const {
      return static_cast<int>(std::count(this->buffer.begin(), this->buffer.end(), '\n'));
}

int Gap_buffer::get_line_size_containing(std::size_t position) const {
      /** Get the number of characters in the line containing position.
       * Specifically, count the characters backwards from position until
       * either '\n' or the beginning of the buffer, whichever comes first, then
       * count the number of characters forwards from position until either '\n' or
       * the end of the buffer, whichever comes first.
       * \param position: a std:size_t representing the position in the
       * buffer around which to measure the line_size. */

      int at_cursor {(buffer[position] == '\n' || buffer[position] == '\0') ? 0 : 1};
      int chars_before {}, chars_after {};

      if (position > 0) {
            for (int index {static_cast<int>(position) - 1}; index >= 0; --index) {
                  char elt {buffer[static_cast<std::size_t>(index)]};
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

      if (at_cursor == 1 || position == 0) {
            for (std::size_t index {position + 1}; index < get_buffer_size(); ++index) {
                  char elt {buffer[index]};
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
