/*
 * Buffer.hpp
 *
 *  Created on: Sep 7, 2014
 *      Author: merlin
 */

#ifndef BUFFER_HPP_
#define BUFFER_HPP_

namespace gll
{

class Buffer
{
private:
	GLuint vbo;
public:
	inline void create()
	{
		glGenBuffers(1, &vbo);
	}

	inline void destroy()
	{
		glDeleteBuffers(1, &vbo);
	}

	inline void bind(GLenum target)
	{
		glBindBuffer(target, vbo);
	}

	template <typename T>
	static inline void setData(GLenum target, T *pointer, int count, GLenum usage)
	{
		glBufferData(target, count * sizeof (T), pointer, usage);
	}
};

}

#endif /* BUFFER_HPP_ */
