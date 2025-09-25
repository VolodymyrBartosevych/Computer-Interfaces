# USB HID Dino Game

A Windows-based C++ console game controlled via USB HID device. Classic dinosaur runner game where you jump and crouch to avoid obstacles using external USB controller input.

## Features

- **USB HID Control**: Game controlled through USB HID device input
- **Jump Mechanics**: Single and double jump functionality
- **Crouch Action**: Duck under flying obstacles
- **Obstacle Variety**: Ground and flying obstacles with different colors
- **Score Tracking**: Real-time score display and final score on game over
- **Game Restart**: Restart functionality after game over

## Game Controls

### DualSense Controller Mapping
- **Jump**: Cross (✖) - `40` (allows double jump)
- **Crouch**: Circle (〇) - `24`
- **Restart**: Options (≡) - `136` (after game over)
- **Exit**: Create (▢←) - `72` (after game over)

## Visual Elements

- **Dino**: `@` (head) and `}` (body) in green
- **Ground Obstacles**: `#` in red
- **Flying Obstacles**: `*` in yellow
- **Ground**: `_` in gray

## Requirements

- Windows operating system
- C++ compiler with Windows API support (Visual Studio, MinGW, etc.) 
- Windows HID API libraries (setupapi.lib, hid.lib)
- Tested with PlayStation DualSense controller(VID 054C))

## Usage

### Running the Application

To compile the application:
```bash
g++ dino_game.cpp -o dino_game.exe -lsetupapi -lhid
```

Then run:
```bash
./dino_game.exe
```