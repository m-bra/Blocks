#ifndef BLOCKFUNCS_HPP_
#define BLOCKFUNCS_HPP_

#include <iostream>
#include <glm/glm.hpp>

#include "../Logger.hpp"
#include "../vec.hpp"

#include "../World.hpp"
#include "../Registerable.hpp"

#include "Types.hpp"

namespace blocks
{

namespace graphics
{

class BlockFuncs : public WorldListener
{
private:
	World *world;
public:
	void onWorldCreate(World *a_world)
	{
		world = a_world;
	}

	void onWorldDestroy() {}
	void onWorldUpdate(Time time) {}

	bool isBlockVisible(ivec3 const &b)
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
			std::stringstream ss;
			ss << "Trying to get visibility from block which does not have one (blockType = " << (int) blockType << ")";
			Log::error(ss);
			return false;
		}
		}
	}

	std::pair<glm::vec2, glm::vec2> getBlockTypeTexCoords(BlockType type)
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
			std::stringstream ss;
			ss << "Trying to get tex coord from block which does not have a general one (blockType = "
				<< (int) type << ")";
			Log::error(ss);
			texCoord =  glm::vec2(0, 0);
		}
		return std::pair<glm::vec2, glm::vec2>(texCoord, glm::vec2(1/3., 1));
	}

	std::pair<glm::vec2, glm::vec2> getBlockTexCoords(ivec3 const &b)
	{
		return getBlockTypeTexCoords(world->blockTypes.blockAt(b));
	}
};

}  // namespace graphics
}

#endif /* BLOCKFUNCS_HPP_ */
