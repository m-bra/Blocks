#version 130

smooth in vec3 frag_RGB;
out vec4 fragOut_RGBA;

void main()
{
	fragOut_RGBA = vec4(frag_RGB, 1);
}