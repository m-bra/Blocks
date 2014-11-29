/*
 * BlockFuncs.hpp
 *
 *  Created on: Sep 30, 2014
 *      Author: merlin
 */

#ifndef BLOCKFUNCS_HPP_
#define BLOCKFUNCS_HPP_

#include <iostream>
#include <glm/glm.hpp>

#include "../Logger.hpp"
#include "../vec.hpp"

#include "../SharedTypes.hpp"
#include "Types.hpp"

namespace graphics
{

template <typename Shared>
class BlockFuncs
{
private:
	Shared *shared;
public:
	BlockFuncs(Shared *world)
		: shared(world)
	{

	}

	bool isBlockVisible(ivec3 const &b)
	{
		BlockType blockType = shared->blockTypes.blockAt(b);
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
		return getBlockTypeTexCoords(shared->blockTypes.blockAt(b));
	}
};

}  // namespace graphics

#endif /* BLOCKFUNCS_HPP_ */
