import serial
import keyboard
import time

arduino = serial.Serial('COM3', 115200, timeout=1)
key_map = {
    'a': ('A', 0.1), 'left': ('A', 0.1),   # Move Left
    'd': ('D', 0.1), 'right': ('D', 0.1),  # Move Right
    'w': ('W', 0.2), 'up': ('W', 0.2),     # Rotate
    's': ('S', 0.3), 'down': ('S', 0.3),   # Drop
    'r': ('R', 0.5), 'delete': ('R', 0.5)  # Reset
}

print("Use A/D/W/S or Arrow Keys to control Tetris. Press R or DEL to reset. Press ESC to exit.")

while True:
    try:
        for key, (command, delay) in key_map.items():
            if keyboard.is_pressed(key):
                arduino.write(command.encode())
                print(f"Sent: {command}")
                time.sleep(delay)
        if keyboard.is_pressed('esc'):
            break  # Exit on ESC
    except:
        break

arduino.close()
