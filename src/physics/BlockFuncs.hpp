/*
 * BlockFuncs.hpp
 *
 *  Created on: Oct 13, 2014
 *      Author: merlin
 */

#ifndef PBLOCKFUNCS_HPP_
#define PBLOCKFUNCS_HPP_

#include "../vec.hpp"
#include "../SharedTypes.hpp"
#include "Types.hpp"

namespace blocks
{

namespace physics
{

template <typename Shared>
class BlockFuncs
{
private:
	Shared *shared;
public:
	BlockFuncs(Shared *shared) : shared(shared) {};

	void addBlockShape(ivec3::cref c, ivec3::cref b, btCompoundShape *chunkShape)
	{
		static btBoxShape box(btVector3(.5, .5, .5));
		// b relative to its chunk
		ivec3 b_c = b - c * ivec3(Shared::CSIZE_X, Shared::CSIZE_Y, Shared::CSIZE_Z);

		BlockType &type = shared->blockTypes.blockAt(b);
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
