#ifndef GRAPHICS_ENTITYFUNCS_HPP_INCLUDED
#define GRAPHICS_ENTITYFUNCS_HPP_INCLUDED

#ifndef PRECOMPILED_HPP_INCLUDED
#warning This header assumes "precompiled.hpp" to be #included
#include "precompiled.hpp"
#endif

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Module.hpp"

namespace blocks
{

class World;

namespace logic {class DefaultLogic;}

namespace graphics
{


class EntityFuncs : public Module
{
private:
	logic::DefaultLogic *logic;
	class DefaultGraphics *graphics;
	class BlockFuncs *blockFuncs;

	GLuint blockTbo;
public:
	void onRegister() override;
	void onEntityCreate(Entity e, std::vector<BaseEntityArgs *> const &args) override;
};

}  // namespace graphics
}


#endif /* ENTITYFUNCS_HPP_ */
