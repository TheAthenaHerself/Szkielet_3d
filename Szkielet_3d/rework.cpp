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
#include <memory>

#include "rework.h"
#include "Throws.h"

#include "CommandQueue.h"
#include "SwapChain.h"
#include "DescriptorHeap.h"
#include "RenderTargets.h"
#include "RootSignature.h"
#include "GraphicsPipelineState.h"
#include "ConstantBuffer.h"
#include "DepthBuffer.h"
#include "VertexBuffer.h"

//#include "TriangleData.h"
#include "BoxData.h"

using std::shared_ptr;
using Microsoft::WRL::ComPtr;
using DirectX::XMFLOAT4X4;
using DirectX::XMFLOAT4;
using DirectX::XMMATRIX;
using DirectX::XMMatrixMultiply;
using DirectX::XMMatrixRotationY;
using DirectX::XMMatrixRotationX;
using DirectX::XMMatrixTranslation;
using DirectX::XMMatrixPerspectiveFovLH;
using std::array;
using std::random_device;
using std::mt19937;
using std::uniform_real_distribution;
using std::numbers::pi_v;

namespace {
	const float blue[] = { 0.0f, 0.0f, 255.0f, 1.0f };
	const float black[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	const float yellow[] = { 255.0f, 255.0f, 0.0f, 1.0f };

	shared_ptr<Application_3d> app(new Application_3d());

	std::chrono::high_resolution_clock::time_point start_point = std::chrono::high_resolution_clock::now();

	double get_time() {
		std::chrono::high_resolution_clock::time_point now_point = std::chrono::high_resolution_clock::now();
		double microseconds_passed = std::chrono::duration_cast<std::chrono::microseconds>(now_point - start_point).count();
		return microseconds_passed / 1000000;
	}
}

void SetApp(shared_ptr<Application_3d>& _app) {
	_app = app;
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
	ThrowIfFailed(CreateDXGIFactory2(0, IID_PPV_ARGS(&app->factory)), "DXGIFactory2");
	ThrowIfFailed(D3D12CreateDevice(
		nullptr,
		D3D_FEATURE_LEVEL_12_0,
		IID_PPV_ARGS(&app->m_device)
	), "Device");
	CommandQueue(app);
	SwapChain(app);
	DescriptorHeap(app);
	GetClientRect(app->hwnd, &app->m_scissorRect);

	app->width = app->m_scissorRect.right - app->m_scissorRect.left;
	app->height = app->m_scissorRect.bottom - app->m_scissorRect.top;
	app->m_viewport = {
		.TopLeftX = 0.0f,
		.TopLeftY = 0.0f,
		.Width = static_cast<FLOAT>(app->m_scissorRect.right - app->m_scissorRect.left),	// aktualna szerokoœæ obszaru roboczego okna (celu rend.)
		.Height = static_cast<FLOAT>(app->m_scissorRect.bottom - app->m_scissorRect.top),	// aktualna wysokoœæ obszaru roboczego okna (celu rend.)
		.MinDepth = 0.0f,
		.MaxDepth = 1.0f,
	};

	RenderTargets(app);
}
void LoadAssets() {
	RootSignature(app);
	GraphicsPipelineState(app);

	for (int n = 0; n < FrameCount; n++) {
		ThrowIfFailed(app->m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT,
			app->m_commandAllocator[n].Get(), nullptr,
			IID_PPV_ARGS(&app->m_commandList[n])), "Create CommandList");
		ThrowIfFailed(app->m_commandList[n]->Close(), "close command list");
	}

	ConstantBuffer(app);
	DepthBuffer(app);
	VertexBuffer(app, VERTEX_BUFFER_SIZE);
	void* pVertexDataBegin;
	D3D12_RANGE  readRange = {
		.Begin = 0,
		.End = 0,
	};        // We do not intend to read from this resource on the CPU.

	ThrowIfFailed(app->m_vertexBuffer->Map(0, &readRange, &pVertexDataBegin), "vertex buffer map");
	memcpy(pVertexDataBegin, data, VERTEX_BUFFER_SIZE);
	app->m_vertexBuffer->Unmap(0, nullptr);

	app->m_vertexBufferView.BufferLocation = app->m_vertexBuffer->GetGPUVirtualAddress();
	app->m_vertexBufferView.StrideInBytes = sizeof(vertex_t);
	app->m_vertexBufferView.SizeInBytes = VERTEX_BUFFER_SIZE;

