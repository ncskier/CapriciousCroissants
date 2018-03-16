# Cornell University Game Library (CUGL)

This template provides a base for you to build off of.  You should add all of your
files to sources and assets, as appropriate.  This will allow access to the Apple,
Android, and Windows IDEs.

You will need to make changes to the project to give it a different name and package
identifier (to prevent your application from clashing with others).  See the course
web page for instructions

## Technical Prototype
### Supported platforms
- Android
- iOS
- Mac
- PC

### APK Location
`CapriciousCroissants/build-android/app/build/outputs/apk/debug/apk-debug.apk`

### Settings Menu
The settings menu is for testing purposes; it will not be part of the game.
At the moment, the "Place Player's Pawns?" checkbox does not do anything.

### Gameplay
- The grey squares are your allies, who you want to protect.
- The black squares with red dots are your enemies, the ones you want to destroy.
- Make a match underneath an enemy to destroy it.
- After you make a move, each enemy will step one square in the direction it is
facing. Enemies face in the direction of their red dot.
- If an enemy steps on an ally, the ally will be destroyed.
- Destroy all the enemies to win.
- If all your allies are destroyed, you lose.
