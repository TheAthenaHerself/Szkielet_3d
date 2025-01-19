#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <memory>
using std::shared_ptr;
#include "Throws.h"
#include "Application3D.h"

using Microsoft::WRL::ComPtr;

#include "RootSignature.h"

void RootSignature(shared_ptr<Application_3d> &app) {
	D3D12_DESCRIPTOR_RANGE desc_range = {
			.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV,
			.NumDescriptors = 1,
			.BaseShaderRegister = 0,
			.RegisterSpace = 0,
			.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND,
	};

	D3D12_ROOT_PARAMETER root_parameter[] = { {
		.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE,
		.DescriptorTable = { 1, &desc_range },
		.ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX,
	} };

	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc = {
		.NumParameters = _countof(root_parameter),
		.pParameters = root_parameter,
		.NumStaticSamplers = 0,
		.pStaticSamplers = nullptr,
		.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS
	};

	ComPtr<ID3DBlob> signature;
	ComPtr<ID3DBlob> error;
	ThrowIfFailed(D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, 
		&signature, &error), "serialize root signature");
	ThrowIfFailed(app->m_device->CreateRootSignature(0, signature->GetBufferPointer(), 
		signature->GetBufferSize(), IID_PPV_ARGS(&app->m_rootSignature)), "create root signature");
}