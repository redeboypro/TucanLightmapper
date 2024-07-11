#version 150

in 	vec2 f_uv;

out 	vec4 out_col;

uniform sampler2D main_tex;

void main(void) {
   out_col = texture(main_tex, f_uv);
}