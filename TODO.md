## Big Todos:
 * Add more game features (precise!)

## Medium Todos:
 * Fog!
 * Sky!
 * Optimize meshing: see glescraft
 * Restructure Code
   - Use Listeners
   - Change Chunk metrics from static to dynamic (-> change at runtime (like read from settings) possible)
   - some .hpp -> .cpp (faster compilation, better dependency-resolving)
   - Rename "Shared" to "World". those days are gone.
 * Add gui, text, hud (-> +crosshair)
 * Rename (rethinking, that is) "Registerable" to "Module"

 * Currently, when a block changes, onChunkChange() is called with the adjacent chunks two (if the block touches them).
   But it should be like this: call onBlockChange() on the module and it will do the check by itself!
 * make onGround() depend on entity (not just the player, yuk!) and move to physics provider
 * Many things, like creating the player entity, can happen after loading so -> onStart()

## Small Todos:
 * Kill entities fallen out of the world
 * don't let the user quit with altf4!!
 * fix randomness of generation
 * Currently, when the distance to hold the blocks is far away, the block jumps far away when picked up.
   Change this behaviour: when picked up, the holding distance is adjusted for the block not to jump away. (hope you get it)
 * I think, we don't need canMove() - moveLock already does it, doesn't it?
