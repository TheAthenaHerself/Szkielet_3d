#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include "Throws.h"
#include "Application3D.h"
#include <memory>
using std::shared_ptr;
using Microsoft::WRL::ComPtr;

#include "RenderTargets.h"

void RenderTargets(shared_ptr<Application_3d> &app) {
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle(app->m_rtvHeap->GetCPUDescriptorHandleForHeapStart());

	// Create a RTV for each frame.
	for (UINT n = 0; n < FrameCount; n++)
	{
		ThrowIfFailed(app->m_swapChain->GetBuffer(n, IID_PPV_ARGS(&app->m_renderTargets[n])), "GetBuffer");
		app->m_device->CreateRenderTargetView(app->m_renderTargets[n].Get(), nullptr, rtvHandle);

		app->m_rtvHandles[n] = rtvHandle;

		rtvHandle.ptr += app->m_rtvDescriptorSize;
		ThrowIfFailed(app->m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&app->m_commandAllocator[n])), "CreateCommandAllocator");
	}
}