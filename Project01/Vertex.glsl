#version 330 core
//--- in_Position: attribute index 0
//--- in_Color: attribute index 1

layout (location = 0) in vec3 in_Position; //--- 위치 변수: attribute position 0
layout (location = 1) in vec3 in_Color; //--- 컬러 변수: attribute position 1
out vec3 out_Color; //--- 프래그먼트 세이더에게 전달

void main(void)
{
	// movenet_vec를 이용해 정점들을 매 프레임 마다 위치 변환
	// 

	gl_Position = vec4 (in_Position.x, in_Position.y, in_Position.z, 1.0);
	out_Color = in_Color;
}
