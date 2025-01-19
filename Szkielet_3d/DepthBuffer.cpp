#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <wincodec.h>
#include <memory>
using std::shared_ptr;
#include "Throws.h"
#include "Application3D.h"

using Microsoft::WRL::ComPtr;

#include "DepthBuffer.h"

void DepthBuffer(shared_ptr<Application_3d> &app) {
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {
				.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV,
				.NumDescriptors = 1,
				.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE,
				.NodeMask = 0,
	};

	ThrowIfFailed(app->m_device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&app->depthbufDescHeap)), "DescriptorHeapDepthBuf");

	D3D12_HEAP_PROPERTIES heapProp = {
		.Type = D3D12_HEAP_TYPE_DEFAULT,
		.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN,
		.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN,
		.CreationNodeMask = 1,
		.VisibleNodeMask = 1,
	};

	D3D12_RESOURCE_DESC desc = {
		.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D,
		.Alignment = 0,
		.Width = app->width, // szerokoœæ celu renderowania
		.Height = app->height, // wysokoœæ celu renderowania
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

	ThrowIfFailed(app->m_device->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		&clear_value,
		IID_PPV_ARGS(&app->depthbufResource)), "create comitted resource for depth buf");

	D3D12_DEPTH_STENCIL_VIEW_DESC depth_stencil = {
		.Format = DXGI_FORMAT_D32_FLOAT,
		.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D,
		.Flags = D3D12_DSV_FLAG_NONE,
		.Texture2D = {},
	};
	app->depthbufDescHandle = app->depthbufDescHeap->GetCPUDescriptorHandleForHeapStart();
	app->m_device->CreateDepthStencilView(
		app->depthbufResource.Get(),
		&depth_stencil,
		app->depthbufDescHandle);
}