#version 400

in vec2 vert;

out vec2 fragTexCoord;

void main() {

	fragTexCoord = (vert + 1.0) / 2.0;
    gl_Position = vec4(vert, 0.f, 1.f);
}