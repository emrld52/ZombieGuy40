@vs vs
layout(location = 0) in vec3 a_pos;
layout(location = 1) in vec2 a_uv;

out vec2 v_uv;
out vec2 sprt_coord;
out vec2 tex_dim;
out vec2 sprt_count;

layout(binding=0) uniform quad_vs_params {
    mat4 projection;
    vec3 position;
    vec3 cam_position;
    vec2 sprite_coord;
    vec2 sprite_count;
    vec2 texture_atlas_dimensions;
};

void main() {
    gl_Position = projection * vec4(a_pos + position + (cam_position * -1), 1.0);
    v_uv = a_uv;
    sprt_coord = sprite_coord;
    tex_dim = texture_atlas_dimensions;
    sprt_count = sprite_count;
}
@end

@fs fs
in vec2 v_uv;
in vec2 sprt_coord;
in vec2 tex_dim;
in vec2 sprt_count;
out vec4 frag_color;

layout(binding = 0) uniform texture2D tex;
layout(binding = 1) uniform sampler samp;

void main() {
    vec2 offset = ((vec2(1, 1) / sprt_count) * vec2(sprt_coord.x - 1, sprt_coord.y - 1));

    frag_color = texture(sampler2D(tex, samp), 
        vec2(
            // stretch uv space to take up only the dimensions of each sprite
            offset.x + (v_uv.x * (1 / sprt_count.x)), 
            offset.y + (v_uv.y * (1 / sprt_count.y))
        )
    );
}
@end

@program quad vs fs