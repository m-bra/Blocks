## Big Todos:
 * Add more game features (precise!)
 * Add gui, text, hud (-> +crosshair)

## Medium Todos:
 * Fog!
 * Sky!
 * Antialias!
 * Optimize meshing: see glescraft

 * Currently, when a block changes, onChunkChange() is called with the adjacent chunks two (if the block touches them).
   But it should be like this: call onBlockChange() on the module and it will do the check by itself!
 * make onGround() depend on entity (not just the player, yuk!) and move to physics provider
 * Use bullet and glew with addSubDirectory()
 * saving and loading
 * remove those randomly generated mysterious staircases 

## Small Todos:
 * don't let the user quit with altf4!!
 * Currently, when the distance to hold the blocks is far away, the block jumps far away when picked up.
   Change this behaviour: when picked up, the holding distance is adjusted for the block not to jump away. (hope you get it)
 * blocks stutter when placed and picked up
