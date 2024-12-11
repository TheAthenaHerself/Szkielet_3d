#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <stdexcept>
#include <chrono>
#include <directxmath.h>

#include "vertex_shader.h"
#include "pixel_shader.h"
#include "Throws.h"
#include "Direct3d.h"

using Microsoft::WRL::ComPtr;
using DirectX::XMFLOAT4X4;
using DirectX::XMFLOAT4;
using DirectX::XMMATRIX;
using DirectX::XMMatrixMultiply;
using DirectX::XMMatrixRotationY;
using DirectX::XMMatrixRotationX;
using DirectX::XMMatrixTranslation;
using DirectX::XMMatrixPerspectiveFovLH;

namespace {
	static const UINT FrameCount = 2;
	UINT64 width = 0;
	UINT height = 0;

	HWND hwnd;

	struct vertex_t {
		FLOAT position[3];
		FLOAT normal[3];
		FLOAT color[4];
	};

	size_t const VERTEX_SIZE = sizeof(vertex_t) / sizeof(FLOAT);
	/*vertex_t triangle_data[] = {
	  { 0.0f, 1.0f, 0.5f,         0.0f, 1.0f, 0.0f, 1.0f },
	  { 1.0f, 0.0f, 0.5f,         1.0f, 0.0f, 0.0f, 1.0f },
	  { -1.0f, -0.5f, 0.5f,       1.0f, 1.0f, 1.0f, 1.0f }
	};*/

	vertex_t box_data[] = {
		{ {-1.0f, 1.0f, -1.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 0.5f, 1.0f}},
		{  {1.0f, 1.0f, -1.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 0.5f, 1.0f}},
		{{-1.0f, -1.0f, -1.0f}, {1.0f, 0.0f, 0.0f}, {0.5f, 0.5f, 0.0f, 1.0f}},
	};

	/*vertex_t box_data[] = {
		// front
		{ {-1.0f, 1.0f, -1.0f}, {1.0f, 1.0f, 0.5f, 1.0f}},
		{  {1.0f, 1.0f, -1.0f}, {1.0f, 1.0f, 0.5f, 1.0f}},
		{{-1.0f, -1.0f, -1.0f}, {0.5f, 0.5f, 0.0f, 1.0f}},

		{{1.0f, 1.0f, -1.0f}, {1.0f, 1.0f, 0.5f, 1.0f}},
		{ {1.0f, -1.0f, -1.0f}, {0.5f, 0.5f, 0.0f, 1.0f}},
		{{-1.0f, -1.0f, -1.0f}, {0.5f, 0.5f, 0.0f, 1.0f}},

		// right
		{  {1.0f, 1.0f, -1.0f}, {1.0f, 0.5f, 0.0f, 1.0f}},
		{   {1.0f, 1.0f, 1.0f}, {1.0f, 0.5f, 0.0f, 1.0f}},
		{ {1.0f, -1.0f, -1.0f}, {0.5f, 0.0f, 0.0f, 1.0f}},

		{   {1.0f, 1.0f, 1.0f}, {1.0f, 0.5f, 0.0f, 1.0f}},
		{  {1.0f, -1.0f, 1.0f}, {0.5f, 0.0f, 0.0f, 1.0f}},
		{ {1.0f, -1.0f, -1.0f}, {0.5f, 0.0f, 0.0f, 1.0f}},

		// left

		{  {-1.0f, 1.0f, 1.0f}, {1.0f, 0.5f, 0.0f, 1.0f}},
		{ {-1.0f, 1.0f, -1.0f}, {1.0f, 0.5f, 0.0f, 1.0f}},
		{{-1.0f, -1.0f, -1.0f}, {0.5f, 0.0f, 0.0f, 1.0f}},


		{ {-1.0f, -1.0f, 1.0f}, {0.5f, 0.0f, 0.0f, 1.0f}},
		{  {-1.0f, 1.0f, 1.0f}, {1.0f, 0.5f, 0.0f, 1.0f}},
		{{-1.0f, -1.0f, -1.0f}, {0.5f, 0.0f, 0.0f, 1.0f}},

		// back

		{   {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 0.5f, 1.0f}},
			{ {-1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 0.5f, 1.0f}},
		{{-1.0f, -1.0f, 1.0f}, {0.5f, 0.5f, 0.0f, 1.0f}},

		{  {1.0f, -1.0f, 1.0f}, {0.5f, 0.5f, 0.0f, 1.0f}},
		{  {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 0.5f, 1.0f}},
		{{-1.0f, -1.0f, 1.0f}, {0.5f, 0.5f, 0.0f, 1.0f}},

		// top
		{   {-1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 0.5f, 1.0f}},
		{  {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 0.5f, 1.0f}},
		{ {-1.0f,  1.0f, -1.0f}, {0.5f, 0.5f, 0.0f, 1.0f}},


		{ {-1.0f, 1.0f, -1.0f}, {0.5f, 0.5f, 0.0f, 1.0f}},
		{   {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 0.5f, 1.0f}},
		{ {1.0f,  1.0f, -1.0f}, {0.5f, 0.5f, 0.0f, 1.0f}},

		// bottom
		{  {1.0f, -1.0f, 1.0f}, {1.0f, 1.0f, 0.5f, 1.0f}},
		{  {-1.0f, -1.0f, 1.0f}, {1.0f, 1.0f, 0.5f, 1.0f}},
		{ {-1.0f, -1.0f, -1.0f}, {0.5f, 0.5f, 0.0f, 1.0f}},

		{  {1.0f, -1.0f, 1.0f}, {1.0f, 1.0f, 0.5f, 1.0f}},
		{ {-1.0f, -1.0f, -1.0f}, {0.5f, 0.5f, 0.0f, 1.0f}},
		{  {1.0f, -1.0f, -1.0f}, {0.5f, 0.5f, 0.0f, 1.0f}},
	};*/

