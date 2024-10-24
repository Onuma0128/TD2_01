#include "CircleGaugeNode.h"

#include "Engine/DirectX/DirectXSwapChain/DirectXSwapChain.h"
#include "Engine/DirectX/PipelineState/PipelineState.h"
#include "Engine/DirectX/PipelineState/PSOBuilder/PSOBuilder.h"

void CircleGaugeNode::initialize() {
	create_pipeline_state();
	pipelineState->set_name("CircleGaugeNode");
	primitiveTopology = D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	depthStencil = DirectXSwapChain::GetDepthStencil();
}

void CircleGaugeNode::create_pipeline_state() {
	RootSignatureBuilder rootSignatureBuilder;
	rootSignatureBuilder.add_cbv(D3D12_SHADER_VISIBILITY_VERTEX, 0); // 0 :  transform
	rootSignatureBuilder.add_cbv(D3D12_SHADER_VISIBILITY_VERTEX, 1); // 1 : camera
	rootSignatureBuilder.add_cbv(D3D12_SHADER_VISIBILITY_PIXEL, 0); // 2 : material
	rootSignatureBuilder.add_cbv(D3D12_SHADER_VISIBILITY_PIXEL, 1); // 3 : percentage
	rootSignatureBuilder.add_texture(D3D12_SHADER_VISIBILITY_PIXEL); // 4 : texture
	rootSignatureBuilder.sampler( // sampler
		D3D12_SHADER_VISIBILITY_PIXEL,
		0,
		D3D12_FILTER_ANISOTROPIC
	);

	InputLayoutBuilder inputLayoutBuilder;
	inputLayoutBuilder.add_element("POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);
	inputLayoutBuilder.add_element("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT);
	inputLayoutBuilder.add_element("NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT);

	ShaderBuilder shaderManager;
	shaderManager.initialize(
		"Resources/GameScene/HLSL/CircleGauge/CircleGauge3D.VS.hlsl",
		"Resources/GameScene/HLSL/CircleGauge/CircleGauge3D.PS.hlsl"
	);

	std::unique_ptr<PSOBuilder> psoBuilder = std::make_unique<PSOBuilder>();
	psoBuilder->blendstate();
	psoBuilder->depthstencilstate(*DirectXSwapChain::GetDepthStencil());
	psoBuilder->inputlayout(inputLayoutBuilder.build());
	psoBuilder->rasterizerstate();
	psoBuilder->rootsignature(rootSignatureBuilder.build());
	psoBuilder->shaders(shaderManager);
	psoBuilder->primitivetopologytype();
	psoBuilder->rendertarget();

	pipelineState = std::make_unique<PipelineState>();
	pipelineState->initialize(psoBuilder->get_rootsignature(), psoBuilder->build());

}
