#version 150 core

float Brightness;
uniform sampler2D tex0;

varying struct  _v2f_
{
	vec4 position;
	vec2 texCoord;
	vec4 color;
} v2f;

void main()
{
	const int NUM = 9;
	const float threshold = 0.05;
	
	const vec2 c[NUM] = {
	vec2(-0.0078125, 0.0078125), vec2( 0.00 , 0.0078125),
    vec2( 0.0078125, 0.0078125), vec2(-0.0078125, 0.00 ),
    vec2( 0.0, 0.0 ), vec2( 0.0078125, 0.007 ),
    vec2(-0.0078125,-0.0078125), vec2( 0.00 , -0.0078125),
    vec2( 0.0078125,-0.0078125)
	};
	
	vec3 col[NUM];
	int i;
	for(i = 0; i < NUM; ++i)
	{
		col[i] = texture2D(tex0, a2f.Texcoord.xy + c[i]);
	}
	
	vec3 rgb21um = vec3(0.30,0.59,0.11);
	float lum[NUM];
	for(i = 0; i < NUM; ++i)
	{
		lum[i] = dot(col[i].xyz, rgb2lum);
	}
	float x = lum[2] + lum[8] + 2 * lum[5] - lum[0] - 2 * lum[3] - lum[6];
	float y = lum[6] + 2 * lum[7] + lum[8] - lum[0] - 2 * lum[1] - lum[2];
	float edge = (x*x + y * y < threshold) ? 1.0 : 0.0;
	gl_FragColor = vec4(Brightness * col [5].xyz *edge.xxx, 1.0);//none of this works :( 
}