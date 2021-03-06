#ifndef GRAPHICS_TYPES_HPP_INCLUDED
#define GRAPHICS_TYPES_HPP_INCLUDED

#ifndef PRECOMPILED_HPP_INCLUDED
#warning This header assumes "precompiled.hpp" to be #included
#endif

#include <GL/glew.h>

namespace blocks
{

namespace graphics
{
using VertexComponent = float;

struct Vertex
{
	VertexComponent x, y, z, u, v, nx, ny, nz;
};

struct ChunkGraphics
{
	bool dirty, created;
	GLuint vbo, vao;
	unsigned vertCount;

	void create()
	{
		glGenBuffers(1, &vbo);
		glGenVertexArrays(1, &vao);
		created = true;
		dirty = false;
		vertCount = 0;
	}

	void destroy()
	{
		glDeleteBuffers(1, &vbo);
		glDeleteVertexArrays(1, &vao);
		created = false;
	}
};

struct EntityGraphics
{
	bool created = false;
	GLuint vbo, vao, tbo;
	unsigned vertCount;
};
}
}

#endif /* TYPES_HPP_ */
