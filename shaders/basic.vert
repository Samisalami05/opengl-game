#version 430 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aUv;
layout (location = 2) in vec3 aNormal;

out vec3 Pos;
out vec3 WorldPos;
out vec2 TexCoord;
out vec3 Normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	vec4 world_pos = model * vec4(aPos, 1.0f);

    gl_Position = projection * view * world_pos;
	
	Pos = aPos;
	WorldPos = world_pos.xyz;
	TexCoord = aUv;
	
	mat3 normalMatrix = transpose(inverse(mat3(model)));
    Normal = normalize(normalMatrix * aNormal);
}
