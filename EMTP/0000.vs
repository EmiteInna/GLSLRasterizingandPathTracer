#version 330 core
layout (location=0) in vec4 position;
layout (location=1) in vec4 texcoord;
layout (location=2) in vec4 normal;
layout (location=3) in vec4 tangent;
layout (location=4) in vec4 color;
out vec4 vertexColor;
out vec4 uv;
out vec4 normalWS;
out vec4 tangentWS;
out vec4 positionWS;
out float texnum;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;

void main(){
	vec4 positionOS=vec4(position.xyz,1);
	positionWS=M*positionOS;
	normalWS=normal;
	gl_Position=P*V*M*positionOS;
	uv=texcoord;
	texnum=normal.w;
}