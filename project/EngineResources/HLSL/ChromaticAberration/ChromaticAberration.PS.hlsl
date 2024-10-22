struct VertexShaderOutput {
	float4 position : SV_POSITION;
	float2 texcoord : TEXCOORD0;
};

struct ChromaticAberrationData {
	float2 center;
	float length;
};

Texture2D<float4> gTexture : register(t0);
ConstantBuffer<ChromaticAberrationData> AberrationInfo : register(b0);
SamplerState gSampler : register(s0);

float4 main(VertexShaderOutput input) : SV_TARGET0 {
	float2 samplaDirection = input.texcoord - AberrationInfo.center;
	samplaDirection *= AberrationInfo.length;
	
	float red = gTexture.Sample(gSampler, input.texcoord).r;
	float green = gTexture.Sample(gSampler, input.texcoord - samplaDirection).g;
	float blue = gTexture.Sample(gSampler, input.texcoord - samplaDirection * 2).b;
	
	float4 outputColor = float4(red, green, blue, 1.0f);
	return outputColor;
}