#ifndef PHYSICS_BLOCKFUNCS_HPP_INCLUDED
#define PHYSICS_BLOCKFUNCS_HPP_INCLUDED

#ifndef PRECOMPILED_HPP_INCLUDED
#warning This header assumes "precompiled.hpp" to be #included
#endif

#include "../vec.hpp"
#include "../Registerable.hpp"
#include "Types.hpp"

class btCompoundShape;

namespace blocks
{

namespace physics
{

class BlockFuncs : public WorldListener, public Registerable
{
private:
	class World *world;
	class ThisClassDoesNotExistIHopeButIUseItForTestingWhetherWeCanStillPointToThisClass *lolTest;
public:
	void onWorldCreate(World *a_world) {world = a_world;}

	void addBlockShape(ivec3_c &c, ivec3_c &b, btCompoundShape *chunkShape);
};

}  // namespace physics

}

#endif /* BLOCKFUNCS_HPP_ */
