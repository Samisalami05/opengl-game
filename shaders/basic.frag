#version 330 core
out vec4 FragColor;

in vec3 Pos;
in vec3 WorldPos;
in vec2 TexCoord;
in vec3 Normal;

struct material {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};

uniform material mat;
uniform vec3 view_pos;

//uniform sampler2D tex;

void main()
{
	vec3 light_pos = vec3(0.0f, 5.0f, 2.5f);

	vec3 light_col = vec3(1.0f, 1.0f, 1.0f);
	vec3 ambient = light_col * mat.ambient;

	vec3 norm = normalize(Normal);
	vec3 light_dir = normalize(light_pos - WorldPos);
	float diff = max(dot(norm, light_dir), 0.0f);
	vec3 diffuse = light_col * (diff * mat.diffuse);

	vec3 view_dir = normalize(view_pos - WorldPos);
	vec3 reflect_dir = reflect(-light_dir, norm);
	float spec = pow(max(dot(view_dir, reflect_dir), 0.0f), mat.shininess);
	vec3 specular = light_col * (spec * mat.specular);

	vec3 result = ambient + diffuse + specular;
	FragColor = vec4(result, 1.0);

	//FragColor = vec4(TexCoord.x, TexCoord.y, 0.0f, 1.0f);
	//FragColor = texture(tex, TexCoord);
	//FragColor = vec4(Pos.x / 32.0f, Pos.y / 32.0f, Pos.z / 32.0f, 1.0f);
	//FragColor = vec4((Normal.x + 1.0f) / 2.0f, (Normal.y + 1.0f) / 2.0f, (Normal.z + 1.0f) / 2.0f, 1.0f);

	//vec3 light_dir = normalize(vec3(0.8f, -0.6f, 0.2f));
	//float diffuse = dot(light_dir, Normal);

	//vec4 color = texture(tex, TexCoord);
	//FragColor = vec4(color.xyz * (max(diffuse, 0.0f) + 0.2f), color.w);
}
