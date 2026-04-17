#include <SDL3/SDL.h>
#include <SDL3/SDL_keyboard.h>
#include <SDL3/SDL_main.h>
#include <SDL3_ttf/SDL_ttf.h>

#include "File_io.h"
#include "GUI.h"
#include "Gap_buffer.h"
#include "Keys.h"

bool init() {
      bool success {true};

      if (!SDL_Init(SDL_INIT_VIDEO)) {
            SDL_Log("init(): failed to initialize SDL. SDL error: %s\n", SDL_GetError());
            success = false;
      }
      else {
            if (!SDL_CreateWindowAndRenderer("Notepad--", GUI::window_width,
                                             GUI::window_height, SDL_WINDOW_RESIZABLE,
                                             &GUI::window, &GUI::renderer)) {
                  SDL_Log("init(): failed to create window and renderer. SDL error: %s\n",
                          SDL_GetError());
                  success = false;
            }
      }
      return success;
}

bool initialize_text() {

      bool success {true};

      if (!TTF_Init()) {
            SDL_Log("init(): failed to initialize TTF. SDL error: %s\n", SDL_GetError());
            success = false;
      }
      else {

            std::string font_path {"/home/scion/.local/share/fonts/GeistMonoNerd/"
                                   "GeistMonoNerdFontMono-Regular.otf"};

            if (GUI::font = TTF_OpenFont(font_path.c_str(), GUI::font_size);
                GUI::font == nullptr) {
                  SDL_Log("Could not load %s! SDL_ttf Error: %s\n", font_path.c_str(),
                          SDL_GetError());
                  success = false;
            }
            else {
                  TTF_GetGlyphMetrics(GUI::font, 'T', nullptr, nullptr, nullptr, nullptr,
                                      &GUI::advance);

                  GUI::cursor.set_width(GUI::advance);
                  GUI::cursor.set_height(TTF_GetFontHeight(GUI::font));
            }
      }
      return success;
}

void close() {

      SDL_DestroyTexture(GUI::texture);
      SDL_DestroySurface(GUI::surface);

      GUI::texture = nullptr;
      GUI::surface = nullptr;

      TTF_CloseFont(GUI::font);
      GUI::font = nullptr;

      SDL_DestroyRenderer(GUI::renderer);
      GUI::renderer = nullptr;

      SDL_DestroyWindow(GUI::window);
      GUI::window = nullptr;

      TTF_Quit();
      SDL_Quit();
}

int main() {

      bool running {true};
      int  return_code {0};

      init();
      initialize_text();

      File_io     file {"/home/scion/Projects/Notepad--/testfile.txt", 1000};
      std::size_t file_size {file.get_filesize()};

      file.read_file_content(0, file_size);

      SDL_Rect input_area {
      .x = 0, .y = 0, .w = GUI::window_width, .h = GUI::window_height};

      SDL_SetTextInputArea(GUI::window, &input_area, 0);
      SDL_StartTextInput(GUI::window);

      /* if (file_size > 0)*/
      if (GUI::surface = TTF_RenderText_Blended_Wrapped(
          GUI::font, file.get_text().data(), file.get_text().size(), GUI::text_color, 0);

          GUI::surface == nullptr) {
            SDL_Log("Could not create text surface! SDL error: %s\n", SDL_GetError());
            return_code = 1;
      }
      else {

            if (GUI::texture = SDL_CreateTextureFromSurface(GUI::renderer, GUI::surface);
                GUI::texture == nullptr) {
                  SDL_Log("Could not create texture from text surface! SDL error: %s\n",
                          SDL_GetError());
                  return_code = 2;
            }
            else {
                  SDL_DestroySurface(GUI::surface);
                  GUI::surface = nullptr;

                  float text_w, text_h;
                  SDL_GetTextureSize(GUI::texture, &text_w, &text_h);
                  SDL_SetRenderDrawColor(GUI::renderer, GUI::background_color.r,
                                         GUI::background_color.g, GUI::background_color.b,
                                         GUI::background_color.a);
                  SDL_RenderClear(GUI::renderer);

                  SDL_FRect text_area {.x = 0, .y = 0, .w = text_w, .h = text_h};
                  SDL_RenderTexture(GUI::renderer, GUI::texture, nullptr, &text_area);
                  SDL_RenderPresent(GUI::renderer);

                  while (running) {
                        SDL_Event event;

                        SDL_GetWindowSize(GUI::window, &GUI::window_width,
                                          &GUI::window_height);

 
                        while (SDL_PollEvent(&event)) {

                              switch (event.type) {
                              case SDL_EVENT_QUIT: {
                                    running = false;
                                    break;
                              }

                              case SDL_EVENT_TEXT_INPUT: {
                                    if (event.text.text == nullptr) {
                                          break;
                                    }

                                    const char first_char {
                                    static_cast<char>(event.text.text[0])};
                                    file.insert_letter(first_char);
                                    GUI::cursor.set_column(
                                    (GUI::cursor.get_column() + 1));

                                    break;
                              }

                              case SDL_EVENT_KEY_DOWN: {
                                    running = Keys::handle_key(event.key.key, file);
                                    break;
                              }
                              }

                              SDL_DestroyTexture(GUI::texture);
                              GUI::surface = TTF_RenderText_Blended_Wrapped(
                              GUI::font, file.get_text().data(), file.get_text().size(),
                              GUI::text_color, 0);

                              if (GUI::surface != nullptr) {
                                    GUI::texture = SDL_CreateTextureFromSurface(
                                    GUI::renderer, GUI::surface);
                              }

                              else {
                                    GUI::surface = TTF_RenderText_Blended_Wrapped(
                                    GUI::font, "", 0, GUI::text_color, 0);
                                    GUI::texture = SDL_CreateTextureFromSurface(
                                    GUI::renderer, GUI::surface);
                              }
                              SDL_DestroySurface(GUI::surface);
                              SDL_RenderClear(GUI::renderer);

                              SDL_SetRenderDrawColor(
                              GUI::renderer, GUI::cursor.get_r(), GUI::cursor.get_g(),
                              GUI::cursor.get_b(), GUI::cursor.get_a());
                              SDL_RenderFillRect(GUI::renderer,
                                                 GUI::cursor.get_rectangle());

                              SDL_GetTextureSize(GUI::texture, &text_w, &text_h);
                              text_area = {.x = 0, .y = 0, .w = text_w, .h = text_h};
                              SDL_RenderTexture(GUI::renderer, GUI::texture, nullptr,
                                                &text_area);

                              SDL_SetRenderDrawColor(GUI::renderer, 0x00, 0x00, 0x00,
                                                     0xFF);

                              SDL_RenderPresent(GUI::renderer);

                              SDL_SetTextInputArea(GUI::window, &input_area, 0);
                        }
                  }
            }
            close();

            return return_code;
      }
}
