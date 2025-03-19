#include "ArduinoGraphics.h"
#include "Arduino_LED_Matrix.h"
#include <string>
ArduinoLEDMatrix matrix;

// Game board dimensions (12x8)
const byte BOARD_HEIGHT = 12;
const byte BOARD_WIDTH = 8;

// Game state
byte gameBoard[BOARD_HEIGHT][BOARD_WIDTH] = {0};
boolean gameOver = false;
unsigned long lastFallTime = 0;
unsigned int fallDelay = 800;  // Milliseconds between falls (decreases as level increases)
unsigned int score = 0;
byte level = 1;

// Current tetromino
byte currentPiece = 0;
byte currentRotation = 0;
byte currentX = 3;  // Starting X position (middle)
byte currentY = 0;  // Starting Y position (top)

// Define the Tetris pieces (tetrominos)
// Each tetromino is defined by its 4 rotations (4x4 grid)
const byte TETROMINOS[7][4][4][4] = {
  // I Piece
  {
    {{0, 0, 0, 0}, {1, 1, 1, 1}, {0, 0, 0, 0}, {0, 0, 0, 0}},
    {{0, 0, 1, 0}, {0, 0, 1, 0}, {0, 0, 1, 0}, {0, 0, 1, 0}},
    {{0, 0, 0, 0}, {0, 0, 0, 0}, {1, 1, 1, 1}, {0, 0, 0, 0}},
    {{0, 1, 0, 0}, {0, 1, 0, 0}, {0, 1, 0, 0}, {0, 1, 0, 0}}
  },
  // J Piece
  {
    {{1, 0, 0, 0}, {1, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
    {{0, 1, 1, 0}, {0, 1, 0, 0}, {0, 1, 0, 0}, {0, 0, 0, 0}},
    {{0, 0, 0, 0}, {1, 1, 1, 0}, {0, 0, 1, 0}, {0, 0, 0, 0}},
    {{0, 1, 0, 0}, {0, 1, 0, 0}, {1, 1, 0, 0}, {0, 0, 0, 0}}
  },
  // L Piece
  {
    {{0, 0, 1, 0}, {1, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
    {{0, 1, 0, 0}, {0, 1, 0, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}},
    {{0, 0, 0, 0}, {1, 1, 1, 0}, {1, 0, 0, 0}, {0, 0, 0, 0}},
    {{1, 1, 0, 0}, {0, 1, 0, 0}, {0, 1, 0, 0}, {0, 0, 0, 0}}
  },
  // O Piece
  {
    {{0, 1, 1, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
    {{0, 1, 1, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
    {{0, 1, 1, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
    {{0, 1, 1, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}
  },
  // S Piece
  {
    {{0, 1, 1, 0}, {1, 1, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
    {{0, 1, 0, 0}, {0, 1, 1, 0}, {0, 0, 1, 0}, {0, 0, 0, 0}},
    {{0, 0, 0, 0}, {0, 1, 1, 0}, {1, 1, 0, 0}, {0, 0, 0, 0}},
    {{1, 0, 0, 0}, {1, 1, 0, 0}, {0, 1, 0, 0}, {0, 0, 0, 0}}
  },
  // T Piece
  {
    {{0, 1, 0, 0}, {1, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
    {{0, 1, 0, 0}, {0, 1, 1, 0}, {0, 1, 0, 0}, {0, 0, 0, 0}},
    {{0, 0, 0, 0}, {1, 1, 1, 0}, {0, 1, 0, 0}, {0, 0, 0, 0}},
    {{0, 1, 0, 0}, {1, 1, 0, 0}, {0, 1, 0, 0}, {0, 0, 0, 0}}
  },
  // Z Piece
  {
    {{1, 1, 0, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
    {{0, 0, 1, 0}, {0, 1, 1, 0}, {0, 1, 0, 0}, {0, 0, 0, 0}},
    {{0, 0, 0, 0}, {1, 1, 0, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}},
    {{0, 1, 0, 0}, {1, 1, 0, 0}, {1, 0, 0, 0}, {0, 0, 0, 0}}
  }
};

unsigned long gameOverStartTime = 0;
int gameOverState = 0;  // 0 = "GAME OVER", 1 = "SCORE", 2 = "PRESS R TO RESTART"

void setup() {
  Serial.begin(115200);
  matrix.begin();

  // Initialize random seed
  randomSeed(analogRead(0));

  showStartScreen();
  newGame();
}

void loop() {
  if (gameOver) {
    showGameOverScreen();
    // Check if any key is pressed to restart
    if (Serial.available() > 0) {
      char key = Serial.read();
      if (key == 'r' || key == 'R') {
        newGame();
      }
    }
    return;
  }
  
  // Handle user input
  handleInput();
  
  // Handle piece falling
  unsigned long currentTime = millis();
  if (currentTime - lastFallTime > fallDelay) {
    lastFallTime = currentTime;
    movePieceDown();
  }
  
  // Update display
  updateDisplay();
}

void showStartScreen() {
  // Display TETRIS animation from built-in library
  matrix.loadSequence(LEDMATRIX_ANIMATION_TETRIS);
  matrix.play(false);
  
  // Wait for animation to complete
  while (!matrix.sequenceDone()) {
    delay(100);
  }
  
  showSplashScreen("Tetris");
  delay(50);
}

void showSplashScreen(std::string txt) {
  matrix.beginDraw();
  matrix.stroke(0xFFFFFFFF);
  matrix.textScrollSpeed(30); // Adjust scroll speed
  std::string fullText = "  " + txt; // Added spaces for smoother scrolling
  matrix.textFont(Font_5x7);
  matrix.beginText(0, 1, 0xFFFFFF);
  matrix.println(fullText.c_str());
  matrix.endText(SCROLL_LEFT);
  matrix.endDraw();
}

void showGameOverScreen() {
  unsigned long currentTime = millis();

  // Initialize the game over screen state if it's just starting
  if (gameOverStartTime == 0) {
    gameOverStartTime = currentTime;
    gameOverState = 0;
  }

  switch (gameOverState) {
    case 0:  // "GAME OVER"
      matrix.beginDraw();
      matrix.stroke(0xFFFFFFFF);
      matrix.textScrollSpeed(70);
      matrix.textFont(Font_5x7);
      matrix.beginText(0, 1, 0xFFFFFF);
      matrix.println("GAME OVER");
      matrix.endText(SCROLL_LEFT);
      matrix.endDraw();
      if (currentTime - gameOverStartTime >= 1000) {
        gameOverState = 1;
        gameOverStartTime = currentTime;
      }
      break;

    case 1:  // "SCORE: X"
      matrix.beginDraw();
      matrix.stroke(0xFFFFFFFF);
      matrix.textScrollSpeed(70);
      matrix.textFont(Font_5x7);
      matrix.beginText(0, 1, 0xFFFFFF);
      matrix.println(("SCORE: " + std::to_string(score)).c_str());
      matrix.endText(SCROLL_LEFT);
      matrix.endDraw();
      if (currentTime - gameOverStartTime >= 1000) {
        gameOverState = 2;
        gameOverStartTime = currentTime;
      }
      break;

    case 2:  // "PRESS R TO RESTART"
      matrix.beginDraw();
      matrix.stroke(0xFFFFFFFF);
      matrix.textScrollSpeed(70);
      matrix.textFont(Font_5x7);
      matrix.beginText(0, 1, 0xFFFFFF);
      matrix.println("PRESS R TO RESTART");
      matrix.endText(SCROLL_LEFT);
      matrix.endDraw();
      if (currentTime - gameOverStartTime >= 1000) {
        gameOverState = 0;
        gameOverStartTime = currentTime;
      }
      break;
  }
}

void newGame() {
  // Reset game variables
  clearBoard();
  gameOver = false;
  score = 0;
  level = 1;
  fallDelay = 800;
  
  // Reset game over screen state
  gameOverStartTime = 0;
  gameOverState = 0;

  // Spawn first piece
  spawnNewPiece();
}

void clearBoard() {
  for (byte y = 0; y < BOARD_HEIGHT; y++) {
    for (byte x = 0; x < BOARD_WIDTH; x++) {
      gameBoard[y][x] = 0;
    }
  }
}

void spawnNewPiece() {
  currentPiece = random(7);  // Choose random piece
  currentRotation = 0;
  currentX = BOARD_WIDTH / 2 - 2;  // Center horizontally
  currentY = 0;  // Top of board
  
  // Check if new piece can be placed
  if (!isValidPosition()) {
    gameOver = true;
  }
}

boolean isValidPosition() {
  for (byte y = 0; y < 4; y++) {
    for (byte x = 0; x < 4; x++) {
      if (TETROMINOS[currentPiece][currentRotation][y][x]) {
        byte boardX = currentX + x;
        byte boardY = currentY + y;
        
        // Check if outside board boundaries
        if (boardX < 0 || boardX >= BOARD_WIDTH || boardY < 0 || boardY >= BOARD_HEIGHT) {
          return false;
        }
        
        // Check if collides with existing pieces on the board
        if (boardY >= 0 && gameBoard[boardY][boardX]) {
          return false;
        }
      }
    }
  }
  return true;
}

void handleInput() {
  // Handle key press
  if (Serial.available() > 0) {
    char key = Serial.read();
    
    // Handle movement keys
    if (key == 'a' || key == 'A') {
      currentX++;
      if (!isValidPosition()) {
        currentX--;  // Undo if invalid
      }
    }
    else if (key == 'd' || key == 'D') {
      currentX--;
      if (!isValidPosition()) {
        currentX++;
      }
    }
    else if (key == 'w' || key == 'W') {
      byte prevRotation = currentRotation;
      currentRotation = (currentRotation + 1) % 4;
      if (!isValidPosition()) {
        currentRotation = prevRotation;  // Undo if invalid
      }
    }
    else if (key == 's' || key == 'S') {
      boolean hasDropped = false;
      while (movePieceDown()) {
        hasDropped = true;
      }
      if (hasDropped) {
        // Only award bonus if a row is completed after the drop
        byte linesCleared = checkLines();
        if (linesCleared > 0) {
          score += 1;  // Bonus points for manual drop leading to a completed row
        }
      }
    }
  }
}

boolean movePieceDown() {
  currentY++;
  
  if (!isValidPosition()) {
    currentY--;  // Move back up
    placePiece();  // Lock piece in place
    checkLines();  // Check for completed lines
    spawnNewPiece();  // Spawn a new piece
    return false;
  }
  
  return true;
}

void placePiece() {
  for (byte y = 0; y < 4; y++) {
    for (byte x = 0; x < 4; x++) {
      if (TETROMINOS[currentPiece][currentRotation][y][x]) {
        byte boardY = currentY + y;
        byte boardX = currentX + x;
        if (boardY >= 0 && boardY < BOARD_HEIGHT && boardX >= 0 && boardX < BOARD_WIDTH) {
          gameBoard[boardY][boardX] = 1;
        }
      }
    }
  }
}

byte checkLines() {
  byte linesCleared = 0;
  
  for (byte y = 0; y < BOARD_HEIGHT; y++) {
    boolean lineComplete = true;
    
    // Check if line is complete
    for (byte x = 0; x < BOARD_WIDTH; x++) {
      if (!gameBoard[y][x]) {
        lineComplete = false;
        break;
      }
    }
    
    if (lineComplete) {
      linesCleared++;
      
      // Remove the line and shift everything down
      for (byte y2 = y; y2 > 0; y2--) {
        for (byte x = 0; x < BOARD_WIDTH; x++) {
          gameBoard[y2][x] = gameBoard[y2-1][x];
        }
      }
      
      // Clear the top line
      for (byte x = 0; x < BOARD_WIDTH; x++) {
        gameBoard[0][x] = 0;
      }
      
      // Line animation effect
      flashLine(y);
    }
  }
  
  // Update score based on lines cleared
  if (linesCleared > 0) {
    // Classic Tetris scoring: 40, 100, 300, 1200 for 1, 2, 3, 4 lines
    switch (linesCleared) {
      case 1: score += 40 * level; break;
      case 2: score += 100 * level; break;
      case 3: score += 300 * level; break;
      case 4: score += 1200 * level; break;  // Tetris!
    }
    
    // Increase level every 10 lines
    level = 1 + (score / 1000);
    
    // Speed up as level increases
    fallDelay = max(100, 800 - (level - 1) * 70);
  }
}

void flashLine(byte line) {
  uint8_t frame[8][12] = {0};
  
  // Copy current board to frame
  for (byte y = 0; y < BOARD_HEIGHT; y++) {
    for (byte x = 0; x < BOARD_WIDTH; x++) {
      frame[x][y] = gameBoard[y][x];
    }
  }
  
  // Flash the line
  for (byte i = 0; i < 3; i++) {
    // Clear the line
    for (byte x = 0; x < BOARD_WIDTH; x++) {
      frame[x][line] = 0;
    }
    matrix.renderBitmap(frame, 8, 12);
    delay(100);
    
    // Fill the line
    for (byte x = 0; x < BOARD_WIDTH; x++) {
      frame[x][line] = 1;
    }
    matrix.renderBitmap(frame, 8, 12);
    delay(100);
  }
}

void updateDisplay() {
  uint8_t frame[8][12] = {0};
  
  // Copy current board to frame
  for (byte y = 0; y < BOARD_HEIGHT; y++) {
    for (byte x = 0; x < BOARD_WIDTH; x++) {
      frame[x][y] = gameBoard[y][x];
    }
  }
  
  // Add current piece to frame
  for (byte y = 0; y < 4; y++) {
    for (byte x = 0; x < 4; x++) {
      if (TETROMINOS[currentPiece][currentRotation][y][x]) {
        byte boardY = currentY + y;
        byte boardX = currentX + x;
        if (boardY >= 0 && boardY < BOARD_HEIGHT && boardX >= 0 && boardX < BOARD_WIDTH) {
          frame[boardX][boardY] = 1;
        }
      }
    }
  }
  
  matrix.renderBitmap(frame, 8, 12);
}