#include "precompiled.hpp"

#include "graphics/BlockFuncs.hpp"

namespace blocks
{

namespace graphics
{

bool BlockFuncs::isBlockVisible(ivec3_c &b)
{
	BlockType blockType = world->blockTypes.blockAt(b);
	return blockType != world->blockType.none && blockType != world->blockType.air;
}

std::pair<glm::vec2, glm::vec2> BlockFuncs::getBlockTypeTexCoords(BlockType type)
{
	glm::vec2 texCoord;
	if (type == world->blockType.ground)
		texCoord = glm::vec2(0, 0);
	else if (type == world->blockType.ground2)
		texCoord = glm::vec2(1/3., 0);
	else if (type == world->blockType.companion)
		texCoord = glm::vec2(2/3., 0);
	else
	{
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
