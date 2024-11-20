#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>

#include "Direct3d.h"
#include "Throws.h"

using Microsoft::WRL::ComPtr;

namespace {
	D3D12_VIEWPORT m_viewport;
	ComPtr<IDXGISwapChain3> m_swapChain;
	ComPtr<ID3D12Device> m_device;
	ComPtr<ID3D12Resource> m_renderTargets[FrameCount];
	ComPtr<ID3D12Resource> m_depthStencil;
	ComPtr<ID3D12CommandAllocator> m_commandAllocator;
	ComPtr<ID3D12GraphicsCommandList> m_commandList;
	ComPtr<ID3D12CommandQueue> m_commandQueue;
	ComPtr<ID3D12RootSignature >m_rootSignature;
	ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
	ComPtr<ID3D12DescriptorHeap> m_cbvSrvHeap;
	ComPtr<ID3D12DescriptorHeap> m_dsvHeap;
	ComPtr<ID3D12DescriptorHeap> m_samplerHeap;
	ComPtr<ID3D12PipelineState> m_pipelineState1;
	ComPtr<ID3D12PipelineState> m_pipelineState2;
	D3D12_RECT m_scissorRect;

	HRESULT error_code;

	void LoadPipeline(HWND hwnd) {
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
		ThrowIfFailed(CreateDXGIFactory1(IID_PPV_ARGS(&factory)));

		//ComPtr<IDXGIAdapter1> hardwareAdapter;
		//GetHardwareAdapter(factory.Get(), &hardwareAdapter);

		ThrowIfFailed(D3D12CreateDevice(
			nullptr,
			D3D_FEATURE_LEVEL_11_0,
			IID_PPV_ARGS(&m_device)
		));
		D3D12_COMMAND_QUEUE_DESC queueDesc = {};
		queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

		ThrowIfFailed(m_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_commandQueue)));
		DXGI_SWAP_CHAIN_DESC1 swapChainDesc1 = {
		.Width = 0,
		.Height = 0,
		.Format = DXGI_FORMAT_R8G8B8A8_UNORM,
		.Stereo = FALSE,
		.SampleDesc = 0, //check
		.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
		.BufferCount = 0, //check
		.Scaling = , //check
		.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD,
		.AlphaMode - ,
		.Flags = ,
		};

		DXGI_SWAP_CHAIN_FULLSCREEN_DESC swapChain

		ComPtr<IDXGISwapChain> swapChain;
		ThrowIfFailed(factory->CreateSwapChainForHwnd(
			m_commandQueue.Get(),        // Swap chain needs the queue so that it can force a flush on it.
			hwnd,
			&swapChainDesc1,
			&swapChain
		));

		ThrowIfFailed(swapChain.As(&m_swapChain));
	}
}

void OnInit(HWND hwnd) {
	LoadPipeline(hwnd);
	LoadAssets();
}