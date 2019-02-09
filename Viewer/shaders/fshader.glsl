#version 330 core

struct Material
{
	sampler2D textureMap;
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
};

uniform Material material;

in vec3 fragPos;
in vec3 fragNormal;
in vec2 fragTexCoords;

out vec4 fragColor;

void main()
{
	// Sample the texture-map at the UV coordinates given by 'fragTexCoords'
	//vec3 textureColor = vec3(texture(material.textureMap, fragTexCoords));

	fragColor = material.ambient;
}
