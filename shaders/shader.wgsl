@vertex 
fn vertex_main(@builtin(vertex_index) i : u32) ->
    @builtin(position) vec4f {
    const pos = array(vec2f(0, 1), vec2f(-1, -1), vec2f(1, -1));
    return vec4f(pos[i], 0, 1);
}
@fragment 
fn fragment_main() -> @location(0) vec4f {
    return vec4f(0.1, 0.4, 0, 1);
}
