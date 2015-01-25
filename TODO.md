## Add
 * game logic
 * gui, text, hud (-> +crosshair)
 * Fog!
 * Sky!
 * Antialias!
 * saving and loading

## Enhance
 * Optimize meshing: see glescraft
 * switch from GLEW to glbinding BECAUSE IT IS FREAKING AWESOME

## Fix
 * Currently, when a block changes, onChunkChange() is called with the adjacent chunks two (if the block touches them).
   But it should be like this: call onBlockChange() on the module and it will do the check by itself!
 * remove those randomly generated mysterious staircases
 * don't let the user quit with altf4!!
 * Currently, when the distance to hold the blocks is far away, the block jumps far away when picked up.
   Change this behaviour: when picked up, the holding distance is adjusted for the block not to jump away. (hope you get it)
 * blocks stutter when placed and picked up
