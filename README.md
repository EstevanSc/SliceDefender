# Slice Defender
This project is a Fruit Ninja-like game using hand detection(if camera is used), it has been made using OpenGL,OpenCV and QT librairies.

## Authors
- Aubin SIONVILLE
- Estevan SCHMITT

## Purpose
Slice Defender is an interactive 3D game developed as part of a Computer Science course at Télécom Saint-Étienne. The player must slice flying projectiles (fruits and vegetables) using a virtual sword. The game combines real-time hand tracking (via webcam) and OpenGL rendering to create an immersive experience. The objective is to score points by slicing as many projectiles as possible while avoiding missing them.

## Instructions

### How to Play
1. **Start the game**: Launch the application. The main window will display the game area and UI.
2. **Camera setup**: The game uses your webcam to track your hand position. Make sure your camera is connected and working.
3. **Sword control**: Move your hand in front of the camera to control the sword in the game. The sword follows your hand's position.
4. **Slice projectiles**: Projectiles (apples, oranges, bananas, corn, strawberries) are launched from the cannon. Slice them with your sword before they fall.
5. **Scoring**: Each successful slice gives you points. Missing projectiles may cost you lives.
6. **Game modes**: You can switch between standard and original game modes for different levels of challenge.
7. **Restart**: Use the UI buttons to restart the game or view your score.

### Controls
- **Hand movement**: Controls the sword (requires a webcam).
- **Keyboard**: Some features may be accessible via keyboard (see in-game instructions).

## Implementation Details

### Main Components
- **projectileManager.h / .cpp**: Manages all projectiles in the game. Handles creation, launching, updating, drawing, and slicing of projectiles. Uses a simple random generator for projectile types and trajectories.
- **projectile.h / .cpp**: Abstract base class for all projectiles. Defines physics, collision, slicing, and rendering logic. Specialized projectiles (Apple, Orange, Banana, Corn, Strawberry) inherit from this class.
- **projectiles/**: Contains all specific projectile types and their sliced halves (e.g., `apple.h`, `bananaHalf.h`). Each type implements its own drawing and slicing behavior.
- **CameraHandler.h / .cpp**: Handles webcam input and hand detection using OpenCV. Tracks the player's hand position and provides it to the game logic.
- **player.h / .cpp**: Represents the player's sword. Handles drawing and positioning in the 3D world.
- **game.h / .cpp**: Main game controller. Manages game state, scoring, lives, and player input.
- **myglwidget.h / .cpp**: OpenGL rendering widget. Draws the game scene, including the cannon, grid, projectiles, and sword.
- **mainwindow.h / .cpp**: Main application window. Integrates all components, manages UI, and handles user interactions.

### Resources
- **res/**: Contains textures for projectiles.
- **.qrc**: Qt resource file for loading images.

### Build & Run
1. Open the project in Qt Creator (or your preferred Qt/C++ IDE).
2. Make sure you have OpenCV and Qt installed.
3. Build the project.
4. Run the executable. Allow camera access if prompted.

## Notes
- The game requires a webcam for hand tracking.
- All projectiles are implemented as C++ classes with clear separation between logic and rendering.
- The code is modular and documented for educational purposes.
- Hand detection would work better with a white background, good lighting conditions and with fist.