	size_t const VERTEX_BUFFER_SIZE = sizeof(box_data);
	size_t const NUM_VERTICES = VERTEX_BUFFER_SIZE / sizeof(vertex_t);

	D3D12_VIEWPORT m_viewport;
	D3D12_RECT m_scissorRect;
	ComPtr<IDXGISwapChain3> m_swapChain;
	ComPtr<ID3D12Device> m_device;
	ComPtr<ID3D12Resource> m_renderTargets[FrameCount];
	ComPtr<ID3D12CommandAllocator> m_commandAllocator[FrameCount];
	ComPtr<ID3D12CommandQueue> m_commandQueue;
	ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
	ComPtr<ID3D12GraphicsCommandList> m_commandList[FrameCount];
	UINT m_rtvDescriptorSize;

	ComPtr<ID3D12RootSignature> m_rootSignature;
	ComPtr<ID3D12PipelineState> m_pipelineState;

	ComPtr<ID3D12Resource> m_vertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;

	ComPtr<ID3D12DescriptorHeap> constbufDescHeap;
	ComPtr<ID3D12Resource> constbufResource;

	ComPtr<ID3D12DescriptorHeap> depthbufDescHeap;
	ComPtr<ID3D12Resource> depthbufResource;
	D3D12_CPU_DESCRIPTOR_HANDLE depthbufDescHandle;

	UINT_PTR timer;

	UINT m_frameIndex = 0;
	HANDLE m_fenceEvent;
	ComPtr<ID3D12Fence> m_fence;
	UINT64 m_fenceValue;

	const float blue[] = { 0.0f, 0.0f, 255.0f, 1.0f };
	const float yellow[] = { 255.0f, 255.0f, 0.0f, 1.0f };

	D3D12_CPU_DESCRIPTOR_HANDLE m_rtvHandles[FrameCount];

	std::chrono::high_resolution_clock::time_point start_point = std::chrono::high_resolution_clock::now();

	struct vs_const_buffer_t {
		XMFLOAT4X4 matWorldViewProj;
		XMFLOAT4X4 matWorldView;
		XMFLOAT4X4 matView;
		XMFLOAT4 colMaterial;
		XMFLOAT4 colLight;
		XMFLOAT4 dirLight;
		XMFLOAT4 padding[(256 - 3*(sizeof(XMFLOAT4X4)) / sizeof(XMFLOAT4)) - 3];
	};

	vs_const_buffer_t constbuf;
	void* constbufbegin;

	FLOAT angle = 0.0f;

	void WaitForPreviousFrame();

	double get_time() {
		std::chrono::high_resolution_clock::time_point now_point = std::chrono::high_resolution_clock::now();
		double microseconds_passed = std::chrono::duration_cast<std::chrono::microseconds>(now_point - start_point).count();
		return microseconds_passed / 1000000;
	}

