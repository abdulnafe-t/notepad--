cmake_minimum_required(VERSION 4.3.1)

project(Notepad-- VERSION 0.1 DESCRIPTION "A featureless text editor" LANGUAGES CXX)

add_executable(notepad--
  main.cpp
  Cursor.cpp
  Cursor.h
  File_io.cpp
  File_io.h
  Gap_buffer.h)

target_link_libraries(notepad-- SDL3 SDL3_ttf)
