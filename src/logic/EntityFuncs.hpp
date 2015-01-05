#ifndef LOGIC_ENTITYFUNCS_HPP_INCLUDED
#define LOGIC_ENTITYFUNCS_HPP_INCLUDED

#ifndef PRECOMPILED_HPP_INCLUDED
#warning This header assumes "precompiled.hpp" to be #included
#endif

#include "../vec.hpp"
#include "Types.hpp"
#include "../Registerable.hpp"

namespace blocks
{

namespace physics {class Module;};

namespace logic
{

class EntityFuncs : public EntityListener, public WorldListener, public Registerable
{
private:
	class Module *logic;

	physics::Module *physics;
	class World *world;
public:
	void onRegister(World *world);
	void onWorldCreate(World *world);

	void onEntityCreate(int e, EntityArgs ls);
	void onEntityUpdate(int e, Time time);
	void onEntityDrop(int e);
	void onEntityTake(int e);
};

}  // namespace logic

}

#endif /* ENTITYFUNCS_HPP_ */
