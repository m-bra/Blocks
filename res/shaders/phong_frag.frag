#version 130
smooth in vec3 fragXYZ;
smooth in vec2 fragUV;
smooth in vec3 fragNormalXYZ;
out vec4 fragRGBA;

uniform mat4 model;
uniform vec3 eyePosXYZ;
uniform sampler2D materialDiffuseTex;
uniform float materialShininess;
uniform vec3 materialSpecularRGB;
uniform vec3 ambientLightRGB;
uniform vec3 lightRGB;
uniform vec3 lightXYZ;
uniform float lightPower;

void main()
{
	vec3 fragXYZ_world = (model * vec4(fragXYZ, 1)).xyz;
	vec3 fragNormalXYZ_world = (model * vec4(fragNormalXYZ, 0)).xyz;
	//vec3 l = normalize(lightXYZ - fragXYZ_world);   // for lightXYZ as point
	vec3 l = -normalize(lightXYZ);                   // for lightXYZ as direction
	vec3 n = normalize(fragNormalXYZ_world);
	vec3 r = normalize(reflect(-l, n));
	vec3 v = normalize(eyePosXYZ - fragXYZ_world);

	float distance_squared = length(lightXYZ - fragXYZ_world);
	distance_squared = pow(distance_squared, lightPower);

	vec4 materialDiffRGBA = texture(materialDiffuseTex, fragUV);

	fragRGBA = materialDiffRGBA * vec4(ambientLightRGB, 1);
	fragRGBA+= materialDiffRGBA * vec4(lightRGB, 1) * max(0, dot(n, l)); // distance_squared; // for point light
	//fragRGBA+= vec4(materialSpecularRGB, 1) * vec4(lightRGB, 1) * pow(max(0, dot(r, v)), materialShininess) / distance_squared;
}
