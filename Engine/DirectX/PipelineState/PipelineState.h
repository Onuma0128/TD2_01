#pragma once

#include <d3d12.h>
#include <wrl/client.h>

class PipelineState final {
public:
	PipelineState() noexcept;
	~PipelineState() noexcept;

private:
	PipelineState(const PipelineState&) = delete;
	PipelineState operator=(const PipelineState&) = delete;

public:
	void initialize(
		Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_,
		Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState_
	);
	void set_root_signature();
	void set_graphics_pipeline_state();

private:
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState;
};
