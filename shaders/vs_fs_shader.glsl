@vs vs
layout(location = 0) in vec3 a_pos;
layout(location = 1) in vec2 a_uv;

out vec2 v_uv;

layout(binding=0) uniform quad_vs_params {
    mat4 projection;
    vec3 position;
};

void main() {
    gl_Position = projection * vec4(a_pos + position, 1.0);
    v_uv = a_uv;
}
@end

@fs fs
in vec2 v_uv;
out vec4 frag_color;

layout(binding = 0) uniform texture2D tex;
layout(binding = 1) uniform sampler samp;

void main() {
    frag_color = texture(sampler2D(tex, samp), v_uv);
}
@end

@program quad vs fs