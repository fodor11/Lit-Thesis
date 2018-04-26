#version 400

uniform sampler2D tex;
in vec2 fragTexCoord;

out vec4 finalColor;

void main() {
	vec4 textureColor = texture2D(tex, fragTexCoord);
    finalColor = vec4(textureColor.rgba);
}