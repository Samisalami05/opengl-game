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
	vec3 color;
};

uniform material mat;
uniform vec3 view_pos;

void main()
{
	FragColor = vec4(mat.color, 1.0);

	//FragColor = vec4(TexCoord.x, TexCoord.y, 0.0f, 1.0f);
	//FragColor = texture(tex, TexCoord);
	//FragColor = vec4(Pos.x / 32.0f, Pos.y / 32.0f, Pos.z / 32.0f, 1.0f);
	//FragColor = vec4((Normal.x + 1.0f) / 2.0f, (Normal.y + 1.0f) / 2.0f, (Normal.z + 1.0f) / 2.0f, 1.0f);

	//vec3 light_dir = normalize(vec3(0.8f, -0.6f, 0.2f));
	//float diffuse = dot(light_dir, Normal);

	//vec4 color = texture(tex, TexCoord);
	//FragColor = vec4(color.xyz * (max(diffuse, 0.0f) + 0.2f), color.w);
}
