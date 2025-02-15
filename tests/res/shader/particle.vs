

#version 330 core
layout (location = 0) in vec4 vertex; // <vec2 position, vec2 texCoords>

out vec2 TexCoords;
out vec4 ParticleColor;

uniform mat4 projection;
uniform vec3 offset;
uniform vec4 color;

void main()
{
    float scale = 20.0f;    //10.0f;
    TexCoords = vertex.zw;
    ParticleColor = color;
    gl_Position = projection * vec4((vertex.x * scale) + offset.x,(vertex.y * scale) + offset.y, offset.z, 1.0);
//    gl_Position = projection * vec4((vertex.xyz * scale) + offset, 1.0);
}
