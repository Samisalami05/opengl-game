#define MAX_LIGHTS 16

struct light {
	int type;
	float intensity;
	float range;
	vec3 color;
	vec3 dir;
	vec3 position;
};

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

const uint LIGHT_GLOBAL = 0;
const uint LIGHT_POINT  = 1;

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
