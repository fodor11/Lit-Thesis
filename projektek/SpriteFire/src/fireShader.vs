#version 150

uniform mat4 camera;
uniform mat4 model;

in vec3 vert;
in vec2 vertTexCoord;

out vec3 fragVert;
out vec2 fragTexCoord;

void main() {
	fragVert = vert;
    fragTexCoord = vertTexCoord;
    
    // Apply all matrix transformations to vert
    gl_Position = camera * model * vec4(vert, 1);
}