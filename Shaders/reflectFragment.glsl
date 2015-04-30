#version 150 core

uniform sampler2D diffuseTex;
uniform sampler2D bumpTex;
uniform samplerCube cubeTex;

uniform vec4 lightColour;
uniform vec3 lightPos;
uniform vec3 cameraPos;
uniform float lightRadius;

in Vertex
{
	vec4 colour;
	vec2 texCoord;
	vec3 normal;
	
	//added by me
	vec3 tangent;
	vec3 binormal;
	
	vec3 worldPos;
} IN;

out vec4 gl_FragColor;

void main()
{
//added
	mat3 TBN = mat3(IN.tangent, IN.binormal, IN.normal);
	vec3 normal = normalize(TBN *(texture2D(bumpTex, IN.texCoord).rgb * 2.0 -1.0));

	vec4 diffuse = texture(diffuseTex, IN.texCoord) * IN.colour;
	vec3 incident = normalize(IN.worldPos - cameraPos);

	//
	float lambert = max(0.0, dot(incident, normal));

	float dist = length(lightPos - IN.worldPos);
	float atten = 1.0 - clamp(dist / lightRadius, 0.2, 1.0);
	vec4 reflection = texture(cubeTex, reflect(incident, normalize(IN.normal)));
	
	gl_FragColor = (lightColour * diffuse * atten ) * (diffuse + reflection) + lambert;
	
	gl_FragColor.a = 0.3;
}

