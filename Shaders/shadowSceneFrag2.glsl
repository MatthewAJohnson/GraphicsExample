#version 150 core

uniform sampler2D diffuseTex;
uniform sampler2D bumpTex;
uniform sampler2DShadow shadowTex;
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
	vec3 tangent;
	vec3 binormal;
	vec3 worldPos;
	vec4 shadowProj;
} IN;

out vec4 gl_FragColor;

void main()
{
	mat3 TBN = mat3(IN.tangent, IN.binormal, IN.normal);
	vec3 normal = normalize(TBN *(texture2D(bumpTex, IN.texCoord).rgb * 2.0 -1.0));
	
	vec4 diffuse = texture(diffuseTex, IN.texCoord) * IN.colour;
	
	vec3 incident = normalize(IN.worldPos - cameraPos);
	float lambert = max(0.0, dot(incident, normal));
	
	float dist = length(lightPos - IN.worldPos);
	float atten = 1.0 - clamp(dist / lightRadius, 0.2, 1.0);
	
	vec4 reflection = texture(cubeTex, reflect(incident, normalize(IN.normal)));
	
	vec3 viewDir = normalize(cameraPos - IN.worldPos);
	vec3 halfDir = normalize(incident + viewDir);
	
	float rFactor = max(0.0, dot(halfDir, normal));
	float sFactor = pow(rFactor, 33.0);
	
	float shadow = 1.0;
	float bias = 0.05;
	
	//if(IN.shadowProj.w > bias)
	//{
		shadow = textureProj(shadowTex, IN.shadowProj);	
	//}
	
	
	lambert *= shadow;
	
	vec3 colour = (diffuse.rgb * lightColour.rgb);
	colour += (lightColour.rgb * sFactor) * 0.33;
	
	gl_FragColor = (colour * atten * lambert, diffuse.a) * (diffuse + reflection);
	gl_FragColor.rgb += (diffuse.rgb * lightColour.rgb) * 0.1;
	gl_FragColor.a = 0.5;
	//gl_FragColor.rgb = vec3(shadow);
}