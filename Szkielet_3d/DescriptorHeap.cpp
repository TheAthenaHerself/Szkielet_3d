#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <memory>
using std::shared_ptr;
#include "Throws.h"
#include "Application3D.h"

using Microsoft::WRL::ComPtr;

#include "DescriptorHeap.h"

void DescriptorHeap(shared_ptr<Application_3d> &app) {
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {
			.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV,
			.NumDescriptors = FrameCount,
			.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE,
			.NodeMask = 0,
	};

	ThrowIfFailed(app->m_device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&app->m_rtvHeap)), "DescriptorHeap");
	app->m_rtvDescriptorSize = app->m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
}