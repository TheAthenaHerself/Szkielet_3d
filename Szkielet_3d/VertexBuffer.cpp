#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <wincodec.h>
#include <memory>
using std::shared_ptr;
#include "Throws.h"
#include "Application3D.h"

#include "VertexBuffer.h"

void VertexBuffer(shared_ptr<Application_3d>& app, size_t VERTEX_BUFFER_SIZE) {
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
	ThrowIfFailed(app->m_device->CreateCommittedResource(
		&heapProps,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&app->m_vertexBuffer)), "create comitted resource");
}