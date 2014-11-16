/*
 * VertexArray.hpp
 *
 *  Created on: Sep 7, 2014
 *      Author: merlin
 */

#ifndef VERTEXARRAY_HPP_
#define VERTEXARRAY_HPP_

namespace gll
{

class VertexArray
{
private:
	GLuint vao;
public:
	inline void create()
	{
		glGenVertexArrays(1, &vao);
	}

	inline void destroy()
	{
		glDeleteVertexArrays(1, &vao);
	}

	inline void bind()
	{
		glBindVertexArray(vao);
	}

	static inline void setAttribute(int index, int size, GLenum type, bool normalized, int stride, int offset)
	{
		glEnableVertexAttribArray(index);
		glVertexAttribPointer(index, size, type, normalized, stride, reinterpret_cast<void *>(offset));
	}
};

}

#endif /* VERTEXARRAY_HPP_ */
