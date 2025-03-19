# Arduino LED Matrix Tetris Game

This project is a classic Tetris game implemented for the **Arduino Uno Rev 4** using its onboard **12x8 LED matrix**. The game is controlled via a keyboard, and the serial input is sent to the Arduino using a Python script (`serial_sender.py`), however it can be easily modified to be controlled by for example physical buttons. The game features all the standard Tetris mechanics, including piece rotation, movement, line clearing, scoring, and increasing difficulty as the game progresses.

---
## Demo

https://i.imgur.com/lPMKZ1p.mp4

See above

---

## Features

- **Game Board**: 12 rows x 8 columns LED matrix.
- **Tetrominoes**: All 7 classic Tetris pieces (I, J, L, O, S, T, Z) with 4 rotations each.
- **Controls**:
  - Move left: `A` or left arrow.
  - Move right: `D` or right arrow.
  - Rotate: `W` or up arrow.
  - Soft drop: `S` or down arrow.
  - Reset game: `R` or `DEL` key.
- **Scoring**: Points are awarded based on the number of lines cleared at once (40, 100, 300, 1200 points for 1, 2, 3, 4 lines respectively).
- **Levels**: The game speeds up as the player progresses through levels. Levels increase every 1000 points.
- **Game Over Screen**: Displays "GAME OVER", the final score, and prompts the player to press `R` to restart.

---

## Hardware Requirements

- **Arduino Uno Rev 4** with onboard LED matrix.
- USB cable for power and serial communication.
- Computer with Python installed (for `serial_sender.py`).

---

## Software Requirements

- **Arduino IDE** (to upload the game code to the Arduino).
- **Python 3.x** (to run the `serial_sender.py` script for keyboard input).

---

## How to Play

1. **Upload the Code**:
   - Open the `Tetris.ino` file in the Arduino IDE.
   - Connect your Arduino Uno Rev 4 to your computer via USB.
   - Select the correct board and port in the Arduino IDE.
   - Upload the code to the Arduino.

2. **Run the Serial Sender Script**:
   - Open a terminal or command prompt.
   - Navigate to the directory containing `serial_sender.py`.
   - Run the script using the command:
     ```bash
     python serial_sender.py
     ```
   - The script will send keyboard inputs to the Arduino over the serial connection.

3. **Play the Game**:
   - Use the following keys to control the game:
     - `A` or left arrow: Move piece left.
     - `D` or right arrow: Move piece right.
     - `W` or up arrow: Rotate piece.
     - `S` or down arrow: Soft drop piece.
     - `R`: Reset the game.
   - The game will display on the Arduino's LED matrix.

---

## Code Overview

### Key Functions

- **`setup()`**: Initializes the LED matrix, serial communication, and starts the game.
- **`loop()`**: Handles game logic, including input, piece movement, and rendering.
- **`handleInput()`**: Processes keyboard input to move or rotate the current piece.
- **`movePieceDown()`**: Moves the current piece down and checks for collisions.
- **`checkLines()`**: Checks for completed lines, clears them, and updates the score.
- **`updateDisplay()`**: Renders the current game state on the LED matrix.
- **`showGameOverScreen()`**: Displays the game over screen with the final score and restart prompt.

### Data Structures

- **`gameBoard`**: A 2D array representing the current state of the game board.
- **`TETROMINOS`**: A 4D array defining the shapes and rotations of all 7 Tetris pieces.

---

## Credits

This project was created by me as a fun implementation of Tetris for the Arduino Uno Rev 4. It uses the `ArduinoGraphics` and `Arduino_LED_Matrix` libraries for rendering.

---

## License

This project is open-source and available under the MIT License. Feel free to modify and distribute it as you see fit.

---
