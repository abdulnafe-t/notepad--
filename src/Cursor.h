#ifndef CURSOR_H
#define CURSOR_H

#include <SDL3/SDL.h>

class Cursor {
private:
      SDL_FRect rectangle {};
      SDL_Color color {};
      int       column {};
      int       row {};

public:
      Cursor() = default;
      Cursor(int x, int y, float w, float h, Uint8 r = 0xFF, Uint8 g = 0xFF,
             Uint8 b = 0xFF, Uint8 a = 0xFF);

      [[nodiscard]] int get_column() const;
      [[nodiscard]] int get_row() const;

      [[nodiscard]] float get_width() const;
      [[nodiscard]] float get_height() const;

      [[nodiscard]] SDL_FRect* get_rectangle();

      [[nodiscard]] Uint8 get_r() const;
      [[nodiscard]] Uint8 get_g() const;
      [[nodiscard]] Uint8 get_b() const;
      [[nodiscard]] Uint8 get_a() const;

      void set_column(int x);
      void set_row(int y);

      void set_width(float w);
      void set_height(float heigt);
};

#endif
