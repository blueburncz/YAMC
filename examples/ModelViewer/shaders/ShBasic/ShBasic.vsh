attribute vec3 in_Position;
attribute vec3 in_Normal;
attribute vec2 in_TextureCoord;
attribute vec4 in_Color;
attribute vec4 in_TangentAndBitangentSign;

varying vec3 v_vPosition;
varying vec2 v_vTexCoord;
varying vec4 v_vColor;
varying mat3 v_mTBN;

void main()
{
	gl_Position = gm_Matrices[MATRIX_WORLD_VIEW_PROJECTION] * vec4(in_Position, 1.0);
	v_vPosition = (gm_Matrices[MATRIX_WORLD] * vec4(in_Position, 1.0)).xyz;
	v_vTexCoord = in_TextureCoord;
	v_vColor = in_Color;

	// Construct TBN matrix for normal mapping
	vec3 normal = in_Normal;
	vec3 tangent = in_TangentAndBitangentSign.xyz;
	vec3 bitangent = cross(normal, tangent) * in_TangentAndBitangentSign.w;

	v_mTBN = mat3(gm_Matrices[MATRIX_WORLD]) * mat3(tangent, bitangent, normal);
}
