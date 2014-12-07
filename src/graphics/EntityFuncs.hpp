/*
 * EntityFuncs.hpp
 *
 *  Created on: Oct 12, 2014
 *      Author: merlin
 */

#ifndef GENTITYFUNCS_HPP_
#define GENTITYFUNCS_HPP_

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "../vec.hpp"

#include "../WorldListener.hpp"

#include "../SharedTypes.hpp"
#include "Types.hpp"
#include "BlockFuncs.hpp"

namespace blocks
{

namespace graphics
{

template <typename Shared>
class EntityFuncs : public WorldListener
{
private:
	Shared *shared;
	BlockFuncs<Shared> *blockFuncs;
	GLuint blockTbo;
public:
	void setBlockFuncs(BlockFuncs<Shared> *blockFuncs) {this->blockFuncs = blockFuncs;}
	void onWorldCreate(Shared *shared) {this->shared = shared;}
	void onWorldDestroy() {}
	void onWorldUpdate(Time time) {}

	void onCreate(int e)
	{
		shared->graphics.entityGraphics[e].tbo = shared->graphics.chunkTbo;
	}

	void onDestroy(int e)
	{

	}

	int putVertices(GLuint vbo, int e)
	{
		EntityType const &type = shared->entityTypes[e];
		logic::EntityLogics &data = shared->logic.entityLogics[e];

		BlockType texType;
		switch (type)
		{
		case EntityType::BLOCK:
			texType = data.blockEntity.blockType;
			break;
		case EntityType::BOT:
			texType = BlockType::COMPANION;
			break;
		default:
			std::stringstream ss;
			ss << "Trying to get vertices of entity which does not have any (type = " << (int) type << ")";
			Log::error(ss);
			break;
		}
		auto t = blockFuncs->getBlockTypeTexCoords(texType);

		float tx0 = t.first.x;
		float tx1 = t.first.x + t.second.x;
		float ty0 = t.first.y;
		float ty1 = t.first.y + t.second.y;
		float cube_verts[] = {
			// left
			-.5, -.5, -.5,  tx0, ty0,  -1, 0, 0,
			-.5, +.5, +.5,  tx1, ty1,  -1, 0, 0,
			-.5, +.5, -.5,  tx1, ty0,  -1, 0, 0,
			-.5, -.5, -.5,  tx0, ty0,  -1, 0, 0,
			-.5, -.5, +.5,  tx0, ty1,  -1, 0, 0,
			-.5, +.5, +.5,  tx1, ty1,  -1, 0, 0,

			// right
			+.5, -.5, -.5,  tx0, ty0,  +1, 0, 0,
			+.5, +.5, -.5,  tx1, ty0,  +1, 0, 0,
			+.5, +.5, +.5,  tx1, ty1,  +1, 0, 0,
			+.5, -.5, -.5,  tx0, ty0,  +1, 0, 0,
			+.5, +.5, +.5,  tx1, ty1,  +1, 0, 0,
			+.5, -.5, +.5,  tx0, ty1,  +1, 0, 0,

			// bottom
			-.5, -.5, -.5,  tx0, ty0,  0, -1, 0,
			+.5, -.5, -.5,  tx1, ty0,  0, -1, 0,
			+.5, -.5, +.5,  tx1, ty1,  0, -1, 0,
			-.5, -.5, -.5,  tx0, ty0,  0, -1, 0,
			+.5, -.5, +.5,  tx1, ty1,  0, -1, 0,
			-.5, -.5, +.5,  tx0, ty1,  0, -1, 0,

			// top
			-.5, +.5, -.5,  tx0, ty0,  0, +1, 0,
			+.5, +.5, +.5,  tx1, ty1,  0, +1, 0,
			+.5, +.5, -.5,  tx1, ty0,  0, +1, 0,
			-.5, +.5, -.5,  tx0, ty0,  0, +1, 0,
			-.5, +.5, +.5,  tx0, ty1,  0, +1, 0,
			+.5, +.5, +.5,  tx1, ty1,  0, +1, 0,

			// back
			-.5, -.5, -.5,  tx0, ty0,  0, 0, -1,
			+.5, +.5, -.5,  tx1, ty1,  0, 0, -1,
			+.5, -.5, -.5,  tx1, ty0,  0, 0, -1,
			-.5, -.5, -.5,  tx0, ty0,  0, 0, -1,
			-.5, +.5, -.5,  tx0, ty1,  0, 0, -1,
			+.5, +.5, -.5,  tx1, ty1,  0, 0, -1,

			// front
			-.5, -.5, +.5,  tx0, ty0,  0, 0, +1,
			+.5, -.5, +.5,  tx1, ty0,  0, 0, +1,
			+.5, +.5, +.5,  tx1, ty1,  0, 0, +1,
			-.5, -.5, +.5,  tx0, ty0,  0, 0, +1,
			+.5, +.5, +.5,  tx1, ty1,  0, 0, +1,
			-.5, +.5, +.5,  tx0, ty1,  0, 0, +1,
		};
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof (cube_verts), cube_verts, GL_STATIC_DRAW);
		return sizeof (cube_verts) / sizeof (float);
	}
};

}  // namespace graphics
}


#endif /* ENTITYFUNCS_HPP_ */
