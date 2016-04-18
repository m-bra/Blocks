## Features
 * game logic
 * gui, text, hud (-> +crosshair)
 * Fog!
 * Sky!
 * Antialias!
 * saving and loading

## Internal
 * Optimize meshing: see glescraft
 * switch from GLEW to glbinding BECAUSE IT IS FREAKING AWESOME
 * modularize voxel generator
   * use/add polyvoxel
 * add speed cap/limit

## Fix
 * remove those randomly generated mysterious staircases
 * don't let the user quit with altf4!!
 * Currently, when the distance to hold the blocks is far away, the block jumps far away when picked up.
   Change this behaviour: when picked up, the holding distance is adjusted for the block not to jump away. (hope you get it)
 * blocks stutter when placed and picked up (especially when there are big chunks) (can be fixed by making chunks smaller)
 * Sometimes when picking up, especially the first time, the player seems to pick up itself;
   he jumps a bit forward doing it and the blocks is not grabbed
