# Arcane Tectonics
## Capricious Croissants
Annie Hughey, Alejandro Devore-Oviedo, Brandon Walker, Joseph Ienna, Rowena Chen, Tyler Thompson

## Alpha Release
### Supported Platforms
- Android
- iOS
- Mac
- PC

### Gameplay
Mika is the girl character whom the player is controlling.

#### Win Condition
Player (Mika) destroys all the enemies on the board.

#### Lose Condition
Player (Mika) gets destroyed.

#### Destroy Enemies
Create a match of three or more tiles in a straight line to destroy them. Any enemies standing on a matched tile are destroyed.

#### Game Loop
Player (Mika) and enemies alternate turns.

- Player can slide a row/column of tiles. Do NOT need to make a match with the slide.
- Enemies can move and attack anything in range.

#### Enemies
All enemies only have melee attacks, so they attack Mika by moving onto the same tile as her.

__Skeletons__ - smart enemies who always move toward Mika.

__Plump Purple Cats__ - dumb enemies who move in the direction they're facing and turn around when they hit an obstacle.