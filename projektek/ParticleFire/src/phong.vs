#version 150

uniform mat4 camera;
uniform mat4 model;

in vec3 vert;
in vec2 vertTexCoord;
in vec3 vertAmbientColor;
in vec3 vertDiffuseColor;
in vec3 vertSpecularColor;
in float vertShininess;
in vec3 vertNorm;

out vec3 fragVert;
out vec2 fragTexCoord;
out vec3 fragAmbientColor;
out vec3 fragDiffuseColor;
out vec3 fragSpecularColor;
out float fragShininess;
out vec3 fragNormal;

void main() {
	fragVert = vert;
    fragTexCoord = vertTexCoord;
	fragAmbientColor = vertAmbientColor;
	fragDiffuseColor = vertDiffuseColor;
	fragSpecularColor = vertSpecularColor;
	fragShininess = vertShininess;
	fragNormal = vertNorm;
    
    // Apply all matrix transformations to vert
    gl_Position = camera * model * vec4(vert, 1);
}