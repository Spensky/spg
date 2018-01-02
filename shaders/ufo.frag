#version 410 core

in vec3 normal;
in vec4 fragPosEye;
in vec4 fragPosLightSpace;
in vec2 fragTexCoords;

out vec4 fColor;

uniform	mat3 normalMatrix;
uniform mat3 lightDirMatrix;
uniform	vec3 lightColor;
uniform	vec3 lightDir;
uniform vec3 cameraPosEye;
uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;

vec3 ambientSun;
vec3 diffuseSun;
vec3 specularSun;
float ambientStrength = 0.2f;
float specularStrength = 0.5f;
float shininess = 64.0f;

void computeSunLight()
{	
		
	//vec3 cameraPosEye = vec3(0.0f);//in eye coordinates, the viewer is situated at the origin
	
	//transform normal
	vec3 normalEye = normalize(normalMatrix * normal);	
	
	//compute light direction
	vec3 lightDirN = normalize(lightDirMatrix * lightDir);	

	//compute view direction 
	vec3 viewDirN = normalize(cameraPosEye - fragPosEye.xyz);
	
	//compute half vector
	vec3 halfVector = normalize(lightDirN + viewDirN);
		
	//compute ambient light
	ambientSun = ambientStrength * lightColor;
	
	//compute diffuse light
	diffuseSun = max(dot(normalEye, lightDirN), 0.0f) * lightColor;
	
	//compute specular light
	float specCoeff = pow(max(dot(halfVector, normalEye), 0.0f), shininess);
	specularSun = specularStrength * specCoeff * lightColor;
}

vec3 calculateSun(){
	computeSunLight();
//	float shadow = computeShadow();
	//modulate with diffuse map
	ambientSun *= vec3(texture(diffuseTexture, fragTexCoords));
	diffuseSun *= vec3(texture(diffuseTexture, fragTexCoords));
	//modulate woth specular map
	specularSun *= vec3(texture(specularTexture, fragTexCoords));
	vec3 color = min(ambientSun + diffuseSun +  specularSun, 1.0f);
	return color;
}

void main() 
{    
	//vec3 color = calculateSun();
	ambientSun = vec3(texture(diffuseTexture, fragTexCoords));
	diffuseSun = vec3(texture(diffuseTexture, fragTexCoords));
	//modulate woth specular map
	specularSun = vec3(texture(specularTexture, fragTexCoords));
	vec3 color = min(ambientSun + diffuseSun +  specularSun, 1.0f);
    	//fColor = vec4(0.5f,0.5f,0.5f,1.0f);
   // fColor = vec4(color,1.0f);
}
