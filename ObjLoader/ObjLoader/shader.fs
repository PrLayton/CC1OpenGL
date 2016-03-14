
uniform sampler2D u_texture0;

varying vec2 v_texcoords;

varying vec4 v_color;
uniform vec4 u_color;

uniform bool u_postProcess; 

void main(void)
{
    vec4 texColor = texture2D(u_texture0, v_texcoords);
	if(u_postProcess == true){
		float lum = 0.2126 * texColor.r + 0.7152 * texColor.g + 0.0722 * texColor.b;
		// Normalise en nuance de gris
		gl_FragColor = vec4(lum, lum, lum, 1.0);
	}
	else{
		gl_FragColor = texColor;
	}

	//gl_FragColor = u_color * v_color;
}