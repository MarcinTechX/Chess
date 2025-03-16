# Chess Game in SFML 3.0.0

## Description
This is a chess game implemented in C++ using SFML 3.0.0.
The game's current state is initial.

## Requirements
To build and run the project, you need:
- **C++ Compiler** (compatible with SFML 3.0.0)
- **SFML 3.0.0** (installed and linked properly)

## Installation & Setup
1. Download and install **SFML 3.0.0** from [SFML official site](https://www.sfml-dev.org/).
2. Clone this repository:
   ```sh
   git clone https://github.com/MarcinTechX/Chess
   cd <repo-folder>
   ```
3. Compile and run using your preferred method.

### Using g++ (Windows)
```sh
g++ -o chess main.cpp -I"<SFML_path>/include" -L"<SFML_path>/lib" -lsfml-graphics -lsfml-window -lsfml-system
```

## Controls
- Left-click: Select and move pieces
- ESC: Exit game

## License
This project is released under the MIT License.
