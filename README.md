"# stuff_with_ivan"




TODO:

- [x] Create VS proj
- [x] add lib (sdl or SFML)
- [x] draw sprite
- [x] restore prev progress from Go proj


- [x] Input for pad
    - [x] Limit movement to screen bounds
    - [x] Move pad by mouse
    - [x] Start game with click LMB
    - [x] Hide cursor
- [ ] Ball movement
    - [x] Collisions with walls
    - [x] Add horizontal velocity
    - [ ] Small randomness in direction on hit
    - [x] Collisions with bricks
    - [x] Increase speed on hit
    - [ ] Non linear collision with pad
- [ ] Bricks
    - [x] Hardcoded level with fixed amount of bricks
    - [x] Destroy on hit
    - [ ] Non destroyable brick
    - [ ] Brick with hp (more then 1 hit is required to destroy)
- [ ] Core
    - [ ] Destroy ball if it falls down 
    - [x] Display player live
    - [x] Display score
    - [x] 3 attempts - game over
    - [ ] All bricks destroyed - win
	- [x] Display Game Over message
- [ ] Extra
    - [ ] Sounds
    - [ ] Explosive brick
    - [ ] Brick can drop bonus. Additional life, player can collect up to X
    - [ ] Ball shouldn’t overlap with any other sprites
    - [ ] Ball should shrink on bounce with pad
