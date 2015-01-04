#ifndef LOGIC_TYPES_HPP_INCLUDED
#define LOGIC_TYPES_HPP_INCLUDED

#ifndef PRECOMPILED_HPP_INCLUDED
#warning This header assumes "precompiled.hpp" to be #included
#endif

#include <cstring>
#include "../vec.hpp"
#include "../WorldTypes.hpp"

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
