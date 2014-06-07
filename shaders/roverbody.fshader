#version 130

uniform sampler2D uTexUnit;

in vec3 vNormal;
in vec3 vPosition;
in vec2 vTexCoord;

out vec4 fragColor;

void main() 
{
    gl_FragColor = texture(uTexUnit, vTexCoord);
}
