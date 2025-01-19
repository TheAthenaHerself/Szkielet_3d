#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <memory>
using std::shared_ptr;
#include "Throws.h"
#include "Application3D.h"

using Microsoft::WRL::ComPtr;

#include "vertex_shader.h"
#include "pixel_shader.h"
#include "GraphicsPipelineState.h"

namespace {
	// just triangle
	D3D12_INPUT_ELEMENT_DESC inputElementDescs2[] = {
	{
		.SemanticName = "POSITION",
		.SemanticIndex = 0,
		.Format = DXGI_FORMAT_R32G32B32_FLOAT,
		.InputSlot = 0,
		.AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT,
		.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
		.InstanceDataStepRate = 0
	},
	{
		.SemanticName = "COLOR",
		.SemanticIndex = 0,
		.Format = DXGI_FORMAT_R32G32B32A32_FLOAT,
		.InputSlot = 0,
		.AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT,
		.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
		.InstanceDataStepRate = 0
	}
	};

	// just cube i think

	D3D12_INPUT_ELEMENT_DESC inputElementDescs3[] = {
		{
			.SemanticName = "POSITION",
			.SemanticIndex = 0,
			.Format = DXGI_FORMAT_R32G32B32_FLOAT,
			.InputSlot = 0,
			.AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT,
			.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			.InstanceDataStepRate = 0
		},
		{
			.SemanticName = "NORMAL",
			.SemanticIndex = 0,
			.Format = DXGI_FORMAT_R32G32B32_FLOAT,
			.InputSlot = 0,
			.AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT,
			.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			.InstanceDataStepRate = 0
		},
		{
			.SemanticName = "COLOR",
			.SemanticIndex = 0,
			.Format = DXGI_FORMAT_R32G32B32A32_FLOAT,
			.InputSlot = 0,
			.AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT,
			.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			.InstanceDataStepRate = 0
		}
	};

	//shaded cube i think

	D3D12_INPUT_ELEMENT_DESC inputElementDescs7[] = {
		{
		.SemanticName = "POSITION",
		.SemanticIndex = 0,
		.Format = DXGI_FORMAT_R32G32B32_FLOAT,
		.InputSlot = 0,
		.AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT,
		.InputSlotClass =
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
		.InstanceDataStepRate = 0
		},
		{
		.SemanticName = "COLOR",
		.SemanticIndex = 0,
		.Format = DXGI_FORMAT_R32G32B32A32_FLOAT,
		.InputSlot = 0,
		.AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT,
		.InputSlotClass =
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
		.InstanceDataStepRate = 0
		},
		{ // Pierwszy wiersz macierzy instancji
		.SemanticName = "WORLD",
		.SemanticIndex = 0,
		.Format = DXGI_FORMAT_R32G32B32A32_FLOAT,
		.InputSlot = 1,
		.AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT,
		.InputSlotClass =
			D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA,
		.InstanceDataStepRate = 1
		},
		{  // Drugi wiersz macierzy instancji
		.SemanticName = "WORLD",
		.SemanticIndex = 1,
		.Format = DXGI_FORMAT_R32G32B32A32_FLOAT,
		.InputSlot = 1,
		.AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT,
		.InputSlotClass =
			D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA,
		.InstanceDataStepRate = 1
		},
		{  // Trzeci wiersz macierzy instancji
		.SemanticName = "WORLD",
		.SemanticIndex = 2,
		.Format = DXGI_FORMAT_R32G32B32A32_FLOAT,
		.InputSlot = 1,
		.AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT,
		.InputSlotClass =
			D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA,
		.InstanceDataStepRate = 1
		},
		{  // Czwarty wiersz macierzy instancji
		.SemanticName = "WORLD",
		.SemanticIndex = 3,
		.Format = DXGI_FORMAT_R32G32B32A32_FLOAT,
		.InputSlot = 1,
		.AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT,
		.InputSlotClass =
			D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA,
		.InstanceDataStepRate = 1
		}
	};
}

void GraphicsPipelineState(shared_ptr<Application_3d>& app) {
	

	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {
		.pRootSignature = app->m_rootSignature.Get(),
		.VS = { vs_main, sizeof(vs_main) }, // bytecode vs w tablicy vs_main
		.PS = { ps_main, sizeof(ps_main) }, // bytecode ps w tablicy ps_main
		.BlendState = {
			.AlphaToCoverageEnable = FALSE,
			.IndependentBlendEnable = FALSE,
			.RenderTarget = {
				{
				.BlendEnable = FALSE,
				.LogicOpEnable = FALSE,
				.SrcBlend = D3D12_BLEND_ONE,
				.DestBlend = D3D12_BLEND_ZERO,
				.BlendOp = D3D12_BLEND_OP_ADD,
				.SrcBlendAlpha = D3D12_BLEND_ONE,
				.DestBlendAlpha = D3D12_BLEND_ZERO,
				.BlendOpAlpha = D3D12_BLEND_OP_ADD,
				.LogicOp = D3D12_LOGIC_OP_NOOP,
				.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL
				}
				}
		},
		.SampleMask = UINT_MAX,
		.RasterizerState = {
			.FillMode = D3D12_FILL_MODE_SOLID,
			.CullMode = D3D12_CULL_MODE_BACK,
			.FrontCounterClockwise = FALSE,
			.DepthBias = D3D12_DEFAULT_DEPTH_BIAS,
			.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP,
			.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS,
			.DepthClipEnable = TRUE,
			.MultisampleEnable = FALSE,
			.AntialiasedLineEnable = FALSE,
			.ForcedSampleCount = 0,
			.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF
		},
		.DepthStencilState = {
			.DepthEnable = TRUE,
			.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL,
			.DepthFunc = D3D12_COMPARISON_FUNC_LESS,
			.StencilEnable = FALSE,
			.StencilReadMask = D3D12_DEFAULT_STENCIL_READ_MASK,
			.StencilWriteMask = D3D12_DEFAULT_STENCIL_READ_MASK,
			.FrontFace = {
				.StencilFailOp = D3D12_STENCIL_OP_KEEP,
				.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP,
				.StencilPassOp = D3D12_STENCIL_OP_KEEP,
				.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS
			},
			.BackFace = {
				.StencilFailOp = D3D12_STENCIL_OP_KEEP,
				.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP,
				.StencilPassOp = D3D12_STENCIL_OP_KEEP,
				.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS
			},
		},
		.InputLayout = { inputElementDescs2, _countof(inputElementDescs2) },
		.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED,
		.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
		.NumRenderTargets = 1,
		.RTVFormats = { DXGI_FORMAT_R8G8B8A8_UNORM },
		.DSVFormat = DXGI_FORMAT_D32_FLOAT,
		.SampleDesc = {.Count = 1, .Quality = 0 },
	};
	ThrowIfFailed(app->m_device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&(app->m_pipelineState))), "create graphics pipeline state");
}