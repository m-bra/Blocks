#ifndef WORLDTYPES_HPP_
#define WORLDTYPES_HPP_

using Time = float;

enum class BlockType : unsigned char
{
	NONE, OUTSIDE, AIR, GROUND, GROUND2, COMPANION
};

enum class EntityType : unsigned char
{
	NONE, BLOCK, BOT
};

#endif /* WORLDTYPES_HPP_ */
