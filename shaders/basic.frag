#version 330 core
out vec4 FragColor;

in vec3 Pos;
in vec2 TexCoord;
in vec3 Normal;

//uniform sampler2D tex;

void main()
{
	FragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
    //FragColor = vec4(TexCoord.x, TexCoord.y, 0.0f, 1.0f);
	//FragColor = texture(tex, TexCoord);
	// FragColor = vec4(Pos.x / 32.0f, Pos.y / 32.0f, Pos.z / 32.0f, 1.0f);
	//FragColor = vec4((Normal.x + 1.0f) / 2.0f, (Normal.y + 1.0f) / 2.0f, (Normal.z + 1.0f) / 2.0f, 1.0f);

	vec3 light_dir = normalize(vec3(0.8f, -0.6f, 0.2f));
	float diffuse = dot(light_dir, Normal);

	//vec4 color = texture(tex, TexCoord);
	//FragColor = vec4(color.xyz * (max(diffuse, 0.0f) + 0.2f), color.w);
}
