#version 330 core
layout (location=0) in vec3 position;



uniform mat4 V;
uniform mat4 P;

void main(){
	vec4 positionWS=vec4(position,1);
	gl_Position=P*V*positionWS;
//	gl_Position=vec4(position,1);
	gl_PointSize=1;
}