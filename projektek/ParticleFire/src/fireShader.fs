#version 400

uniform sampler2D tex;

in vec3 fragVert;
in vec2 fragTexCoord;
in vec2 fragTexCoord2;
in float currentBlend;

out vec4 finalColor;

void main() {
	vec4 textureColor = texture(tex, fragTexCoord);
	vec4 textureColor2 = texture(tex, fragTexCoord2);
    if (fragTexCoord.x == -1 && fragTexCoord.y == -1){
	    finalColor = vec4(1,1,1,1);
    }
    else{
        finalColor = vec4(currentBlend * textureColor.rgba + (1 - currentBlend) * textureColor2.rgba);
    }

    //finalColor = vec4(1,1,1,1);
}