
// Variable en entrée
// Attribue fait référence à des sommets donc il ne peut être défini qu'ici, dans le vertex shader
attribute vec4 a_position;
attribute vec2 a_texcoords;

varying vec2 v_texcoords;

uniform mat4 u_rotationMatrix;
uniform mat4 u_scaleMatrix; //worldMatrix
uniform mat4 u_projectionMatrix;

void main(void)
{
	v_texcoords = a_texcoords;
	gl_Position = /*u_projectionMatrix **/ u_rotationMatrix * /*u_scaleMatrix **/ a_position;
}