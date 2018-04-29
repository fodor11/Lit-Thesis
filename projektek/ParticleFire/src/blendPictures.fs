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

        //float stepX = 1.f/860.f;
        //float stepY = 1.f/640.f;
        ////TODO: test if new fgdepth < 1.0
        //float leftUpper = bgDepth - texture2D(foregroundDepth, vec2(fragTexCoord.x-stepX, fragTexCoord.y+stepY)).r;
        //float rightUpper = bgDepth - texture2D(foregroundDepth, vec2(fragTexCoord.x+stepX, fragTexCoord.y+stepY)).r;
        //float leftLower = bgDepth - texture2D(foregroundDepth, vec2(fragTexCoord.x-stepX, fragTexCoord.y-stepY)).r;
        //float rightLower = bgDepth - texture2D(foregroundDepth, vec2(fragTexCoord.x+stepX, fragTexCoord.y-stepY)).r;
        //float middleUpper = bgDepth - texture2D(foregroundDepth, vec2(fragTexCoord.x, fragTexCoord.y+stepY)).r;
        //float middleLower = bgDepth - texture2D(foregroundDepth, vec2(fragTexCoord.x, fragTexCoord.y-stepY)).r;
        //float rightMiddle = bgDepth - texture2D(foregroundDepth, vec2(fragTexCoord.x+stepX, fragTexCoord.y)).r;
        //float leftMiddle = bgDepth - texture2D(foregroundDepth, vec2(fragTexCoord.x-stepX, fragTexCoord.y)).r;
        //float distance = ((bgDepth - fgDepth) + leftUpper + rightUpper + leftLower + rightLower + middleUpper + middleLower + rightMiddle + leftMiddle) / 9.f;
        //if (distance < 0.003){
        //    float blend = max(distance / 0.003, 0.f);
        //    finalColor = vec4(blend * foreground.rgb + background.rgb, background.a);
        //}
        //else {
            finalColor = vec4(foreground.rgb + background.rgb, background.a);
        //}
    }
    else{
        finalColor = background;
    }
}