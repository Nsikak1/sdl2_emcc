cmake_minimum_required(VERSION 3.16)
project(sdl2_hello LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(sdl2_hello main.cpp)

add_custom_command(
  TARGET sdl2_hello
  PRE_BUILD
  COMMENT "Copying font file to build directory"
  COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/font.ttf"
          "$<TARGET_FILE_DIR:sdl2_hello>/font.ttf")

# -------------------------
# Emscripten build
# -------------------------

if(EMSCRIPTEN)
  message(STATUS "Configuring for Emscripten")

  set_target_properties(sdl2_hello PROPERTIES OUTPUT_NAME "index" SUFFIX
                                                                  ".html")

  target_compile_options(sdl2_hello PRIVATE -sUSE_SDL=2 -sUSE_SDL_TTF=2)

  target_link_options(
    sdl2_hello
    PRIVATE
    -sUSE_SDL=2
    -sUSE_SDL_TTF=2
    --preload-file=font.ttf
    -sALLOW_MEMORY_GROWTH=1
    -sASYNCIFY)

  # -------------------------
  # Native build
  # -------------------------
else()
  find_package(SDL2 REQUIRED)
  find_package(SDL2_ttf REQUIRED)
  target_link_libraries(sdl2_hello PRIVATE SDL2::SDL2 SDL2_ttf::SDL2_ttf)

  # On some platforms you may need: target_link_libraries(sdl2_hello PRIVATE
  # SDL2::SDL2main)
endif()
