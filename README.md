# C++ Chess Implementation with SFML 3.0
Wanted to try the latest release of SMFL (https://www.sfml-dev.org/documentation/3.0.0/) with some newer features. Implemented a little Chess game that lets you play against AI (albeit a dumb one as of now) or against a Human.  

## Features

* Complete chess rule implementation including special moves (castling, en passant, promotion)
* Multiple game modes:
  * Human vs Human
  * Human vs AI (play as white or black)
* Game state loading with FEN notation


## Requirements

* C++23 compatible compiler (GCC 14+, Clang 18+ ...)
  * Using C++23's deducing this
  * Ranges
* CMake

## Building the project

```bash
git clone git@github.com:PierreBhs/chessfml.git
cd chessfml

cmake -S . -B build
cmake --build build

./build/chessfml
```

## Project Structure

* src/game/ - Core chess logic and game state management
* src/states/ - Game state handling (menu, gameplay, etc.)
* src/ui/ - Rendering and user interface components
* src/common/ - Utilities and common functionality


https://github.com/user-attachments/assets/50bc4875-3ddc-4920-a583-4824a8c882bb

