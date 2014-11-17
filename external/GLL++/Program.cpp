/*
 * Program.cpp
 *
 *  Created on: Aug 31, 2014
 *      Author: merlin
 */

#include "GLL.hpp"
#include <fstream>
#include <vector>
#include <cstdio>

void gll::Program::addShader(GLenum type, std::string src, bool isFile)
{
	using namespace std;

	// load
	if (isFile)
	{
		ifstream ifs(src.c_str(), ios::in | ios::binary | ios::ate);
		if (!ifs.is_open())
		{
			cerr << "ERROR: GLL: Cannot open shader: " << src << "\n";
			exit(EXIT_FAILURE);
		}
		ifstream::pos_type size = ifs.tellg();
		ifs.seekg(0, ios::beg);
		vector<char> bytes(size);
		ifs.read(&bytes[0], size);
		src = string(&bytes[0], size);
	}

	// compile
	GLuint shader = glCreateShader(type);
	char const *src_c_str = src.c_str();
	int srclen = src.length();
	glShaderSource(shader, 1, &src_c_str, &srclen);
	glCompileShader(shader);

	// check
	GLint compiled, logLen;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLen);
	if (!compiled)
	{
		string log;
		log.reserve(logLen);
		glGetShaderInfoLog(shader, logLen, 0, &log[0]);
		char const *typestr = type == GL_VERTEX_SHADER ?"vertex" :"fragment";
		cerr << "GLL: ERROR: Failed to compile " <<  typestr << " shader: " << log << "\n";
		exit(EXIT_FAILURE);
	}

	// add
	glAttachShader(program, shader);

	dirty = true;
}

void gll::Program::link()
{
	using namespace std;

	if (!dirty)
		cout << "GLL: WARNING: Program linked unnecessarily\n";

	glLinkProgram(program);

	int status;
	int logLength;
	glGetProgramiv(program, GL_LINK_STATUS, &status);
	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
	if (!status)
	{
		char log[logLength];
		glGetProgramInfoLog(program, logLength, 0, log);
		fprintf(stderr, "GLL: ERROR: Failed to link shaders: %s\n", log);
		exit(EXIT_FAILURE);
	}

#if 0
	glGetProgramiv(program, GL_VALIDATE_STATUS, &status);
	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
	if (!status)
	{
		char log[logLength];
		glGetProgramInfoLog(program, logLength, 0, log);
		fprintf(stderr, "GLL: ERROR: Failed to validate program: %s\n", log);
		exit(EXIT_FAILURE);
	}
#endif

	int const maxShaderCount = 10;
	int shaderCount;
	GLuint shaders[maxShaderCount];
	glGetAttachedShaders(program, maxShaderCount, &shaderCount, &shaders[0]);
	for (int i = 0; i < shaderCount; ++i)
	{
		glDetachShader(program, shaders[i]);
		glDeleteShader(shaders[i]);
	}

	dirty = false;
}
