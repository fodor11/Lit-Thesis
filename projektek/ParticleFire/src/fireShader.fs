#version 400

uniform sampler2D tex;

in vec3 fragVert;
in vec2 fragTexCoord;
in vec2 fragTexCoord2;
in float currentAlpha;

out vec4 finalColor;

void main() {
	vec4 textureColor = texture(tex, fragTexCoord);
	vec4 textureColor2 = texture(tex, fragTexCoord2);
    if (fragTexCoord.x == -1 && fragTexCoord.y == -1){
	    finalColor = vec4(1,1,1,1);
    }
    else{
        finalColor = vec4(currentAlpha * textureColor.rgba + (1 - currentAlpha) * textureColor2.rgba);
    }

    //finalColor = vec4(1,1,1,1);
}