#ifndef LENTITYFUNCS_HPP_INCLUDED
#define LENTITYFUNCS_HPP_INCLUDED

#include <sstream>
#include "../vec.hpp"

#include "../World.hpp"
#include "Types.hpp"
#include "../Registerable.hpp"

namespace blocks
{

namespace logic
{

class EntityFuncs : public EntityListener, public WorldListener
{
private:
	class Module *logic;
	class physics::Module *physics;
	World *world;
public:
	EntityFuncs(Module *a_module) : logic(a_module) {}

	void onWorldCreate(World *world);

	void onWorldDestroy() {}
	void onWorldUpdate(Time time) {}

	void onEntityCreate(int e, EntityArgs ls);
	void onEntityUpdate(int e, Time time);

	void onEntityDrop(int e)
	{
		// assert type == BLOCK
		EntityLogics::BlockEntity &data = shared->logic.entityLogics[e].blockEntity;
		data.moveToTarget = false;
	}

	void onEntityTake(int e)
	{
		logic::EntityLogics::BlockEntity &data = shared->logic.entityLogics[e].blockEntity;
		data.moveToTarget = true;
		data.fixTime = -1;
	}

	void onEntityDestroy(int e) {}
	void onEntityArrayResize(int newsize) {}
};

}  // namespace logic

}

#endif /* ENTITYFUNCS_HPP_ */
