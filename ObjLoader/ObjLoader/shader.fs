
uniform sampler2D u_texture0;

varying vec2 v_texcoords;

uniform bool u_postProcess; 

void main(void)
{
    vec4 texColor = texture2D(u_texture0, v_texcoords);

	// Si on veux afficher le rendu avec le post processing
	if(u_postProcess == true){
		// Calcul de la luminance
		float lum = 0.2126 * texColor.r + 0.7152 * texColor.g + 0.0722 * texColor.b;
		// Normalise en nuance de gris
		gl_FragColor = vec4(lum, lum, lum, 1.0);
	}
	else{
		gl_FragColor = texColor;
	}
}