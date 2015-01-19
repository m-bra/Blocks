#ifndef LOGIC_TYPES_HPP_INCLUDED
#define LOGIC_TYPES_HPP_INCLUDED

#ifndef PRECOMPILED_HPP_INCLUDED
#warning This header assumes "precompiled.hpp" to be #included
#include "precompiled.hpp"
#endif

#include <cstring>

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

	union
	{
	BlockEntity blockEntity;
	Bot bot;
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
