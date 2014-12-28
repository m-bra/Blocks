#ifndef LENTITYFUNCS_HPP_INCLUDED
#define LENTITYFUNCS_HPP_INCLUDED

#include <sstream>
#include "../vec.hpp"

#include "../World.hpp"
#include "Types.hpp"
#include "../Registerable.hpp"

namespace blocks
{

namespace physics {class Module;};

namespace logic
{

class EntityFuncs : public EntityListener, public WorldListener
{
private:
	class Module *logic;

	physics::Module *physics;
	World *world;
public:
	EntityFuncs(Module *a_module) : logic(a_module) {}

	void onWorldCreate(World *world);

	void onWorldDestroy() {}
	void onWorldUpdate(Time time) {}

	void onEntityCreate(int e, EntityArgs ls);
	void onEntityUpdate(int e, Time time);
	void onEntityDrop(int e);
	void onEntityTake(int e);
	void onEntityDestroy(int e) {}
	void onEntityArrayResize(int newsize) {}
};

}  // namespace logic

}

#endif /* ENTITYFUNCS_HPP_ */
