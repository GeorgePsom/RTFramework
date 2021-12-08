struct PSInput
{
	float4 position : SV_POSITION;
	float2 uv: TEXCOORD;
};

Texture2D g_texture : register(t0);
SamplerState g_sampler : register(s0);

PSInput VSMain(float4 position : POSITION, float2 uv : TEXCOORD)
{
	PSInput result;

	result.position = position;
	result.uv = uv;

	return result;
}

float4 PSMain(PSInput input) : SV_TARGET
{
	// Barrel Distortion
	float BarrelPower = 1.0f;
	float2 ndc = input.uv * 2.0f - 1.0f;

	float theta = atan2(ndc.y, ndc.x);
	float radius = length(ndc);
	radius = pow(radius, BarrelPower);
	ndc.x = radius * cos(theta);
	ndc.y = radius * sin(theta);
	float2 texCoord =  0.5f * (ndc + 1.0f);

	float3 color;
	// Chromatic Aberration
	color.r = g_texture.Sample(g_sampler, texCoord - float2(0.001f, 0.001f)).r;
	color.g = g_texture.Sample(g_sampler, texCoord).g;
	color.b = g_texture.Sample(g_sampler, texCoord - float2(0.002f, 0.002f)).b;


	
	// Vignette
	const float outerRadius = 0.6;
	const float innerRadius = 0.2f;
	const float intensity = 0.8f;
	float2 pos = input.uv - 0.5f;
	float len = length(pos);
	float vignette = smoothstep(outerRadius, innerRadius, len);
	color.rgb = lerp(color.rgb, color.rgb * vignette, intensity);

	
	/*color *= e;*/
	float gamma = 1.0f / 2.2f;
	
	color = float3(pow(color.r, gamma), pow(color.g, gamma), pow(color.b, gamma));
	return float4(color.rgb, 1.0f);
}