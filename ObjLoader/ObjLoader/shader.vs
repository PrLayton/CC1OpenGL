
// Variable en entr�e
// Attribue fait r�f�rence � des sommets donc il ne peut �tre d�fini qu'ici, dans le vertex shader
attribute vec4 a_position;
attribute vec2 a_texcoords;
attribute vec3 a_normal;

// Coordonn�es de la texture en sortie pour le fs
varying vec2 v_texcoords;

// Matrices de transformation
uniform mat4 u_rotationMatrix;
uniform mat4 u_rotationMatrix2;
uniform mat4 u_scaleMatrix; //worldMatrix
uniform mat4 u_projectionMatrix;

void main(void)
{
	v_texcoords = a_texcoords;
	gl_Position = /*u_projectionMatrix **/ u_rotationMatrix2 * u_rotationMatrix * u_scaleMatrix *  a_position;
}	
