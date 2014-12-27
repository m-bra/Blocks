#ifndef GENTITYFUNCS_HPP_
#define GENTITYFUNCS_HPP_

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "../vec.hpp"

#include "../Registerable.hpp"

#include "../World.hpp"
#include "../logic/Module.hpp"
#include "BlockFuncs.hpp"

namespace blocks
{

namespace graphics
{

class EntityFuncs : public WorldListener, public EntityListener, public Registerable
{
private:
	logic::Module *logic;
	class Module *graphics;
	World *world;

	BlockFuncs<Shared> *blockFuncs;
	GLuint blockTbo;
public:
	EntityFuncs(class Module *a_module) {graphics = a_module;}

	void setBlockFuncs(BlockFuncs *a_blockFuncs) {blockFuncs = a_blockFuncs;}
	void onWorldCreate(World *a_world) {world = a_world;}
	void onWorldDestroy() {}
	void onWorldUpdate(Time time) {}

	void onRegister(World *world)
	{
		logic = world->getFirstRegisterableByType();
		assert(logic);
	}

	void onEntityCreate(int e, EntityArgs args);
	void onEntityDestroy(int e) {}
	void onEntityArrayResize(int newsize) {}
	void onEntityUpdate(int e, Time time) {}

	int putVertices(GLuint vbo, int e);
};

}  // namespace graphics
}


#endif /* ENTITYFUNCS_HPP_ */
