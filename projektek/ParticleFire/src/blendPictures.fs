#version 400

uniform sampler2D tex;
uniform sampler2D tex2;
uniform sampler2D backgroundDepth;
uniform sampler2D foregroundDepth;

in vec2 fragTexCoord;

out vec4 finalColor;

void main() {
	vec4 background = texture2D(tex, fragTexCoord);
    float bgDepth = texture2D(backgroundDepth, fragTexCoord).r;
    float fgDepth = texture2D(foregroundDepth, fragTexCoord).r;

    if (fgDepth < bgDepth){
        vec4 foreground = texture2D(tex2, fragTexCoord);
        finalColor = vec4(foreground.rgb + background.rgb, background.a);
    }
    else{
        finalColor = background;
    }
}