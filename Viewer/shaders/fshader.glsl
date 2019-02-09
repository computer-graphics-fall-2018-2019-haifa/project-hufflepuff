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

in vec4 fragPos;
in vec4 fragNormal;
in vec2 fragTexCoords;

out vec4 fragColor;

void main()
{
	// Sample the texture-map at the UV coordinates given by 'fragTexCoords'
	//vec3 textureColor = vec3(texture(material.textureMap, fragTexCoords));

	vec4 N = normalize(fragNormal);
	vec4 V = normalize(fragPos);

	vec4 IA = material.Ka * material.color;
	vec4 ID = vec4(0.0f);
	vec4 IS = vec4(0.0f);

	for (int i = 0; i < 5; i++) {
		vec4 lightColor = lightColors[i];
		vec4 lightLocation = lightLocations[i];
		vec4 L = normalize(lightLocation - fragPos);
		vec4 R = normalize(reflect(-L, N));
		float LN = max(dot(N, L), 0.0f);
		float RV = pow(max(dot(R, V), 0.0f), material.alpha);

		vec4 diffuseColor = material.Kd * LN * lightColor;
		ID = ID + diffuseColor;

		vec4 specularColor = material.Ks * RV * lightColor;
		IS = IS + specularColor;
	}

	vec4 illumination = IA + ID + IS;

	fragColor = clamp(illumination * material.color, 0.0f, 1.0f);
}
