/*
 * Program.hpp
 *
 *  Created on: Aug 31, 2014
 *      Author: merlin
 */

#ifndef PROGRAM_HPP_
#define PROGRAM_HPP_

namespace gll
{

class Program
{
private:
	GLuint program;
	bool dirty;
	GLuint currentAttr;

	void checkDirty(std::string msg)
	{
		if (dirty)
		{
			std::cerr << "GLL: ERROR: " << msg << " (Program is not linked)\n";
		}
	}
public:
	void create()
	{
		program = glCreateProgram();
		dirty = true;
		currentAttr = 0;
	}

	void destroy()
	{
		glDeleteProgram(program);
	}

	void addShader(GLenum type, std::string src, bool isFile);

	Attribute getAttribute(std::string name)
	{
		glBindAttribLocation(program, currentAttr, name.c_str());
		dirty = true;
		return currentAttr++;
	}

	void link();

	GLint getUniformLocation(std::string name)
	{
		checkDirty("Cannot get uniform location");
		return glGetUniformLocation(program, name.c_str());
	}

	void bind()
	{
		checkDirty("Cannot bind program");
		glUseProgram(program);
	}
};

}  // namespace gll

#endif /* PROGRAM_HPP_ */
