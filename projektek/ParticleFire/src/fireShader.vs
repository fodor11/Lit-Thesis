#version 150

uniform mat4 viewProjection;
uniform mat4 model;

in vec3 vert;
//in vec2 vertTexCoord;
in vec4 positionAndSize;

out vec3 fragVert;
//out vec2 fragTexCoord;

void main() {
	fragVert = vert;
    //fragTexCoord = vertTexCoord;
    
	//Apply translation, scaling, rotation to vertex
	mat4 particleMatrix = model;
	/*
	particleMatrix[0][0] = particleMatrix[0][0] * positionAndSize.z; // scale
	particleMatrix[1][1] = particleMatrix[0][0] * positionAndSize.z; // scale
	particleMatrix[2][2] = particleMatrix[0][0] * positionAndSize.z; // scale
	particleMatrix[3][3] = particleMatrix[0][0] * positionAndSize.z; // scale
	*/
	particleMatrix[3].xyz = positionAndSize.xyz; // translate
	
    // Apply all matrix transformations to vert
    gl_Position = viewProjection * particleMatrix * vec4(vert, 1);

}