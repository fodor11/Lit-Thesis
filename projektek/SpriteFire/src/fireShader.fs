#version 150

uniform sampler2D tex;

in vec3 fragVert;
in vec2 fragTexCoord;

out vec4 finalColor;

void main() {
	vec4 textureColor = texture(tex, fragTexCoord);
    
	finalColor = vec4(textureColor.rgba);
}