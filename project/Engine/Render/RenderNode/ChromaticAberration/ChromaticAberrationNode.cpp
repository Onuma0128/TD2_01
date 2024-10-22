#include "ChromaticAberrationNode.h"

#include "Engine/DirectX/DirectXCommand/DirectXCommand.h"
#include "Engine/DirectX/PipelineState/PipelineState.h"
#include "Engine/DirectX/PipelineState/PSOBuilder/PSOBuilder.h"

#ifdef _DEBUG
#include "externals/imgui/imgui.h"
#endif // _DEBUG

ChromaticAberrationNode::ChromaticAberrationNode() = default;

ChromaticAberrationNode::~ChromaticAberrationNode() noexcept = default;

void ChromaticAberrationNode::initialize() {
	create_pipeline_state();
	primitiveTopology = D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	aberrationInfo.get_data()->center = Vector2{ 0.5f, 0.5f };
	aberrationInfo.get_data()->length = 0;
}

void ChromaticAberrationNode::draw() {
	auto&& command = DirectXCommand::GetCommandList();
	command->SetGraphicsRootConstantBufferView(0, aberrationInfo.get_resource()->GetGPUVirtualAddress());
	command->SetGraphicsRootDescriptorTable(1, textureGPUHandle);
	command->DrawInstanced(3, 1, 0, 0);
}

void ChromaticAberrationNode::set_texture_resource(const D3D12_GPU_DESCRIPTOR_HANDLE& textureGPUHandle_) {
	textureGPUHandle = textureGPUHandle_;
}

void ChromaticAberrationNode::create_pipeline_state() {
	RootSignatureBuilder rootSignatureBuilder;
	rootSignatureBuilder.add_cbv(D3D12_SHADER_VISIBILITY_PIXEL, 0);
	rootSignatureBuilder.add_texture(D3D12_SHADER_VISIBILITY_PIXEL);
	rootSignatureBuilder.sampler(
		D3D12_SHADER_VISIBILITY_PIXEL,
		0
	);

	ShaderBuilder shaderManager;
	shaderManager.initialize(
		"EngineResources/HLSL/FullscreenShader.hlsl",
		"EngineResources/HLSL/ChromaticAberration/ChromaticAberration.PS.hlsl"
	);

	std::unique_ptr<PSOBuilder> psoBuilder = std::make_unique<PSOBuilder>();
	psoBuilder->blendstate();
	psoBuilder->rasterizerstate();
	psoBuilder->rootsignature(rootSignatureBuilder.build());
	psoBuilder->shaders(shaderManager);
	psoBuilder->primitivetopologytype();
	psoBuilder->rendertarget();

	pipelineState = std::make_unique<PipelineState>();
	pipelineState->initialize(psoBuilder->get_rootsignature(), psoBuilder->build());
}

void ChromaticAberrationNode::set_length(float length) {
	aberrationInfo.get_data()->length = length;
}

void ChromaticAberrationNode::set_center(const Vector2& center) {
	aberrationInfo.get_data()->center = center;
}

#ifdef _DEBUG
void ChromaticAberrationNode::debug_gui() {
	if (ImGui::CollapsingHeader("ChromaticAberrationNode", ImGuiTreeNodeFlags_DefaultOpen)) {
		ImGui::DragFloat2("Center", &aberrationInfo.get_data()->center.x, 0.01f, -1.0f, 1.0f, "%.4f");
		ImGui::DragFloat("Length", &aberrationInfo.get_data()->length, 0.001f, 0.0f, 2.0f, "%.4f");
	}
}
#endif // _DEBUG