	ThrowIfFailed(app->m_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&app->m_fence)), "create fence");
	app->m_fenceValue = 1;

	app->m_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	if (app->m_fenceEvent == nullptr)
	{
		ThrowIfFailed(HRESULT_FROM_WIN32(GetLastError()), "create fence event");
	}

	WaitForPreviousFrame();
}

void PopulateCommandList() {
	ThrowIfFailed(app->m_commandAllocator[app->m_frameIndex]->Reset(), "reset command allocator");
	ThrowIfFailed(app->m_commandList[app->m_frameIndex]->Reset(app->m_commandAllocator[app->m_frameIndex].Get()
		, app->m_pipelineState.Get()), "reset command list");

	app->m_commandList[app->m_frameIndex]->SetGraphicsRootSignature(app->m_rootSignature.Get());

	ID3D12DescriptorHeap* ppHeaps[] = { app->constbufDescHeap.Get() };
	app->m_commandList[app->m_frameIndex]->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
	app->m_commandList[app->m_frameIndex]->SetGraphicsRootDescriptorTable(0, app->constbufDescHeap->GetGPUDescriptorHandleForHeapStart());

	app->m_commandList[app->m_frameIndex]->RSSetViewports(1, &app->m_viewport);
	app->m_commandList[app->m_frameIndex]->RSSetScissorRects(1, &app->m_scissorRect);

	D3D12_RESOURCE_TRANSITION_BARRIER barrier = {
		.pResource = app->m_renderTargets[app->m_frameIndex].Get(),
		.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES,
		.StateBefore = D3D12_RESOURCE_STATE_PRESENT,
		.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET,
	};

	D3D12_RESOURCE_BARRIER pbarrier = {
		.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION,
		.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE,
		.Transition = barrier,
	};
	app->m_commandList[app->m_frameIndex]->ResourceBarrier(1, &pbarrier);

	app->m_commandList[app->m_frameIndex]->OMSetRenderTargets(1, &app->m_rtvHandles[app->m_frameIndex], FALSE, &app->depthbufDescHandle);

	double time = get_time();

	if (static_cast<int>(time) % 2 == 1) {
		app->m_commandList[app->m_frameIndex]->ClearRenderTargetView(app->m_rtvHandles[app->m_frameIndex], blue, 0, nullptr);
	}
	else {
		app->m_commandList[app->m_frameIndex]->ClearRenderTargetView(app->m_rtvHandles[app->m_frameIndex], yellow, 0, nullptr);
	}

	app->m_commandList[app->m_frameIndex]->ClearDepthStencilView(app->depthbufDescHandle,
		D3D12_CLEAR_FLAG_DEPTH,
		1.0f,
		0,
		0,
		nullptr);

	app->m_commandList[app->m_frameIndex]->IASetVertexBuffers(0, 1, &app->m_vertexBufferView);
	app->m_commandList[app->m_frameIndex]->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	app->m_commandList[app->m_frameIndex]->DrawInstanced(NUM_VERTICES, 1, 0, 0);
	
	app->m_commandList[app->m_frameIndex]->IASetVertexBuffers(
		1, 1, &app->instance_buffer_view);

	barrier = {
		.pResource = app->m_renderTargets[app->m_frameIndex].Get(),
		.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES,
		.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET,
		.StateAfter = D3D12_RESOURCE_STATE_PRESENT,
	};

	pbarrier = {
		.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION,
		.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE,
		.Transition = barrier,
	};
	app->m_commandList[app->m_frameIndex]->ResourceBarrier(1, &pbarrier);

	ThrowIfFailed(app->m_commandList[app->m_frameIndex]->Close(), "close command list");
	app->m_frameIndex = app->m_swapChain->GetCurrentBackBufferIndex();
}

void WaitForPreviousFrame()
{
	const UINT64 fence = app->m_fenceValue;
	ThrowIfFailed(app->m_commandQueue->Signal(app->m_fence.Get(), fence), "signal command queue");
	app->m_fenceValue++;

	if (app->m_fence->GetCompletedValue() < fence)
	{
		ThrowIfFailed(app->m_fence->SetEventOnCompletion(fence, app->m_fenceEvent), "setEvent");
		WaitForSingleObject(app->m_fenceEvent, INFINITE);
	}

	app->m_frameIndex = app->m_swapChain->GetCurrentBackBufferIndex();
}
