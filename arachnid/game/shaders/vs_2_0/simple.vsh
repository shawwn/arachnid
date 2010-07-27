
float4x4	_worldViewProj;

struct 		VS_INPUT
{
    float4		Position	: POSITION;
	float2		UV			: TEXCOORD0;
};

struct		VS_OUTPUT
{
    float4 		Position	: POSITION;
	float2		UV			: TEXCOORD0;
};

VS_OUTPUT	VS(VS_INPUT In)
{
	VS_OUTPUT Out;

	Out.Position = mul(float4(In.Position.xyz, 1.0f), _worldViewProj);
	Out.UV = In.UV;

	return Out;
}

