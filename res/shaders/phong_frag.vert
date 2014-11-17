#version 130
in vec3 vertXYZ;
in vec2 vertUV;
in vec3 vertNormalXYZ;
smooth out vec3 fragXYZ;
smooth out vec2 fragUV;
smooth out vec3 fragNormalXYZ;

uniform mat4 mvp;

void main()
{
	gl_Position = mvp * vec4(vertXYZ, 1);
	fragXYZ = vertXYZ;
	fragUV = vertUV;
	fragNormalXYZ = vertNormalXYZ;
}
