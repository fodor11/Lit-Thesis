#version 400

uniform sampler2D tex;
uniform sampler2D tex2;
uniform sampler2D backgroundDepth;
uniform sampler2D foregroundDepth;
uniform float offset;

in vec2 fragTexCoord;

out vec4 finalColor;

void main() {
    vec4 distortionColor = texture(tex2, fragTexCoord);
    float distortionMap = distortionColor.r * distortionColor.a;
    float bgDepth = texture2D(backgroundDepth, fragTexCoord).r;
    float fgDepth = texture2D(foregroundDepth, fragTexCoord).r;

    vec4 textureColor;

    if (fgDepth < bgDepth)
    {
        if (distortionMap > 0.f)
        {
            float amplitude = 0.003f; 
            float frequency = 50.0f;  
            float distortion = sin(fragTexCoord.y * frequency + offset) * amplitude;
            distortion = distortion * pow(distortionMap, 0.45);

            textureColor = texture2D(tex, vec2(fragTexCoord.x + distortion, fragTexCoord.y));
            //textureColor = vec4(pow(distortionMap, .45),0,0,1);
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