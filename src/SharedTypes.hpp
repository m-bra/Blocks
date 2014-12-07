#ifndef WORLDTYPES_HPP_
#define WORLDTYPES_HPP_

#include <map>
#include <cstdint>

namespace blocks
{

using Time = float;
using EntityArgs = std::map<char const *, intptr_t>;

enum class BlockType : unsigned char
{
	NONE, OUTSIDE, AIR, GROUND, GROUND2, COMPANION
};

enum class EntityType : unsigned char
{
	NONE, PLAYER, BLOCK, BOT
};

}

#endif /* WORLDTYPES_HPP_ */
