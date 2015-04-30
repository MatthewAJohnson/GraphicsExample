#version 150 core

uniform sampler2D diffuseTex;

in Vertex 
{
	vec2 texCoord;
} IN;

out vec4 gl_FragColor;

void main() 
{
	gl_FragColor = texture(diffuseTex, IN.texCoord);
}