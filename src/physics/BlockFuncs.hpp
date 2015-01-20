#ifndef PHYSICS_BLOCKFUNCS_HPP_INCLUDED
#define PHYSICS_BLOCKFUNCS_HPP_INCLUDED

#ifndef PRECOMPILED_HPP_INCLUDED
#warning This header assumes "precompiled.hpp" to be #included
#include "precompiled.hpp"
#endif

#include "Module.hpp"

#include "physics/Types.hpp"

class btCompoundShape;

namespace blocks
{

namespace physics
{

class BlockFuncs : public Module
{
public:
	void onRegister() override;
	void addBlockShape(ivec3_c &c, ivec3_c &b, btCompoundShape *chunkShape);
};

}  // namespace physics

}

#endif /* BLOCKFUNCS_HPP_ */
