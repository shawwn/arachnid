
float4x4	_worldViewProj;

struct 		VS_INPUT
{
    float4		Position	: POSITION;
};

struct		VS_OUTPUT
{
    float4 		Position	: POSITION;
};

VS_OUTPUT	VS(VS_INPUT In)
{
	VS_OUTPUT Out;

	Out.Position = mul(float4(In.Position.xyz, 1.0f), _worldViewProj);

	return Out;
}

