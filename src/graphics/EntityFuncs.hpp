#ifndef GRAPHICS_ENTITYFUNCS_HPP_INCLUDED
#define GRAPHICS_ENTITYFUNCS_HPP_INCLUDED

#ifndef PRECOMPILED_HPP_INCLUDED
#warning This header assumes "precompiled.hpp" to be #included
#endif

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "../vec.hpp"
#include "../Registerable.hpp"

namespace blocks
{

namespace logic {class Module;}

class World;

namespace graphics
{

class EntityFuncs : public WorldListener, public EntityListener, public Registerable
{
private:
	logic::Module *logic;
	class Module *graphics;
	class BlockFuncs *blockFuncs;
	World *world;

	GLuint blockTbo;
public:
	void onWorldCreate(World *a_world) {world = a_world;}
	void onWorldDestroy() {}
	void onWorldUpdate(Time time) {}

	void onRegister(World *world);

	void onEntityCreate(int e, EntityArgs args);
	void onEntityDestroy(int e) {}
	void onEntityArrayResize(int newsize) {}
	void onEntityUpdate(int e, Time time) {}

	int putVertices(GLuint vbo, int e);
};

}  // namespace graphics
}


#endif /* ENTITYFUNCS_HPP_ */
