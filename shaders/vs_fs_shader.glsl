@vs vs
layout(location = 0) in vec3 a_pos;
layout(location = 1) in vec2 a_uv;

out vec2 v_uv;
out vec2 resolution;

layout(binding=0) uniform quad_vs_params {
    mat4 projection;
    vec3 position;
    vec2 scale;
    vec3 cam_position;
    int flip_x;
};

void main() {
    vec3 b_pos;

    if(flip_x == 1) 
    {
        b_pos = vec3(a_pos.x * -1, a_pos.y, a_pos.z);
    }
    else 
    {
        b_pos = vec3(a_pos.x, a_pos.y, a_pos.z);
    }

    gl_Position = projection * vec4(vec3(b_pos.x * scale.x, b_pos.y * scale.y, b_pos.z) + position + (cam_position * -1), 1.0);
    v_uv = a_uv;
    resolution = scale;
}
@end

@fs fs
in vec2 v_uv;
in vec2 resolution;
out vec4 frag_color;

layout(binding = 0) uniform texture2D tex;
layout(binding = 1) uniform sampler samp;

layout(binding=2) uniform quad_fs_params {
    vec2 sprite_coord;
    vec2 sprite_count;
    vec2 texture_atlas_dimensions;
    vec2 texture_offset;
};

void main() {
    vec2 offset = ((vec2(1, 1) / sprite_count) * vec2(sprite_coord.x - 1, sprite_coord.y - 1));

    vec2 sprite_uv_size = vec2(
        resolution.x / texture_atlas_dimensions.x,
        resolution.y / texture_atlas_dimensions.y
    );

    vec2 sprite_start = ((vec2(sprite_coord.x - 1, sprite_coord.y - 1)) 
                        * vec2(1.0) / sprite_count);

    vec2 wrapped_uv = mod(v_uv * sprite_uv_size + texture_offset, sprite_uv_size);

    vec2 final_uv = sprite_start + wrapped_uv;

    frag_color = texture(sampler2D(tex, samp), final_uv);

}
@end

@program quad vs fs