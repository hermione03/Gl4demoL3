#version 330

layout (location = 0) in vec3 vsiPosition;

out vec2 vsoPosition;

uniform mat4 projectionMatrix, viewMatrix, modelMatrix;

void main(void) {

  gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(vsiPosition, 1.0);
  vsoPosition =  vsiPosition.xy;
}
