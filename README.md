# PACMAN game for hardware engineering course

PacMan is a maze game. The player controls the Pacman character through an enclosed maze; the objective of the game is to eat all of the dots (coins) placed in the maze while avoiding some ghosts that pursue him. When all of the dots are eaten, the player advances to the next level. There are two levels with different maps.
Placed at the two corners of the maze are large coins named "Freeze". Eating these will cause the ghosts to stop from moving and Pacman can pass over them without losing a life.
The game increases in difficulty as the player progresses; the map will be changed, the number of ghosts becomes increase, the lives decrease and the ghosts produce from more places.  
The main requirements for the game to be advanced are as follows: 
- The game supports high score lists.
- It supports different modes of playing the game such as: easy, medium and hard.
- The user can be able to control the game with buttons on the ChipKIT board.
- The different maps were designed to raise the difficulty.

This project is developed on the ChipKIT Uno32 board together with the Basic I/O shield. We used interrupts triggered by the built in timer to update the screen and control the speed of the game. All the development was done using the MCB32tools and all code is written in C.
