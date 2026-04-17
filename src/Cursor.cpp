#include "Cursor.h"

int Cursor::get_column() const {
      return column;
}

int Cursor::get_row() const {
      return row;
}

float Cursor::get_width() const {
      return rectangle.w;
}

float Cursor::get_height() const {
      return rectangle.h;
}

SDL_FRect* Cursor::get_rectangle() {
      return &rectangle;
}

float Cursor::get_r() const {
      return color.r;
}

float Cursor::get_g() const {
      return color.g;
}
float Cursor::get_b() const {
      return color.b;
}
float Cursor::get_a() const {
      return color.a;
}

void Cursor::set_column(int x) {
      rectangle.x = x * rectangle.w;
      column      = x;
}

void Cursor::set_row(int y) {
      rectangle.y = y * rectangle.h;
      row         = y;
}

void Cursor::set_width(float w) {
      rectangle.w = w;
}

void Cursor::set_height(float h) {
      rectangle.h = h;
}

Cursor::Cursor(int x, int y, float w, float h, Uint8 r, Uint8 g, Uint8 b, Uint8 a)
  : rectangle {.x {static_cast<float>(x)}, .y {static_cast<float>(y)}, .w {w}, .h {h}}
  , color {.r {r}, .g {g}, .b {b}, .a {a}} {}
