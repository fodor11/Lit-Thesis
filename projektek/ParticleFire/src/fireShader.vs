#version 400

uniform mat4 viewProjection;
uniform mat4 model;
uniform float scale;
uniform int rowCount;

in vec3 vert;
in int textureNumber;
in vec4 positionAndSize;
in vec2 angleAndBlend;

out vec3 fragVert;
out vec2 fragTexCoord;
out vec2 fragTexCoord2;
out float currentBlend;
flat out int smoke;

vec2 getTextureCoord(int textureNr, float offset){
	int row = int(textureNr) / rowCount;
	int column = int(mod(int(textureNr), int(rowCount)));
	if (vert.x == -0.5f){
		if(vert.y == 0.5){
			// upper left corner
			return(vec2(column * offset, row * offset));
		}
		// lower left corner
		return(vec2(column * offset, (row + 1) * offset));
	}
	if (vert.x == 0.5f){
		if(vert.y == 0.5){
			// upper right corner
			return(vec2((column + 1) * offset, row * offset));
		}
		// lower right corner
		return(vec2((column + 1) * offset, (row + 1) * offset));
	}
	return(vec2(-1,-1));
}

void main() {
	//fragVert = vert;
	currentBlend = angleAndBlend.y;

	float offset = 1.0f / float(rowCount);
    fragTexCoord = getTextureCoord(textureNumber, offset);
	// 15 should be the maximum index -> make this a uniform
	if (textureNumber < 15) {
		fragTexCoord2 = getTextureCoord(textureNumber+1, offset);	
		smoke = 0;
	}
	else{
		smoke = 1;
	}
    
	// Scaling matrix
	float scaleValue = scale * positionAndSize.w;
	mat4 scalingMatrix;
	scalingMatrix[0][0] = scaleValue; 
	scalingMatrix[1][1] = scaleValue;
	scalingMatrix[2][2] = scaleValue;
	scalingMatrix[3][3] = 1;

	
	// Rotation matrix (Z axis)
	float cosAlpha = cos(angleAndBlend.x);
	float sinAlpha = sin(angleAndBlend.x);
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