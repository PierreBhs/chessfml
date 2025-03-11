cmake_minimum_required(VERSION 3.25)
project(chessfml LANGUAGES CXX)

include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.0.0
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(SFML)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_options(-Wall -Wextra -Wpedantic -Werror -Wno-missing-field-initializers)

add_executable(${PROJECT_NAME}
                ${CMAKE_SOURCE_DIR}/src/main.cpp
                
                ${CMAKE_SOURCE_DIR}/src/game/game.cpp
                ${CMAKE_SOURCE_DIR}/src/game/board.cpp
                ${CMAKE_SOURCE_DIR}/src/game/moves.cpp
                ${CMAKE_SOURCE_DIR}/src/game/game_state.cpp # Find a better name to not confuse with the states/
                
                ${CMAKE_SOURCE_DIR}/src/states/state_manager.cpp
                ${CMAKE_SOURCE_DIR}/src/states/menu_state.cpp
                ${CMAKE_SOURCE_DIR}/src/states/play_state.cpp
                ${CMAKE_SOURCE_DIR}/src/states/load_game_state.cpp
                ${CMAKE_SOURCE_DIR}/src/states/game_over.cpp
                ${CMAKE_SOURCE_DIR}/src/states/game_selection_state.cpp

                ${CMAKE_SOURCE_DIR}/src/ui/menu_renderer.cpp
                ${CMAKE_SOURCE_DIR}/src/ui/board_renderer.cpp
                
                ${CMAKE_SOURCE_DIR}/src/common/fen.cpp
)

include_directories(${CMAKE_SOURCE_DIR}/src/include)
target_link_libraries(${PROJECT_NAME} PRIVATE SFML::Graphics)

execute_process(
    COMMAND ${CMAKE_COMMAND} -E create_symlink
        ${CMAKE_BINARY_DIR}/compile_commands.json
        ${CMAKE_SOURCE_DIR}/compile_commands.json
)
