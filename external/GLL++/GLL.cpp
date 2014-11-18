#include "GLL/GLL.hpp"

void gll::init()
{
	static bool initialized = false;
	if (initialized)
		return;
	initialized = true;
	GLenum returnVal = glewInit();
	if (returnVal != GLEW_OK)
	{
		std::cerr << "Failed initialize GLEW: " << glewGetErrorString(returnVal) << std::endl;
		exit(EXIT_FAILURE);
	}
}
