cmake_minimum_required(VERSION 4.3.1)

project(Notepad-- VERSION 0.1 DESCRIPTION "A featureless text editor" LANGUAGES C CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED On)
set(CMAKE_CXX_EXTENSIONS Off)

add_compile_options(-pedantic-errors -Werror -Wshadow -Wall -Weffc++ -Wextra -Wconversion -Wsign-conversion -O0 -g -Wno-braced-scalar-init)

add_library(imgui STATIC
  ../vendor/imgui/imgui.cpp
  ../vendor/imgui/imgui_draw.cpp
  ../vendor/imgui/imgui_tables.cpp
  ../vendor/imgui/imgui_widgets.cpp
  ../vendor/imgui/backends/imgui_impl_sdl3.cpp
  ../vendor/imgui/backends/imgui_impl_sdlrenderer3.cpp)

target_include_directories(imgui SYSTEM PUBLIC
  ../vendor/imgui
  ../vendor/imgui/backends)

target_compile_options(imgui PRIVATE -w)

add_executable(notepad--
  Cursor.cpp
  Cursor.h
  Edit.cpp
  Edit.h
  File_io.cpp
  File_io.h
  Gap_buffer.h
  GUI.cpp
  GUI.h
  Keys.cpp
  Keys.h
  main.cpp)

target_link_libraries(notepad-- imgui SDL3 SDL3_ttf)
