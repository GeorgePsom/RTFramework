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
	// Chromatic Abreviation
	color.r = g_texture.Sample(g_sampler, texCoord - float2(0.001f, 0.001f)).r;
	color.g = g_texture.Sample(g_sampler, texCoord /*- float2(0.0015f, 0.0015f)*/).g;
	color.b = g_texture.Sample(g_sampler, texCoord - float2(0.002f, 0.002f)).b;


	
	// Vignette
	float falloff = 0.5f;
	float2 coord = (input.uv - 0.5)  * 2;
	float rf = sqrt(dot(coord, coord)) * 1.0f;
	float rf2_1 = rf * rf + 1.0;
	float e = 1.0 / (rf2_1 * rf2_1);

	
	color *= e;
	float gamma = 1.0f / 2.2f;
	
	color = float3(pow(color.r, gamma), pow(color.g, gamma), pow(color.b, gamma));
	return float4(color.rgb, 1.0f);
}