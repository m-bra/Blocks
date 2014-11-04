#version 130

in vec3 vert_XYZ;
in vec3 vert_RGB;
smooth out vec3 frag_RGB;

uniform mat4 transform;

void main()
{
	gl_Position = transform * vec4(vert_XYZ, 1);
	frag_RGB = vert_RGB;
}