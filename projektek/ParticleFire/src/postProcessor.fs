#version 400

uniform sampler2D tex;
uniform float offset;

in vec2 fragTexCoord;

out vec4 finalColor;

void main() {
    float amplitude = 0.003f;
    float frequency = 100.0f;
    float distortion = sin(fragTexCoord.y * frequency + offset) * amplitude;


	vec4 textureColor = texture2D(tex, vec2(fragTexCoord.x + distortion, fragTexCoord.y));
    finalColor = vec4(textureColor.rgba);
}