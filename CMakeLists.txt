cmake_minimum_required(VERSION 3.28)
project(Dead_Cell CXX)

set(SFML_ROOT "D:/SFML-3.0.2")
list(APPEND CMAKE_PREFIX_PATH "${SFML_ROOT}/lib/cmake/SFML")

include(FetchContent)
FetchContent_Declare(sfml GIT_REPOSITORY https://github.com/SFML/SFML.git GIT_TAG 3.0.2)
FetchContent_MakeAvailable(sfml)

set(PROJECT_SOURCES
        Main.cpp
        src/Game.cpp
        src/Animation.cpp
        src/Player.cpp
        src/Physics.cpp
        src/PlayerState.cpp
)
add_executable(Dead_Cell ${PROJECT_SOURCES})
target_include_directories(Dead_Cell PRIVATE ${CMAKE_SOURCE_DIR}/include)


target_compile_features(Dead_Cell PRIVATE cxx_std_20)
target_link_libraries(Dead_Cell PRIVATE SFML::Graphics)
target_link_libraries(Dead_Cell PRIVATE SFML::Audio)
