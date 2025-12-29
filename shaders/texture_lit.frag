#version 430 core
out vec4 FragColor;

in vec3 Pos;
in vec3 WorldPos;
in vec2 TexCoord;
in vec3 Normal;

// TODO: Light buffer
//struct light {
//    vec4 color_intensity; // color     + intensity
//    vec4 position_range;  // position  + range
//    vec4 direction_type;  // direction + type (encoded)
//};

//layout(std430, binding = 0) buffer lightbuffer {
//    uint lightCount;
//    light lights[];
//};

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
};

struct light {
	int type;
	float intensity;
	float range;
	vec3 color;
	vec3 dir;
	vec3 position;
};

#define MAX_LIGHTS 16

const uint LIGHT_GLOBAL = 0;
const uint LIGHT_POINT  = 1;

uniform material mat;
uniform vec3 view_pos;
uniform int light_count;
uniform light lights[MAX_LIGHTS];

vec3 calc_global_light(light light, material mat, vec3 normal, vec3 view_dir) {
	vec3 light_dir = normalize(-light.dir);
	float diff = max(dot(normal, light_dir), 0.0f);

	vec3 reflect_dir = reflect(-light_dir, normal);
	float spec = pow(max(dot(view_dir, reflect_dir), 0.0f), mat.shininess);

	vec3 ambient  = light.color * mat.ambient;
    vec3 diffuse  = light.color * diff * mat.diffuse;
    vec3 specular = light.color * spec * mat.specular;
	return (ambient + diffuse + specular) * light.intensity;
}

vec3 calc_point_light(light light, material mat, vec3 normal, vec3 view_dir, vec3 frag_pos) {
	vec3 light_dir = normalize(-light.dir);
	float diff = max(dot(normal, light_dir), 0.0f);

	vec3 reflect_dir = reflect(-light_dir, normal);
	float spec = pow(max(dot(view_dir, reflect_dir), 0.0f), mat.shininess);

	float dist = length(light.position - frag_pos);
	float d = dist / light.range;
	float invSq = 1.0 / (1.0 + d * d);
	float cutoff = 1.0 - smoothstep(0.7, 1.0, d);

	float attenuation = invSq * cutoff;

	vec3 ambient  = light.color * mat.ambient;
    vec3 diffuse  = light.color * diff * mat.diffuse;
    vec3 specular = light.color * spec * mat.specular;
	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

	return (ambient + diffuse + specular) * light.intensity;
}

vec3 calc_light(light light, material mat, vec3 normal, vec3 view_dir, vec3 frag_pos) {
	switch (light.type) {
		case LIGHT_GLOBAL:
			return calc_global_light(light, mat, normal, view_dir);
		case LIGHT_POINT:
			return calc_point_light(light, mat, normal, view_dir, frag_pos);
	}
	return vec3(0.0f);
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

	FragColor = vec4(result * texture(mat.albedo_tex, TexCoord).rgb * mat.color, 1.0);

	//FragColor = vec4(TexCoord.x, TexCoord.y, 0.0f, 1.0f);
	//FragColor = texture(tex, TexCoord);
	//FragColor = vec4(Pos.x / 32.0f, Pos.y / 32.0f, Pos.z / 32.0f, 1.0f);
	//FragColor = vec4((Normal.x + 1.0f) / 2.0f, (Normal.y + 1.0f) / 2.0f, (Normal.z + 1.0f) / 2.0f, 1.0f);

	//vec3 light_dir = normalize(vec3(0.8f, -0.6f, 0.2f));
	//float diffuse = dot(light_dir, Normal);

	//vec4 color = texture(tex, TexCoord);
	//FragColor = vec4(color.xyz * (max(diffuse, 0.0f) + 0.2f), color.w);
}
