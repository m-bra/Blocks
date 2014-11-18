/*
 * OpenGLWrapper.hpp
 *
 *  Created on: Aug 30, 2014
 *      Author: merlin
 */

#ifndef OPENGLWRAPPER_HPP_
#define OPENGLWRAPPER_HPP_

#include <cstdlib>
#include <iostream>
#include <vector>
#include <GL/glew.h>

namespace gll
{

using Attribute = GLuint;
using Uniform = GLint;

void init();

inline void setDepthTest(bool enabled, GLenum func = GL_LEQUAL)
{
	(enabled ?glEnable :glDisable)(GL_DEPTH_TEST);
	glDepthFunc(func);
}

inline void setCulling(bool enabled, GLenum frontFace = GL_CCW)
{
	(enabled ?glEnable :glDisable)(GL_CULL_FACE);
	glFrontFace(frontFace);
	glCullFace(GL_BACK);
}

/**
 * Executes function if an error occurred since the last call or beginning of the program
 * Function has to have to parameters:
 * GLenum error // the error code
 * char const *errorStr // the name of the error
 * returns true if an error occurred, else false
 */
template <typename F>
inline bool onOpenGLErr(F fun)
{
	GLenum err = glGetError();
	bool isErr = err != GL_NO_ERROR;
	if (isErr)
		fun(err, gluErrorString(err));
	return isErr;
}

template <typename T>
struct OpenGLType
{
};

template <>
struct OpenGLType<float>
{
	static constexpr GLenum type = GL_FLOAT;
};

template <>
struct OpenGLType<double>
{
	static constexpr GLenum type = GL_DOUBLE;
};

template <>
struct OpenGLType<unsigned char>
{
	static constexpr GLenum type = GL_UNSIGNED_BYTE;
};

template <>
struct OpenGLType<char>
{
	static constexpr GLenum type = GL_BYTE;
};

}  // namespace gll

#include "Program.hpp"
#include "Buffer.hpp"
#include "VertexArray.hpp"

#endif /* OPENGLWRAPPER_HPP_ */
