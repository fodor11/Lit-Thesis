#version 400

uniform sampler2D tex;
uniform sampler2D tex2;
uniform float offset; // fire distance to the camera

in vec2 fragTexCoord;

out vec4 finalColor;

void main() {
    float distanceFactor = 1.f / offset;
    // push texture upwards by offset, average rgb
    float y = fragTexCoord.y - 0.5 * distanceFactor;
    //clamp(y, 0.f, 1.f);

	vec4 textureColor = texture2D(tex2, vec2(fragTexCoord.x, y));
    //ITU-R recommendation (BT.709) - Luma
    float avg = 0.2126 * textureColor.r + 0.7152 * textureColor.g + 0.0722 * textureColor.b;
    finalColor = vec4(avg, avg, avg, textureColor.a);
}