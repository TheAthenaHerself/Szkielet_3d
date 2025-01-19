#pragma once

#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <stdexcept>
#include <chrono>
#include <directxmath.h>
#include <numbers>
#include <array>
#include <random>
#include <wincodec.h>

#include "Throws.h"
#include "Direct3d.h"

static const UINT FrameCount = 2;

class Application_3d {
public:
	UINT64 width = 0;
	UINT height = 0;

	HWND hwnd;

	Microsoft::WRL::ComPtr<IDXGIFactory7> factory;

	Microsoft::WRL::ComPtr<ID3D12Resource> instance_buffer = nullptr;
	D3D12_VERTEX_BUFFER_VIEW instance_buffer_view = {};

	D3D12_VIEWPORT m_viewport;
	D3D12_RECT m_scissorRect;
	Microsoft::WRL::ComPtr<IDXGISwapChain3> m_swapChain;
	Microsoft::WRL::ComPtr<ID3D12Device> m_device;
	Microsoft::WRL::ComPtr<ID3D12Resource> m_renderTargets[FrameCount];
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_commandAllocator[FrameCount];
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_commandQueue;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_commandList[FrameCount];
	UINT m_rtvDescriptorSize;

	Microsoft::WRL::ComPtr<ID3D12RootSignature> m_rootSignature;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> m_pipelineState;

	Microsoft::WRL::ComPtr<ID3D12Resource> m_vertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> constbufDescHeap;
	Microsoft::WRL::ComPtr<ID3D12Resource> constbufResource;

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> depthbufDescHeap;
	Microsoft::WRL::ComPtr<ID3D12Resource> depthbufResource;
	D3D12_CPU_DESCRIPTOR_HANDLE depthbufDescHandle;

	Microsoft::WRL::ComPtr<IWICImagingFactory> wic_factory;

	UINT_PTR timer;

	UINT m_frameIndex = 0;
	HANDLE m_fenceEvent;
	Microsoft::WRL::ComPtr<ID3D12Fence> m_fence;
	UINT64 m_fenceValue;

	D3D12_CPU_DESCRIPTOR_HANDLE m_rtvHandles[FrameCount];
	virtual ~Application_3d() {
	}
};