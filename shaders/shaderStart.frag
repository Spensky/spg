#version 410 core

in vec3 normal;
in vec4 fragPosEye;
in vec4 fragPosLightSpace;
in vec2 fragTexCoords;

out vec4 fColor;

//lighting
uniform	mat3 normalMatrix;
uniform mat3 lightDirMatrix;
uniform	vec3 lightColor;
uniform	vec3 lightDir;
uniform vec3 cameraPosEye;
uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;
uniform sampler2D shadowMap;
uniform float nbOfPointLights;
uniform vec3 pointPos;
uniform vec3 lightColorPoint;
uniform vec3 fogDensity;
vec3 ambient;
vec3 diffuse;
vec3 specular;
vec3 ambientSun;
vec3 diffuseSun;
vec3 specularSun;
float ambientStrength = 0.2f;
float specularStrength = 0.5f;
float shininess = 64.0f;
vec3 o;

float fogFactor(){
	float fogDensity = 0.02f;
	float result = 0.0f;
	float fragmentDistance = length(fragPosEye.z/fragPosEye.w);
	result = exp(-pow(fragmentDistance * fogDensity,2));
	return clamp(result,0.0f,1.0f);
}

float computeAttenuation(){
	float constant = 1.0f;
	float linear = 0.045f;//0.045
	float quadratic = 0.0075f;//0.0075

	//compute distance to light
	float dist = length(pointPos -fragPosEye.xyz);
	//compute attenuation
	float att = 1.0f / (constant + linear * dist + quadratic * (dist * dist));
	return att;
}

void computePointLight()
{	
		
	//vec3 cameraPosEye = vec3(0.0f);//in eye coordinates, the viewer is situated at the origin
	
	//transform normal
	vec3 normalEye = normalize(normalMatrix * normal);	
	
	//compute light direction
	vec3 lightDirN = normalize(lightDirMatrix * pointPos - fragPosEye.xyz);	

	//compute view direction 
	vec3 viewDirN = normalize(cameraPosEye - fragPosEye.xyz);
	
	//compute half vector
	vec3 halfVector = normalize(lightDirN + viewDirN);
		
	//compute ambient light
	ambient = ambientStrength * lightColorPoint;
	
	//compute diffuse light
	diffuse = max(dot(normalEye, lightDirN), 0.0f) * lightColorPoint;
	
	//compute specular light
	float specCoeff = pow(max(dot(halfVector, normalEye), 0.0f), shininess);
	specular = specularStrength * specCoeff * lightColorPoint;
}

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

float computeShadow()
{	
	// perform perspective divide
    vec3 normalizedCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    if(normalizedCoords.z > 1.0f)
        return 0.0f;
    // Transform to [0,1] range
    normalizedCoords = normalizedCoords * 0.5f + 0.5f;
    // Get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, normalizedCoords.xy).r;    
    // Get depth of current fragment from light's perspective
    float currentDepth = normalizedCoords.z;
    // Check whether current frag pos is in shadow
    float bias = max(0.05f * (1.0f - dot(normal, lightDir)), 0.005f); 
    float shadow = currentDepth - bias> closestDepth  ? 1.0f : 0.0f;
    return shadow;	
}

vec3 calculateSun(){
	computeSunLight();
	float shadow = computeShadow();
	//modulate with diffuse map
	ambientSun *= vec3(texture(diffuseTexture, fragTexCoords));
	diffuseSun *= vec3(texture(diffuseTexture, fragTexCoords));
	//modulate woth specular map
	specularSun *= vec3(texture(specularTexture, fragTexCoords));
	vec3 color = min((ambientSun + (1.0f - shadow) * diffuseSun) + (1.0f - shadow) * specularSun, 1.0f);
	return color;
}

vec3 calculatePointLight(){
	vec3 color;
	//for(int i=0; i<nbOfPointLights; ++i){
		computePointLight();
		float att = computeAttenuation();
		float shadow = computeShadow();
		ambient *= vec3(texture(diffuseTexture, fragTexCoords));
		diffuse *= vec3(texture(diffuseTexture, fragTexCoords));
		//modulate woth specular map
		specular *= vec3(texture(specularTexture, fragTexCoords));
		ambient *= att;
		diffuse *= att;
		specular *= att;
		color += min((ambient + (1.0f - shadow) * diffuse) + (1.0f - shadow) * 	specular, 1.0f);
//	}
	return color;
}


void main() 
{
	vec4 fogColor = vec4(0.5f,0.5f,0.5f,1.0f);
	float fogFactor = fogFactor();
	//float fogCoord = abs(fragPosEye.z / fragPosEye.w);
	vec3 color = calculateSun();
	color += calculatePointLight();
	
	fColor = fogColor * (1- fogFactor) + vec4(color,1.0f)*fogFactor;
	//specular *= att;
	//diffuse *= att;
	//ambient *= att ;
	//ambient *=  vec3(texture(diffuseTexture, fragTexCoords));
	//diffuse *=  vec3(texture(diffuseTexture, fragTexCoords));
	//modulate woth specular map
	//specular *=  vec3(texture(specularTexture, fragTexCoords));
	
	//modulate with shadow
	
    
    //fColor = vec4(color, 1.0f);
    //fColor = vec4(o, 1.0f);
    
}
