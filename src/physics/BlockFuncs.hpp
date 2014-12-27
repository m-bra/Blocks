#ifndef PBLOCKFUNCS_HPP_INCLUDED
#define PBLOCKFUNCS_HPP_INCLUDED

#include "btBulletDynamicsCommon.h"
#include "../vec.hpp"
#include "../World.hpp"
#include "../Registerable.hpp"
#include "Types.hpp"

namespace blocks
{

namespace physics
{

class BlockFuncs : public WorldListener
{
private:
	World *world;
public:
	void onWorldCreate(World *world) {this->world = world;}
	void onWorldDestroy() {}
	void onWorldUpdate(Time time) {}

	void addBlockShape(ivec3_c &c, ivec3_c &b, btCompoundShape *chunkShape)
	{
		static btBoxShape box(btVector3(.5, .5, .5));
		// b relative to its chunk
		ivec3 b_c = b - c * world->size;

		BlockType &type = world->blockTypes.blockAt(b);
		switch (type)
		{
		case BlockType::GROUND:
		case BlockType::GROUND2:
		case BlockType::COMPANION:
			chunkShape->addChildShape(btTransform(btQuaternion(0, 0, 0, 1), (b_c + .5).bt()), &box);
			break;
		case BlockType::AIR:
			break;
		default:
		{
			std::stringstream ss;
			ss << "Trying to get shape of block which does not have one (blockType = " << (int) type << ")";
			Log::error(ss);
		}
		}
	}
};

}  // namespace physics

}

#endif /* BLOCKFUNCS_HPP_ */
