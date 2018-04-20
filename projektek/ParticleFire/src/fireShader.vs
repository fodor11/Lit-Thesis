#version 150

uniform mat4 viewProjection;
uniform mat4 model;
uniform float scale;

in vec3 vert;
//in vec2 vertTexCoord;
in vec4 positionAndSize;
in float angle;

out vec3 fragVert;
//out vec2 fragTexCoord;

void main() {
	fragVert = vert;
    //fragTexCoord = vertTexCoord;
    
	// Scaling matrix
	float scaleValue = scale * positionAndSize.w;
	mat4 scalingMatrix;
	scalingMatrix[0][0] = scaleValue; 
	scalingMatrix[1][1] = scaleValue;
	scalingMatrix[2][2] = scaleValue;
	scalingMatrix[3][3] = 1;

	
	// Rotation matrix (Z axis)
	float cosAlpha = cos(angle);
	float sinAlpha = sin(angle);
	mat4 rotationMatrix;
	rotationMatrix[0][0] = cosAlpha;
	rotationMatrix[1][0] = -sinAlpha;
	rotationMatrix[0][1] = sinAlpha;
	rotationMatrix[1][1] = cosAlpha;
	rotationMatrix[2][2] = 1;
	rotationMatrix[3][3] = 1;
	

	// Translation, counter camera rotation
	mat4 translationMatrix;
	translationMatrix[0][0] = 1;
	translationMatrix[1][1] = 1;
	translationMatrix[2][2] = 1;
	translationMatrix[3][3] = 1;
	translationMatrix[3].xyz = positionAndSize.xyz;

	mat4 particleMatrix;
	particleMatrix = translationMatrix * model * rotationMatrix * scalingMatrix;
	
    // Apply all matrix transformations to vert
    gl_Position = viewProjection * particleMatrix * vec4(vert, 1);
}