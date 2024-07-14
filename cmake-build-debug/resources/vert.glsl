#version 150

in  	vec3 origin;
in  	vec2 uv;

out 	vec2 f_uv;

uniform mat4 view_mat;
uniform mat4 proj_mat;

void main(void) {
    gl_Position = proj_mat * view_mat * vec4(origin, 1.0); 
    f_uv        = uv;
}