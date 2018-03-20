#version 150

uniform mat4 model;
uniform mat3 normalMatrix;
uniform vec3 viewPosition;
uniform sampler2D tex;

uniform struct Light
{
	vec3 position;
	vec3 ambientIntensities;
	vec3 diffuseIntensities;
	vec3 specularIntensities;
} light;

in vec3 fragVert;
in vec2 fragTexCoord;
in vec3 fragAmbientColor;
in vec3 fragDiffuseColor;
in vec3 fragSpecularColor;
in float fragShininess;
in vec3 fragNormal;

out vec4 finalColor;

void main() {
	// transform back the normal into world coordinates
	vec3 normal = normalize(fragNormal * normalMatrix);
	
	// calculate the location of this fragment (pixel) in world coordinates
	vec3 fragPosition = vec3(model * vec4(fragVert, 1.0));

	// vector from the surface to the light
	vec3 surfaceToLight = normalize(light.position - fragPosition);

	// cosine of the angle of incidence
	float cosAngle = dot(normal, surfaceToLight);
	cosAngle = max(cosAngle, 0.0);

	vec4 textureColor = texture(tex, fragTexCoord);

	vec3 ambientColor = fragAmbientColor * light.ambientIntensities;

	vec3 diffuseColor = fragDiffuseColor * light.diffuseIntensities * cosAngle;

	vec3 viewDirection = normalize(viewPosition - fragPosition);
	vec3 reflectDirection = reflect(-surfaceToLight, normal);

	float specular = pow(max(dot(viewDirection, reflectDirection), 0.0), fragShininess);
	vec3 specularColor = fragSpecularColor * specular * light.specularIntensities;
    
	finalColor = vec4((ambientColor + diffuseColor + specularColor) * textureColor.rgb, textureColor.a);
}