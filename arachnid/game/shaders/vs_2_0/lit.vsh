
float4x4	_worldViewProj;
float4		_lightPos;
float4		_lightRadius;

struct 		VS_INPUT
{
    float4		pos			: POSITION;

	float3		tangent		: TANGENT;
	float3		binormal	: BINORMAL;
	float3		normal		: NORMAL;

	float2		uv			: TEXCOORD0;
};

struct		VS_OUTPUT
{
    float4 		pos			: POSITION;
	float4		tex0		: TEXCOORD0;
	float4		tex1		: TEXCOORD1;
	float4		tex2		: TEXCOORD2;
	float4		tex3		: TEXCOORD3;
};

VS_OUTPUT	VS(VS_INPUT In)
{
	VS_OUTPUT Out;

	float3 worldPos = In.pos.xyz;
	float3 lightPos = _lightPos.xyz;
	float3 lightDir = normalize( lightPos - worldPos );
	float lightPercent = distance( lightPos, worldPos ) / _lightRadius.x;

	Out.pos = mul( float4( In.pos.xyz, 1.0 ), _worldViewProj );

	float3 outLightDir;
	outLightDir.x = dot( lightDir, In.tangent );
	outLightDir.y = dot( lightDir, In.binormal );
	outLightDir.z = dot( lightDir, In.normal );

	Out.tex0 = float4( In.uv.xy, 0.0, 0.0 );
	Out.tex1 = float4( outLightDir.xyz, lightPercent );
	Out.tex2 = float4( lightPos.xyz, 1.0 );
	Out.tex3 = float4( In.normal.xyz, 1.0 );

	return Out;
}

