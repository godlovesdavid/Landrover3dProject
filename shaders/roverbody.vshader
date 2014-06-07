#version 130

uniform mat4 uProjMatrix;
uniform mat4 uModelViewMatrix;
uniform mat4 uNormalMatrix;

in vec3 aPosition; //obj coords
in vec3 aNormal;
in vec2 aTexCoord;

out vec3 vNormal; //eye coords
out vec3 vPosition;
out vec2 vTexCoord;

void main() 
{
    vNormal = vec3(uNormalMatrix * vec4(aNormal, 0.0));
    vec4 tPosition = uModelViewMatrix * vec4(aPosition, 1.0);
    vPosition = vec3(tPosition);

    gl_Position = uProjMatrix * tPosition; //clip coords

    vTexCoord = aTexCoord;
}
