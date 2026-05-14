# Cellular Automata Engine
Cellular automata engine developed in **C** using **Raylib** and **Raygui**.

![Gameplay Demo](media/gameplay.gif)

## Table of Contents
- [Requirements](#requirements)
- [Installation](#installation)
- [Usage](#usage)
- [Controls](#controls)

## Requirements
* **CMake 3.12**
* **GCC**
* **Raylib**: Manual installation not required; the `CMakeLists.txt` automatically downloads it from the official repository during configuration.
* **Raygui**: Manual installation not required; the `CMakeLists.txt` automatically downloads it from the official repository during configuration.

## Installation
1. `git clone https://github.com/jonathan-bug/cellular-automata-engine.git`
2. `cd cellular-automata-engine`
3. `cmake -S . -B build`
4. `cmake --build build`
>[!IMPORTANT]
>The build process generates the `cellular-automata-engine` executable inside the `./build/` directory.

## Usage
To ensure the engine loads designs correctly, you must run the binary from the **build** folder:

1. `cd build`
2. `./cellular-automata-engine`

### Simulation
The engine operates on a **128** x **64** grid.

* **Loading**: Upon startup, the engine checks for a `simulation.rle`file. If it exists, the design is automatically loaded into the buffer.
* **Editing**: Whether a file was loaded or not, the engine allows you to interactively edit the design on the grid in real-time. This allows you to create new designs from scratch.
* **Exporting**: You can save the current grid state at any time. If `simulation.rle` already exists, the engine will update it with the new design; if it does not exist, the engine will create it.

>[!IMPORTANT]
>The engine expects the `simulation.rle` file to be in the same working directory as the binary. Ensure your design is placed inside the `./build/` folder for correct loading and saving.

## Controls
|Action|Key/Mouse|
|-|-|
|Draw|Left Click|
|Erase|Right Click|
|Start/Pause|Space|
|Move|W, A, S, D|
|Quick Save (Buffer)|E|
|Quick Load (Buffer)|Q|
|Clear Grid|R|
|Zoom|Mouse Scroll|
|Auto Center|Left Shift|