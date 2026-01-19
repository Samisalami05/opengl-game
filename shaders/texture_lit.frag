#version 430 core
out vec4 FragColor;

in vec3 Pos;
in vec3 WorldPos;
in vec2 TexCoord;
in vec3 Normal;

struct material {
	sampler2D albedo_tex;
	//texture* normal_tex;
	//texture* roughness_tex;
	//texture* metallic_tex;
	//texture* glossiness_tex;
	//texture* specular_tex;

	vec3 color;

	vec3 ambient; // TODO: Implement textures for these aswell
	vec3 diffuse;
	vec3 specular;
	float shininess;

	vec3 emission;
	float emission_str;

	vec2 tiling;
};

#include "shaders/lighting.glsl"

uniform material mat;
uniform vec3 view_pos;

const float near = 0.1f;
const float far = 200.0f;

float linear_depth(float depth) 
{
    float z = depth * 2.0f - 1.0f;
    return (2.0f * near * far) / (far + near - z * (far - near));	
}

void main()
{
	vec3 normal = normalize(Normal);
	vec3 view_dir = normalize(view_pos - WorldPos);
	
	vec3 result = vec3(0.0f);
	for (int i = 0; i < light_count; i++) {
		light light = lights[i];
		result += calc_light(light, mat, normal, view_dir, WorldPos);
	}

	FragColor = vec4(result * mat.color, 1.0) * texture(mat.albedo_tex, TexCoord * mat.tiling);
	
	//FragColor = vec4(vec3(linear_depth(gl_FragCoord.z) / far), 1);
	//FragColor = vec4(TexCoord.x, TexCoord.y, 0.0f, 1.0f);
	//FragColor = texture(mat.albedo_tex, TexCoord);
	//FragColor = vec4(Pos.x / 32.0f, Pos.y / 32.0f, Pos.z / 32.0f, 1.0f);
	//FragColor = vec4((Normal.x + 1.0f) / 2.0f, (Normal.y + 1.0f) / 2.0f, (Normal.z + 1.0f) / 2.0f, 1.0f);

	//vec3 light_dir = normalize(vec3(0.8f, -0.6f, 0.2f));
	//float diffuse = dot(light_dir, Normal);

	//vec4 color = texture(tex, TexCoord);
	//FragColor = vec4(color.xyz * (max(diffuse, 0.0f) + 0.2f), color.w);
}
