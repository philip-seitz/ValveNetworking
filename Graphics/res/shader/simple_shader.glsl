#shader vertex
#version 330 core
layout(location = 0) in vec4 a_Position;

uniform mat4 u_Model;
uniform mat4 u_Projection;
uniform mat4 u_View;

void main()
{
	gl_Position = u_Projection * u_View * u_Model * a_Position;
}

#shader fragment
#version 330 core
layout(location = 0) out vec4 o_Color;

void main()
{
	o_Color = vec4(0.3f, 0.3f, 0.9f, 1.0f);
}