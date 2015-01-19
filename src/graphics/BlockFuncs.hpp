#ifndef GRAPHICS_BLOCKFUNCS_HPP_INCLUDED
#define GRAPHICS_BLOCKFUNCS_HPP_INCLUDED

#ifndef PRECOMPILED_HPP_INCLUDED
#warning This header assumes "precompiled.hpp" to be #included
#include "precompiled.hpp"
#endif

#include <glm/glm.hpp>
#include "Module.hpp"

namespace blocks
{

namespace graphics
{

class BlockFuncs : public Module
{
public:
	bool isBlockVisible(ivec3 const &b);
	std::pair<glm::vec2, glm::vec2> getBlockTypeTexCoords(BlockType type);
	std::pair<glm::vec2, glm::vec2> getBlockTexCoords(ivec3 const &b);
};

}  // namespace graphics
}

#endif /* BLOCKFUNCS_HPP_ */
