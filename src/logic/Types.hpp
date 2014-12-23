/*
 * Types.hpp
 *
 *  Created on: Oct 17, 2014
 *      Author: merlin
 */

#ifndef LTYPES_HPP_
#define LTYPES_HPP_

#include <cstring>
#include "../vec.hpp"
#include "../SharedTypes.hpp"

namespace blocks
{

namespace logic
{

struct EntityLogics
{
	bool created;

	struct BlockEntity
	{
		BlockType blockType;
		fvec3 target;
		bool moveToTarget;
		float fixTime, unactiveTime;
	};

	struct Bot
	{
		fvec3 dir;
		float nextChange;
	};

	struct Player
	{
		int heldEntity;
		float holdDistance;
	};

	union
	{
	BlockEntity blockEntity;
	Bot bot;
	Player player;
	};

	EntityLogics()
	{
		memset(this, 0, sizeof (EntityLogics));
	}
	EntityLogics(EntityLogics const &other)
	{
		memcpy(this, &other, sizeof (EntityLogics));
	}
	EntityLogics &operator =(EntityLogics const &other)
	{
		memcpy(this, &other, sizeof (EntityLogics));
		return *this;
	}
};
}
}


#endif /* TYPES_HPP_ */
