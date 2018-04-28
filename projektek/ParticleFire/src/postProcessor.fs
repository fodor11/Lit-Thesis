#version 400

uniform sampler2D tex;
uniform sampler2D tex2;
uniform sampler2D backgroundDepth;
uniform sampler2D foregroundDepth;
uniform float offset;

in vec2 fragTexCoord;

out vec4 finalColor;

void main() {

    float distortionMap = texture2D(tex2, fragTexCoord).r;
    float bgDepth = texture2D(backgroundDepth, fragTexCoord).r;
    float fgDepth = texture2D(foregroundDepth, fragTexCoord).r;

    vec4 textureColor;

    if (fgDepth < bgDepth)
    {
        if (distortionMap > 0.f)
        {
            float amplitude = 0.003f; // szethuz
            float frequency = 50.0f;  // osszenyom
            float distortion = sin(fragTexCoord.y * frequency + offset) * amplitude;
            distortion = distortion * pow(distortionMap, 1/2);

            textureColor = texture2D(tex, vec2(fragTexCoord.x + distortion, fragTexCoord.y));
            //textureColor = vec4(1,0,0,1);
        }
        else
        {
             textureColor = texture2D(tex, fragTexCoord);
        }
    }
    else
    {
         textureColor = texture2D(tex, fragTexCoord);
    }
	
    finalColor = vec4(textureColor.rgba);
}