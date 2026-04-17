cmake_minimum_required(VERSION 4.3.1)

project(Notepad-- VERSION 0.1 DESCRIPTION "A featureless text editor" LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED On)
set(CMAKE_CXX_EXTENSIONS Off)

add_executable(notepad--
  Cursor.cpp
  Cursor.h
  File_io.cpp
  File_io.h
  Gap_buffer.h
  main.cpp)

target_link_libraries(notepad-- SDL3 SDL3_ttf)
