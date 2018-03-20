#version 150

uniform mat4 model;
uniform sampler2D tex;

in vec3 fragVert;
in vec2 fragTexCoord;

out vec4 finalColor;

void main() {
	// calculate the location of this fragment (pixel) in world coordinates
	//vec3 fragPosition = vec3(model * vec4(fragVert, 1.0));

	vec4 textureColor = texture(tex, fragTexCoord);
    
	finalColor = vec4(textureColor.rgba);
}