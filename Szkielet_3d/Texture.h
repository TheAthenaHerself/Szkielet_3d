#pragma once

struct vertex_t {
    FLOAT position[3];
    FLOAT normal[3];
    FLOAT color[4];
    FLOAT tex_coord[2];
};

size_t const VERTEX_SIZE = sizeof(vertex_t) / sizeof(FLOAT);

vertex_t triangle_data[] = {
 { { -1.0f, -1.0f, 0.0f },   { 0.0f, 0.0f, -1.0f },
   { 1.0f, 1.0f, 1.0f, 1.0f },   { 0.0f, 1.0f } },
 { { -1.0f,  1.0f, 0.0f },   { 0.0f, 0.0f, -1.0f },
   { 1.0f, 1.0f, 1.0f, 1.0f },   { 0.0f, 0.0f } },
 { {  1.0f, -1.0f, 0.0f },   { 0.0f, 0.0f, -1.0f },
   { 1.0f, 1.0f, 1.0f, 1.0f },   { 1.0f, 1.0f } }
};

size_t const VERTEX_BUFFER_SIZE = sizeof(triangle_data);
size_t const NUM_VERTICES = VERTEX_BUFFER_SIZE / sizeof(vertex_t);

constexpr UINT BMP_PX_SIZE = 4;
UINT bmp_width = 0, bmp_height = 0;
BYTE* bmp_bits = nullptr;

auto GraphicPipelineState() {
	std::array inputElementDescs = {
		D3D12_INPUT_ELEMENT_DESC{
			.SemanticName = "POSITION",
			.SemanticIndex = 0,
			.Format = DXGI_FORMAT_R32G32B32_FLOAT,
			.InputSlot = 0,
			.AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT,
			.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			.InstanceDataStepRate = 0
		},
		D3D12_INPUT_ELEMENT_DESC{
			.SemanticName = "NORMAL",
			.SemanticIndex = 0,
			.Format = DXGI_FORMAT_R32G32B32_FLOAT,
			.InputSlot = 0,
			.AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT,
			.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			.InstanceDataStepRate = 0
		},
		D3D12_INPUT_ELEMENT_DESC{
			.SemanticName = "COLOR",
			.SemanticIndex = 0,
			.Format = DXGI_FORMAT_R32G32B32A32_FLOAT,
			.InputSlot = 0,
			.AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT,
			.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			.InstanceDataStepRate = 0
		},
		D3D12_INPUT_ELEMENT_DESC{
			.SemanticName = "TEXCOORD",
			.SemanticIndex = 0,
			.Format = DXGI_FORMAT_R32G32_FLOAT,
			.InputSlot = 0,
			.AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT,
			.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			.InstanceDataStepRate = 0
		  }
	};
	return inputElementDescs;
}

HRESULT LoadBitmapFromFile(
	PCWSTR uri, UINT& width, UINT& height, BYTE** ppBits, ComPtr<IWICImagingFactory> wic_factory);

void RootSignature(ComPtr<ID3D12RootSignature> m_rootSignature, ComPtr<ID3D12Device> m_device);

void ConstBuf(ComPtr<ID3D12Device> m_device, ComPtr<ID3D12DescriptorHeap> constbufDescHeap);