	void LoadPipeline() {
#if defined(_DEBUG)
		{

			ComPtr<ID3D12Debug> debugController;
			if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
			{
				debugController->EnableDebugLayer();
			}
		}
#endif
		ComPtr<IDXGIFactory7> factory;
		ThrowIfFailed(CreateDXGIFactory2(0, IID_PPV_ARGS(&factory)), "DXGIFactory2");

		ThrowIfFailed(D3D12CreateDevice(
			nullptr,
			D3D_FEATURE_LEVEL_12_0,
			IID_PPV_ARGS(&m_device)
		), "Device");

		D3D12_COMMAND_QUEUE_DESC queueDesc = {
			.Type = D3D12_COMMAND_LIST_TYPE_DIRECT,
			.Priority = D3D12_COMMAND_QUEUE_PRIORITY_HIGH,
			.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE,
			.NodeMask = 0,
		};

		ThrowIfFailed(m_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_commandQueue)), "CommandQueue");

		DXGI_SAMPLE_DESC sampleDesc = {
			.Count = 1,
			.Quality = 0,
		};

		DXGI_SWAP_CHAIN_DESC1 swapChainDesc1 = {
			.Width = 0,
			.Height = 0,
			.Format = DXGI_FORMAT_R8G8B8A8_UNORM,
			.Stereo = FALSE,
			.SampleDesc = sampleDesc,
			.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
			.BufferCount = FrameCount, // from 2 to 16
			.Scaling = DXGI_SCALING_NONE,
			.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD,
			.AlphaMode = DXGI_ALPHA_MODE_IGNORE,
			.Flags = 0,
		};

		DXGI_RATIONAL ratio = {
			.Numerator = 60,
			.Denominator = 1,
		};

		DXGI_SWAP_CHAIN_FULLSCREEN_DESC swapChainFullscreen = {
			.RefreshRate = ratio,
			.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_PROGRESSIVE,
			.Scaling = DXGI_MODE_SCALING_CENTERED,
			.Windowed = TRUE,
		};

		ComPtr<IDXGISwapChain1> swapChain;
		ThrowIfFailed(factory->CreateSwapChainForHwnd(
			m_commandQueue.Get(),        // Swap chain needs the queue so that it can force a flush on it.
			hwnd,
			&swapChainDesc1,
			&swapChainFullscreen,
			nullptr,
			&swapChain
		), "SwapChainHwnd");

		ThrowIfFailed(swapChain.As(&m_swapChain), "swapChain3");

		D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {
			.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV,
			.NumDescriptors = FrameCount,
			.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE,
			.NodeMask = 0,
		};

		ThrowIfFailed(m_device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_rtvHeap)), "DescriptorHeap");
		m_rtvDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);


		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart());

		// Create a RTV for each frame.
		for (UINT n = 0; n < FrameCount; n++)
		{
			ThrowIfFailed(m_swapChain->GetBuffer(n, IID_PPV_ARGS(&m_renderTargets[n])), "GetBuffer");
			m_device->CreateRenderTargetView(m_renderTargets[n].Get(), nullptr, rtvHandle);

			m_rtvHandles[n] = rtvHandle;

			rtvHandle.ptr += m_rtvDescriptorSize;
			ThrowIfFailed(m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_commandAllocator[n])), "CreateCommandAllocator");
		}
		
	}
	
	void LoadAssets() {
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
		ThrowIfFailed(D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error), "serialize root signature");
		ThrowIfFailed(m_device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_rootSignature)), "create root signature");

		D3D12_INPUT_ELEMENT_DESC inputElementDescs[] = {
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

		D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {
			.pRootSignature = m_rootSignature.Get(),
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
			.InputLayout = { inputElementDescs, _countof(inputElementDescs) },
			.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED,
			.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
			.NumRenderTargets = 1,
			.RTVFormats = { DXGI_FORMAT_R8G8B8A8_UNORM },
			.DSVFormat = DXGI_FORMAT_D32_FLOAT,
			.SampleDesc = {.Count = 1, .Quality = 0 },
		};
		ThrowIfFailed(m_device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_pipelineState)), "create graphics pipeline state");

		for (int n = 0; n < FrameCount; n++) {
			ThrowIfFailed(m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT,
				m_commandAllocator[n].Get(), nullptr,
				IID_PPV_ARGS(&m_commandList[n])), "Create CommandList");
			ThrowIfFailed(m_commandList[n]->Close(), "close command list");
		}
		{
			D3D12_DESCRIPTOR_HEAP_DESC heapDescConstBuf = {
				.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
				.NumDescriptors = 1,
				.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE,
				.NodeMask = 0,
			};

			ThrowIfFailed(m_device->CreateDescriptorHeap(&heapDescConstBuf, IID_PPV_ARGS(&constbufDescHeap)), "DescriptorHeapConstBuf");

			D3D12_HEAP_PROPERTIES heapPropConstBuf = {
				.Type = D3D12_HEAP_TYPE_UPLOAD,
				.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN,
				.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN,
				.CreationNodeMask = 1,
				.VisibleNodeMask = 1,
			};

			D3D12_RESOURCE_DESC descConstBuf = {
				.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER,
				.Alignment = 0,
				.Width = 256, // todo check
				.Height = 1,
				.DepthOrArraySize = 1,
				.MipLevels = 1,
				.Format = DXGI_FORMAT_UNKNOWN,
				.SampleDesc = {.Count = 1, .Quality = 0 },
				.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR,
				.Flags = D3D12_RESOURCE_FLAG_NONE,
			};

			ThrowIfFailed(m_device->CreateCommittedResource(
				&heapPropConstBuf,
				D3D12_HEAP_FLAG_NONE,
				&descConstBuf,
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr,
				IID_PPV_ARGS(&constbufResource)), "create comitted resource for const buf");

			D3D12_CONSTANT_BUFFER_VIEW_DESC viewDescConstBuf = {
				.BufferLocation = constbufResource->GetGPUVirtualAddress(),
				.SizeInBytes = 256, //todo check
			};

			m_device->CreateConstantBufferView(&viewDescConstBuf, constbufDescHeap->GetCPUDescriptorHandleForHeapStart());
		}

		{
			D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {
				.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV,
				.NumDescriptors = 1,
				.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE,
				.NodeMask = 0,
			};

			ThrowIfFailed(m_device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&depthbufDescHeap)), "DescriptorHeapDepthBuf");

			D3D12_HEAP_PROPERTIES heapProp = {
				.Type = D3D12_HEAP_TYPE_DEFAULT,
				.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN,
				.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN,
				.CreationNodeMask = 1,
				.VisibleNodeMask = 1,
			};

			GetClientRect(hwnd, &m_scissorRect);
			width = m_scissorRect.right - m_scissorRect.left;
			height = m_scissorRect.bottom - m_scissorRect.top;

			D3D12_RESOURCE_DESC desc = {
				.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D,
				.Alignment = 0,
				.Width = width, // szerokoœæ celu renderowania
				.Height = height, // wysokoœæ celu renderowania
				.DepthOrArraySize = 1,
				.MipLevels = 0,
				.Format = DXGI_FORMAT_D32_FLOAT,
				.SampleDesc = {.Count = 1, .Quality = 0 },
				.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN,
				.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL,
			};
			
			D3D12_CLEAR_VALUE clear_value = {
				.Format = DXGI_FORMAT_D32_FLOAT,
				.DepthStencil = {.Depth = 1.0f, .Stencil = 0 }
			};

			ThrowIfFailed(m_device->CreateCommittedResource(
				&heapProp,
				D3D12_HEAP_FLAG_NONE,
				&desc,
				D3D12_RESOURCE_STATE_GENERIC_READ,
				&clear_value,
				IID_PPV_ARGS(&depthbufResource)), "create comitted resource for depth buf");
			
			D3D12_DEPTH_STENCIL_VIEW_DESC depth_stencil = {
				.Format = DXGI_FORMAT_D32_FLOAT,
				.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D,
				.Flags = D3D12_DSV_FLAG_NONE,
				.Texture2D = {},
			};
			depthbufDescHandle = depthbufDescHeap->GetCPUDescriptorHandleForHeapStart();
			m_device->CreateDepthStencilView( // TODO check
				nullptr,
				&depth_stencil,
				depthbufDescHandle);
		}
		// TODO set more of constbuf FLOAT4 jako vectory
		DirectX::XMStoreFloat4x4(&constbuf.matWorldViewProj, DirectX::XMMatrixIdentity());
		DirectX::XMStoreFloat4x4(&constbuf.matWorldView, DirectX::XMMatrixIdentity());
		DirectX::XMStoreFloat4x4(&constbuf.matView, DirectX::XMMatrixIdentity());

		D3D12_RANGE  readRangeconstbuf = {
			.Begin = 0,
			.End = 0,
		};

		ThrowIfFailed(constbufResource->Map(0, &readRangeconstbuf, &constbufbegin), "vertex buffer map");
		memcpy(constbufbegin, &constbuf, sizeof(constbuf));
		{
			D3D12_HEAP_PROPERTIES heapProps = {
				.Type = D3D12_HEAP_TYPE_UPLOAD,
				.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN,
				.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN,
				.CreationNodeMask = 1,
				.VisibleNodeMask = 1,
			};
			D3D12_RESOURCE_DESC desc = {
				.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER,
				.Alignment = 0,
				.Width = VERTEX_BUFFER_SIZE,
				.Height = 1,
				.DepthOrArraySize = 1,
				.MipLevels = 1,
				.Format = DXGI_FORMAT_UNKNOWN,
				.SampleDesc = {.Count = 1, .Quality = 0 },
				.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR,
				.Flags = D3D12_RESOURCE_FLAG_NONE,

			};
			ThrowIfFailed(m_device->CreateCommittedResource(
				&heapProps,
				D3D12_HEAP_FLAG_NONE,
				&desc,
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr,
				IID_PPV_ARGS(&m_vertexBuffer)), "create comitted resource");
		}
		void* pVertexDataBegin;
		D3D12_RANGE  readRange = {
			.Begin = 0,
			.End = 0,
		};        // We do not intend to read from this resource on the CPU.
		ThrowIfFailed(m_vertexBuffer->Map(0, &readRange, &pVertexDataBegin), "vertex buffer map");
		memcpy(pVertexDataBegin, box_data, VERTEX_BUFFER_SIZE);
		m_vertexBuffer->Unmap(0, nullptr);

		m_vertexBufferView.BufferLocation = m_vertexBuffer->GetGPUVirtualAddress();
		m_vertexBufferView.StrideInBytes = sizeof(vertex_t);
		m_vertexBufferView.SizeInBytes = VERTEX_BUFFER_SIZE;

		ThrowIfFailed(m_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence)), "create fence");
		m_fenceValue = 1;

		// Create an event handle to use for frame synchronization.
		m_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
		if (m_fenceEvent == nullptr)
		{
			ThrowIfFailed(HRESULT_FROM_WIN32(GetLastError()), "create fence event");
		}

		// Wait for the command list to execute; we are reusing the same command 
		// list in our main loop but for now, we just want to wait for setup to 
		// complete before continuing.
		WaitForPreviousFrame();
	}

	void PopulateCommandList() {
		ThrowIfFailed(m_commandAllocator[m_frameIndex]->Reset(), "reset command allocator");
		ThrowIfFailed(m_commandList[m_frameIndex]->Reset(m_commandAllocator[m_frameIndex].Get(), m_pipelineState.Get()), "reset command list");

		m_commandList[m_frameIndex]->SetGraphicsRootSignature(m_rootSignature.Get());

		ID3D12DescriptorHeap* ppHeaps[] = { constbufDescHeap.Get() };
		m_commandList[m_frameIndex]->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
		m_commandList[m_frameIndex]->SetGraphicsRootDescriptorTable(0, constbufDescHeap->GetGPUDescriptorHandleForHeapStart());

		GetClientRect(hwnd, &m_scissorRect);
		width = m_scissorRect.right - m_scissorRect.left;
		height = m_scissorRect.bottom - m_scissorRect.top;
		m_viewport = {
			.TopLeftX = 0.0f,
			.TopLeftY = 0.0f,
			.Width = static_cast<FLOAT>(m_scissorRect.right - m_scissorRect.left),	// aktualna szerokoœæ obszaru roboczego okna (celu rend.)
			.Height = static_cast<FLOAT>(m_scissorRect.bottom - m_scissorRect.top),	// aktualna wysokoœæ obszaru roboczego okna (celu rend.)
			.MinDepth = 0.0f,
			.MaxDepth = 1.0f,
		};
		m_commandList[m_frameIndex]->RSSetViewports(1, &m_viewport);
		m_commandList[m_frameIndex]->RSSetScissorRects(1, &m_scissorRect);

		D3D12_RESOURCE_TRANSITION_BARRIER barrier = {
			.pResource = m_renderTargets[m_frameIndex].Get(),
			.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES,
			.StateBefore = D3D12_RESOURCE_STATE_PRESENT,
			.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET,
		};

		D3D12_RESOURCE_BARRIER pbarrier = {
			.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION,
			.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE,
			.Transition = barrier,
		};
		m_commandList[m_frameIndex]->ResourceBarrier(1, &pbarrier);

		m_commandList[m_frameIndex]->OMSetRenderTargets(1, &m_rtvHandles[m_frameIndex], TRUE, &depthbufDescHandle);

		double time = get_time();

		if (static_cast<int>(time) % 2 == 1) {
			m_commandList[m_frameIndex]->ClearRenderTargetView(m_rtvHandles[m_frameIndex], blue, 0, nullptr);
		}
		else {
			m_commandList[m_frameIndex]->ClearRenderTargetView(m_rtvHandles[m_frameIndex], yellow, 0, nullptr);
		}
		m_commandList[m_frameIndex]->ClearDepthStencilView(depthbufDescHandle,
			D3D12_CLEAR_FLAG_STENCIL| D3D12_CLEAR_FLAG_DEPTH,
			1.0f,
			0,
			0,
			nullptr);
		m_commandList[m_frameIndex]->IASetVertexBuffers(0, 1, &m_vertexBufferView);
		m_commandList[m_frameIndex]->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		m_commandList[m_frameIndex]->DrawInstanced(NUM_VERTICES, 1, 0, 0);

		barrier = {
			.pResource = m_renderTargets[m_frameIndex].Get(),
			.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES,
			.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET,
			.StateAfter = D3D12_RESOURCE_STATE_PRESENT,
		};

		pbarrier = {
			.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION,
			.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE,
			.Transition = barrier,
		};
		m_commandList[m_frameIndex]->ResourceBarrier(1, &pbarrier);

		ThrowIfFailed(m_commandList[m_frameIndex]->Close(), "close command list");
		m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();
	}

	void WaitForPreviousFrame()
	{
		const UINT64 fence = m_fenceValue;
		ThrowIfFailed(m_commandQueue->Signal(m_fence.Get(), fence), "signal command queue");
		m_fenceValue++;

		if (m_fence->GetCompletedValue() < fence)
		{
			ThrowIfFailed(m_fence->SetEventOnCompletion(fence, m_fenceEvent), "setEvent");
			WaitForSingleObject(m_fenceEvent, INFINITE);
		}

		m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();
	}
}

