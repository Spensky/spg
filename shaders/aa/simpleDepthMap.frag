#version 410 core

out vec4 fColor;
//in vec2 fragTexCoords;

//uniform sampler2D depthMap;

void main()
{
	fColor = vec4(1.0f);
	//float depthValue = texture(depthMap, fragTexCoords).r;
    //fColor = vec4(vec3(depthValue), 1.0);
}
