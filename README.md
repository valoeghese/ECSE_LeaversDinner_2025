# ECSE Leavers Dinner
## Tetris Edition

An Rpi-2040 based microcontroller board with a 5x5 pixel display and two push buttons was given out as an ID card for the Department of ECSE at the University of Auckland's 2025 leaver's dinner. As the code was public on GitHub, I have forked and modified it to enable playing the game 'Tetris'.

## Summary of Changes

- Polling moved to second core of the microcontroller so it doesn't get blocked by rendering to the display
- Abstraction for 'behaviours' of the board created
- Existing behaviour made into a "default" behaviour
- Introduction of a new "tetris" behaviour which plays Tetris (explained below)
- In default behaviour, PB1 now enters *Tetris* mode, and PB2 toggles between the two messages. (Base code had each button switch to a different message.)

## Tetris Mode

Press Push Button 1 (GPIO 15) to play tetris.

Blocks will fall from the sky in one of 4 shapes. You can move and rotate the block before it solifies upon touching the ground, or existing structure.

User Inputs in *Tetris* mode:
- Push Button 1 (GPIO 15): Rotate block anticlockwise
- Push Button 2 (GPIO 14): Shift block left

The inputs will skip any illegal positions for the block, so the block cannot fall onto nothing.

Upon losing tetris, the game will switch back into *Default* mode.

## Known bugs
- It is possible to get the line to solidify 1 block above the ground
- Blocks will fall through the top layer instead of causing an immediate game over, if they wouldn't otherwise solidify