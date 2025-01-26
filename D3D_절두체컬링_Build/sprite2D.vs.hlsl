// sprite2D.vs.hlsl
#pragma pack_matrix(row_major)
cbuffer constants
{
	matrix worldMatrix;
	matrix viewProjectionMatrix;
};
struct VertexInput
{
	float3 position : POSITION;
	float4 color : COLOR;
	float2 texcoord : TEXCOORD0;
	float3 normal : NORMAL;
};
struct VertexOutput
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
	float2 texcoord : TEXCOORD0;
};
VertexOutput main(VertexInput input)
{
	VertexOutput output;
	output.position = mul(float4(input.position, 1.0f), worldMatrix);
	output.position = mul(output.position, viewProjectionMatrix);
	output.color = input.color;
	output.texcoord = input.texcoord;
	return output;
}