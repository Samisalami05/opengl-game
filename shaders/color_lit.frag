#version 430 core
out vec4 FragColor;

in vec3 Pos;
in vec3 WorldPos;
in vec2 TexCoord;
in vec3 Normal;

struct material {
	vec3 color;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};

#include "shaders/lighting.glsl"

uniform material mat;
uniform vec3 view_pos;

void main()
{
	vec3 normal = normalize(Normal);
	vec3 view_dir = normalize(view_pos - WorldPos);
	
	vec3 result = vec3(0.0f);
	for (int i = 0; i < light_count; i++) {
		light light = lights[i];
		result += calc_light(light, mat, normal, view_dir, WorldPos);
	}

	FragColor = vec4(result * mat.color, 1.0);

	//FragColor = vec4(TexCoord.x, TexCoord.y, 0.0f, 1.0f);
	//FragColor = texture(tex, TexCoord);
	//FragColor = vec4(Pos.x / 32.0f, Pos.y / 32.0f, Pos.z / 32.0f, 1.0f);
	//FragColor = vec4((Normal.x + 1.0f) / 2.0f, (Normal.y + 1.0f) / 2.0f, (Normal.z + 1.0f) / 2.0f, 1.0f);

	//vec3 light_dir = normalize(vec3(0.8f, -0.6f, 0.2f));
	//float diffuse = dot(light_dir, Normal);

	//vec4 color = texture(tex, TexCoord);
	//FragColor = vec4(color.xyz * (max(diffuse, 0.0f) + 0.2f), color.w);
}
