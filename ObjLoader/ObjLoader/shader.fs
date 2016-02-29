
uniform sampler2D u_texture0;

varying vec2 v_texcoords;

void main(void)
{
    vec4 texColor = texture2D(u_texture0, v_texcoords);

    gl_FragColor = texColor;
}