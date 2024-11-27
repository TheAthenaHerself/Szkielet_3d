#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <stdexcept>
#include <chrono>

#include "Throws.h"
#include "Direct3d.h"

using Microsoft::WRL::ComPtr;

namespace {
	static const UINT FrameCount = 2;

	ComPtr<IDXGISwapChain3> m_swapChain;
	ComPtr<ID3D12Device> m_device;
	ComPtr<ID3D12Resource> m_renderTargets[FrameCount];
	ComPtr<ID3D12CommandAllocator> m_commandAllocator;
	ComPtr<ID3D12CommandQueue> m_commandQueue;
	ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
	ComPtr<ID3D12GraphicsCommandList> m_commandList;
	UINT m_rtvDescriptorSize;

	UINT_PTR timer;

	UINT m_frameIndex = 0;
	HANDLE m_fenceEvent;
	ComPtr<ID3D12Fence> m_fence;
	UINT64 m_fenceValue;

	const float blue[] = { 0.0f, 0.0f, 255.0f, 1.0f };
	const float yellow[] = { 255.0f, 255.0f, 0.0f, 1.0f };

	D3D12_CPU_DESCRIPTOR_HANDLE m_rtvHandles[FrameCount];

	std::chrono::high_resolution_clock::time_point start_point = std::chrono::high_resolution_clock::now();

	void WaitForPreviousFrame();

	double get_time() {
		std::chrono::high_resolution_clock::time_point now_point = std::chrono::high_resolution_clock::now();
		double microseconds_passed = std::chrono::duration_cast<std::chrono::microseconds>(now_point - start_point).count();
		return microseconds_passed / 1000000;
	}

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
		}
		ThrowIfFailed(m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_commandAllocator)), "CreateCommandAllocator");
	}
	
	void LoadAssets() {
		ThrowIfFailed(m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT,
			m_commandAllocator.Get(), nullptr, 
			IID_PPV_ARGS(&m_commandList)), "Create CommandList");
		ThrowIfFailed(m_commandList->Close(), "close command list");
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
		ThrowIfFailed(m_commandAllocator->Reset(), "reset command allocator");
		ThrowIfFailed(m_commandList->Reset(m_commandAllocator.Get(), nullptr), "reset command list");

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
		m_commandList->ResourceBarrier(1, &pbarrier);

		m_commandList->OMSetRenderTargets(1, &m_rtvHandles[m_frameIndex], TRUE, nullptr);

		double time = get_time();

		if (static_cast<int>(time) % 2 == 1) {
			m_commandList->ClearRenderTargetView(m_rtvHandles[m_frameIndex], blue, 0, nullptr);
		}
		else {
			m_commandList->ClearRenderTargetView(m_rtvHandles[m_frameIndex], yellow, 0, nullptr);
		}

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
		m_commandList->ResourceBarrier(1, &pbarrier);

		ThrowIfFailed(m_commandList->Close(), "close command list");
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

void OnInit(HWND hwnd) {
	LoadPipeline(hwnd);
	LoadAssets();
}

void OnUpdate(){
}

void OnRender()
{
	PopulateCommandList();

	ID3D12CommandList* ppCommandLists[] = { m_commandList.Get() };
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