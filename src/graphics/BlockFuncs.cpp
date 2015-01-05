#include "precompiled.hpp"

#include "BlockFuncs.hpp"
#include "../World.hpp"

namespace blocks
{

namespace graphics
{

bool BlockFuncs::isBlockVisible(ivec3 const &b)
{
	BlockType blockType = world->blockTypes.blockAt(b);
	switch (blockType)
	{
	case BlockType::NONE:
	case BlockType::AIR:
		return false;
	case BlockType::GROUND:
	case BlockType::GROUND2:
	case BlockType::COMPANION:
		return true;
	default:
	{
		LOG_ERR("Trying to get visibility from block which does not have one (blockType = ", (int) blockType);
		return false;
	}
	}
}

std::pair<glm::vec2, glm::vec2> BlockFuncs::getBlockTypeTexCoords(BlockType type)
{
	glm::vec2 texCoord;
	switch (type)
	{
	case BlockType::GROUND:
		texCoord = glm::vec2(0, 0);
		break;
	case BlockType::GROUND2:
		texCoord = glm::vec2(1/3., 0);
		break;
	case BlockType::COMPANION:
		texCoord = glm::vec2(2/3., 0);
		break;
	default:
		LOG_ERR("Trying to get tex coord from block which does not have a general one (blockType = ", (int) type, ")");
		texCoord =  glm::vec2(0, 0);
	}
	return std::pair<glm::vec2, glm::vec2>(texCoord, glm::vec2(1/3., 1));
}

std::pair<glm::vec2, glm::vec2> BlockFuncs::getBlockTexCoords(ivec3 const &b)
{
	return getBlockTypeTexCoords(world->blockTypes.blockAt(b));
}

}

}
