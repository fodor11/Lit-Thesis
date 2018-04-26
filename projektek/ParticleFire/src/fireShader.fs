#version 400

uniform sampler2D tex;

in vec3 fragVert;
in vec2 fragTexCoord;
in vec2 fragTexCoord2;
in float currentBlend;
flat in int smoke;

out vec4 finalColor;

void main() {
	vec4 textureColor = texture(tex, fragTexCoord);
    if (smoke == 1)
    {
        // blending with the texture color for a short time
        float blendWithOldColor = currentBlend - 0.85f;
        if(blendWithOldColor > 0.f)
        {
            blendWithOldColor = blendWithOldColor / 0.15f;
            finalColor = vec4((1.f - blendWithOldColor) * vec3(0.2f,0.2f,0.2f) + blendWithOldColor * textureColor.rgb, currentBlend * textureColor.a);
            //finalColor = vec4(1,0,0,1);
        }
        else 
        {
            finalColor = vec4(vec3(0.2f,0.2f,0.2f), currentBlend * textureColor.a);
        }
    }
    else
    {
	    vec4 textureColor2 = texture(tex, fragTexCoord2);
        finalColor = vec4(currentBlend * textureColor.rgba + (1 - currentBlend) * textureColor2.rgba);
    }

    //finalColor = vec4(1,1,1,1);
}