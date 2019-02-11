#version 330 core

struct Material
{
	sampler2D textureMap;
	vec4 color;

	float Ka;
	float Kd;
	float Ks;
	int alpha;
};

uniform Material material;
uniform vec4 lightLocations[5];
uniform vec4 lightColors[5];
uniform bool useTexture;

in vec4 fragPos;
in vec4 fragNormal;
in vec2 fragTexCoords;

out vec4 fragColor;

void main()
{
	// Sample the texture-map at the UV coordinates given by 'fragTexCoords'
	vec4 materialColor = material.color;

	if (useTexture) {
		vec3 textureColor = vec3(texture(material.textureMap, fragTexCoords));
		materialColor = vec4(textureColor, 1.0f);
	}

	vec3 N = normalize(fragNormal.xyz / fragNormal.w);
	vec3 V = normalize(fragPos.xyz / fragPos.w);

	vec4 IA = material.Ka * materialColor;
	vec4 ID = vec4(0.0f);
	vec4 IS = vec4(0.0f);

	for (int i = 0; i < 5; i++) {
		vec4 lightColor = lightColors[i];
		vec3 lightLocation = lightLocations[i].xyz / lightLocations[i].w;

		vec3 L = normalize(lightLocation - (fragPos.xyz / fragPos.w));
		vec3 R = normalize(reflect(-L, N));
		float diff = max(dot(N, L), 0.0f);
		float spec = pow(max(dot(R, V), 0.0f), material.alpha);

		vec4 diffuseColor = material.Kd * diff * lightColor;
		ID = ID + diffuseColor;

		vec4 specularColor = material.Ks * spec * lightColor;
		IS = IS + specularColor;
	}

	vec4 illumination = IA + ID + IS;

	fragColor = clamp(illumination * materialColor, 0.0f, 1.0f);
}