void OnInit(HWND _hwnd) {
	hwnd = _hwnd;
	LoadPipeline();
	LoadAssets();
}

void OnUpdate(){
	angle += 1.0f / 64.0f;
	XMMATRIX wvp_matrix;
	wvp_matrix = XMMatrixMultiply(
		XMMatrixRotationY(2.5f * angle),	// zmienna angle zmienia siê
		// o 1 / 64 co ok. 15 ms 
		XMMatrixRotationX(static_cast<FLOAT>(sin(angle)) / 2.0f));
	wvp_matrix = XMMatrixMultiply(
		wvp_matrix, XMMatrixTranslation(0.0f, 0.0f, 4.0f));
	wvp_matrix = XMMatrixMultiply(
		wvp_matrix, XMMatrixPerspectiveFovLH(
			45.0f, m_viewport.Width / m_viewport.Height, 1.0f, 100.0f));
	wvp_matrix = XMMatrixTranspose(wvp_matrix);
	XMStoreFloat4x4(
		&constbuf.matWorldViewProj, 	// zmienna typu vs_const_buffer_t z pkt. 2d
		wvp_matrix);
	memcpy(
		constbufbegin, 		// wskaŸnik do zmapowanej pamiêci (buf. sta³ego)
		&constbuf, 		// zmienna typu vs_const_buffer_t z pkt. 2d
		sizeof(constbuf)	// zmienna typu vs_const_buffer_t z pkt. 2d
	);
}

void OnRender()
{
	PopulateCommandList();

	ID3D12CommandList* ppCommandLists[] = { m_commandList[m_frameIndex].Get() };
	m_commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

	ThrowIfFailed(m_swapChain->Present(1, 0), "Present");

	WaitForPreviousFrame();
}

void OnDestroy() {
	WaitForPreviousFrame();
	CloseHandle(m_fenceEvent);
}

void InitTimer(HWND hwnd) {
	timer = SetTimer(hwnd, 0, 0, 0);
}

void ReleaseTimer(HWND hwnd) {
	KillTimer(hwnd, timer);
}
