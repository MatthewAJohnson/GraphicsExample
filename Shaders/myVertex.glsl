#version 150 core

in vec3 position;
in vec4 colour;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

out Vertex
{
	vec4 colour;
	vec3 normal;
} OUT;

void main()
{
	gl_Position = (projMatrix * viewMatrix * modelMatrix) * vec4(position, 1.0);
	OUT.colour = colour;
	vec3 tempPos = position - vec3(0,0,1);
}