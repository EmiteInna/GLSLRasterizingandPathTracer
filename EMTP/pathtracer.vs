#version 460 core
layout (location=0) in vec3 position;

out vec2 coord;

void main(){
	gl_Position=vec4(position,1);
	coord=position.xy;
}