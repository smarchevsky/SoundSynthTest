cmake_minimum_required(VERSION 3.5)

project(SoundSynthTest LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(SoundSynthTest main.cpp synth.cpp)

target_link_libraries(${PROJECT_NAME} GL SDL2 SDL2_mixer)
