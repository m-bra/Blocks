#ifndef LOGIC_ENTITYFUNCS_HPP_INCLUDED
#define LOGIC_ENTITYFUNCS_HPP_INCLUDED

#ifndef PRECOMPILED_HPP_INCLUDED
#warning This header assumes "precompiled.hpp" to be #included
#include "precompiled.hpp"
#endif

#include "logic/Types.hpp"
#include "Module.hpp"

namespace blocks
{

namespace physics {class BulletPhysics;}

namespace logic
{

class EntityFuncs : public Module
{
private:
	class DefaultLogic *logic;

	physics::BulletPhysics *physics;
public:
	void onRegister() override;
	void onUpdate(GameTime gtime) override;

	void onEntityCreate(Entity e, EntityArgs ls) override;
	void onEntityDrop(Entity e, int slot, Entity holder) override;
	void onEntityTake(Entity e, int slot, Entity holder) override;
	void onEntityPlace(Entity e, int slot, Entity holder) override;
};

}  // namespace logic

}

#endif /* ENTITYFUNCS_HPP_ */
