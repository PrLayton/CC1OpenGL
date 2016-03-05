
uniform sampler2D u_texture0;

varying vec2 v_texcoords;

varying vec4 v_color;
uniform vec4 u_color;

void main(void)
{
    vec4 texColor = texture2D(u_texture0, v_texcoords);
    gl_FragColor = texColor;

	//gl_FragColor = u_color * v_color;
}