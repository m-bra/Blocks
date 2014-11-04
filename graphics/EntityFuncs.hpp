/*
 * EntityFuncs.hpp
 *
 *  Created on: Oct 12, 2014
 *      Author: merlin
 */

#ifndef GENTITYFUNCS_HPP_
#define GENTITYFUNCS_HPP_

#include <GL/glew.h>
#include <SFML/Graphics.hpp>
#include "../vec.hpp"

#include "../SharedTypes.hpp"
#include "Types.hpp"
#include "BlockFuncs.hpp"

namespace graphics
{

template <typename Shared>
class EntityFuncs
{
private:
	Shared *shared;
	BlockFuncs<Shared> *blockFuncs;
	GLuint blockTbo;
public:
	EntityFuncs(Shared *shared, BlockFuncs<Shared> *blockFuncs);

	void onCreate(int e)
	{
		shared->graphics.entityGraphics[e].tbo = blockTbo;
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

template <typename Shared>
EntityFuncs<Shared>::EntityFuncs(Shared *world, BlockFuncs<Shared> *blockFuncs) : shared(world), blockFuncs(blockFuncs)
{
	sf::Image blockImg;
	if (!blockImg.loadFromFile("chunk.png"))
	{
		Log::error("Cannot open 'chunk.png'");
		exit(EXIT_FAILURE);
	}

	glGenTextures(1, &blockTbo);
	glBindTexture(GL_TEXTURE_2D, blockTbo);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, blockImg.getSize().x, blockImg.getSize().y, 0,
		GL_RGBA, GL_UNSIGNED_BYTE, blockImg.getPixelsPtr());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
}

}  // namespace graphics



#endif /* ENTITYFUNCS_HPP_ */
