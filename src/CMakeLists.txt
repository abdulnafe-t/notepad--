cmake_minimum_required(VERSION 4.3.1)

project(Notepad-- VERSION 0.1 DESCRIPTION "A featureless text editor" LANGUAGES C CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED On)
set(CMAKE_CXX_EXTENSIONS Off)

add_compile_options(-pedantic-errors -Werror -Wshadow -Wall -Weffc++ -Wextra -Wconversion -Wsign-conversion -O0 -g -Wno-braced-scalar-init)

add_subdirectory(../vendor/imgui ${CMAKE_BINARY_DIR}/vendor/imgui)

add_executable(notepad--
  Cursor.cpp
  Cursor.h
  File_io.cpp
  File_io.h
  Gap_buffer.h
  GUI.cpp
  GUI.h
  Keys.cpp
  Keys.h
  main.cpp)

target_link_libraries(notepad-- imgui SDL3 SDL3_ttf)
