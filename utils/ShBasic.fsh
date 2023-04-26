varying vec3 v_vPosition;
varying vec2 v_vTexCoord;
varying vec4 v_vColor;
varying mat3 v_mTBN;

// Camera position in world-space
uniform vec3 u_vCamera;
// Tangent-space normal map
uniform sampler2D u_texNormal;
// Specular color (linear)
uniform sampler2D u_texSpecular;
// Glossiness
uniform sampler2D u_texGloss;

#define PI 3.14159265359

float Pow3(float x)
{
	return x * x * x;
}

vec3 GammaToLinear(vec3 color)
{
	return pow(color, vec3(2.2));
}

vec3 LinearToGamma(vec3 color)
{
	return pow(color, vec3(1.0 / 2.2));
}

vec3 TonemapReinhard(vec3 color)
{
	return color / (vec3(1.0) + color);
}

vec3 TonemapExposure(vec3 color, float exposure)
{
	return vec3(1.0) - exp(-color * exposure);
}

vec3 LambertNormalized(vec3 diffuseColor)
{
	return diffuseColor / PI;
}

// Source: http://www.thetenthplanet.de/archives/255
vec3 BlinnPhongNormalized(vec3 specularColor, float specularPower, float NdotH, float LdotH)
{
	return specularColor * ((specularPower + 1.0) * pow(NdotH, specularPower)) / (8.0 * PI * Pow3(LdotH));
}

void main()
{
	// Unpack material and convert to linear color space
	vec4 baseOpacity = texture2D(gm_BaseTexture, v_vTexCoord);
	vec3 baseColor = GammaToLinear(v_vColor.rgb * baseOpacity.rgb);
	vec3 normal = normalize(texture2D(u_texNormal, v_vTexCoord).rgb * 2.0 - 1.0);
	vec3 specularColor = texture2D(u_texSpecular, v_vTexCoord).rgb;
	float gloss = texture2D(u_texGloss, v_vTexCoord).x;
	float specularPower = exp2(10.0 * gloss + 1.0);
	vec3 N = normalize(v_mTBN * normal);

	// Accumulate lighting...
	vec3 V = normalize(u_vCamera - v_vPosition);
	vec3 light = vec3(0.0);

	// Ambient
	light += GammaToLinear(vec3(0.2)) * baseColor;

	// Directional
	vec3 L = normalize(-vec3(-1.0));
	vec3 H = normalize(L + V);
	float NdotL = max(dot(N, L), 0.0);
	float NdotH = max(dot(N, H), 0.0);
	float LdotH = max(dot(L, H), 0.0);

	light += GammaToLinear(vec3(1.0)) * NdotL * (LambertNormalized(baseColor)
		+ BlinnPhongNormalized(specularColor, specularPower, NdotH, LdotH));

	// Tonemap and convert back to gamma
	gl_FragColor.rgb = LinearToGamma(TonemapExposure(light, 3.0));
	gl_FragColor.a = v_vColor.a * baseOpacity.a;
}
