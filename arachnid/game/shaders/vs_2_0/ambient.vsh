
float4x4	_worldViewProj;

struct 		VS_INPUT
{
    float4		pos			: POSITION;
	float2		uv			: TEXCOORD0;
};

struct		VS_OUTPUT
{
    float4 		pos			: POSITION;
	float4		tex0		: TEXCOORD0;
};

VS_OUTPUT	VS(VS_INPUT In)
{
	VS_OUTPUT Out;

	Out.pos = mul( float4( In.pos.xyz, 1.0 ), _worldViewProj );
	Out.tex0 = float4( In.uv.xy, 0.0, 0.0 );

	return Out;
}

