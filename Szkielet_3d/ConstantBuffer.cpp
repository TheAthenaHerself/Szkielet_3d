#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <directxmath.h>
#include <memory>

#include "Throws.h"
#include "Application3D.h"

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
#include "ConstantBuffer.h"

struct vs_const_buffer_t {
	XMFLOAT4X4 matWorldViewProj;
	XMFLOAT4X4 matWorldView;
	XMFLOAT4X4 matView;
	XMFLOAT4 colMaterial;
	XMFLOAT4 colLight;
	XMFLOAT4 dirLight;
	XMFLOAT4 padding[(256 - 3 * (sizeof(XMFLOAT4X4)) / sizeof(XMFLOAT4)) - 3];
};

vs_const_buffer_t constbuf;
void* constbufbegin;

void ConstantBuffer(shared_ptr<Application_3d> &app) {
	D3D12_DESCRIPTOR_HEAP_DESC heapDescConstBuf = {
		.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
		.NumDescriptors = 1,
		.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE,
		.NodeMask = 0,
	};

	ThrowIfFailed(app->m_device->CreateDescriptorHeap(&heapDescConstBuf, 
		IID_PPV_ARGS(&app->constbufDescHeap)), "DescriptorHeapConstBuf");

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

	ThrowIfFailed(app->m_device->CreateCommittedResource(
		&heapPropConstBuf,
		D3D12_HEAP_FLAG_NONE,
		&descConstBuf,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&app->constbufResource)), "create comitted resource for const buf");

	D3D12_CONSTANT_BUFFER_VIEW_DESC viewDescConstBuf = {
		.BufferLocation = app->constbufResource->GetGPUVirtualAddress(),
		.SizeInBytes = 256, //todo check
	};

	app->m_device->CreateConstantBufferView(&viewDescConstBuf, app->constbufDescHeap->GetCPUDescriptorHandleForHeapStart());

	DirectX::XMStoreFloat4x4(&constbuf.matWorldViewProj, DirectX::XMMatrixIdentity());
	DirectX::XMStoreFloat4x4(&constbuf.matWorldView, DirectX::XMMatrixIdentity());
	DirectX::XMStoreFloat4x4(&constbuf.matView, DirectX::XMMatrixIdentity());
	constbuf.colLight = { 1.0f, 0.7f, 0.4f, 1.0f };
	constbuf.colMaterial = { 0.8f, 0.1f, 0.1f, 1.0f };
	constbuf.dirLight = { 1.0f, 1.0f, 1.0f, 0.0f };

	D3D12_RANGE  readRangeconstbuf = {
		.Begin = 0,
		.End = 0,
	};

	ThrowIfFailed(app->constbufResource->Map(0, &readRangeconstbuf, &constbufbegin), "vertex buffer map");
	memcpy(constbufbegin, &constbuf, sizeof(constbuf));
}

void UpdateBuffer(shared_ptr<Application_3d>& app, FLOAT angle) {
	XMMATRIX world, view, proj;
	world = XMMatrixMultiply(
		XMMatrixRotationY(2.5f * angle),	// zmienna angle zmienia siê
		// o 1 / 64 co ok. 15 ms 
		XMMatrixRotationX(static_cast<FLOAT>(sin(30.0f)) / 2.0f));
	view = XMMatrixTranslation(0.0f, 0.0f, 3.0f);
	proj = XMMatrixPerspectiveFovLH(
		45.0f, app->m_viewport.Width / app->m_viewport.Height, 1.0f, 100.0f);

	XMMATRIX world_view = XMMatrixMultiply(world, view);
	XMMATRIX world_view_proj = XMMatrixMultiply(world_view, proj);

	view = XMMatrixTranspose(view);
	world_view = XMMatrixTranspose(world_view);
	world_view_proj = XMMatrixTranspose(world_view_proj);
	XMStoreFloat4x4(
		&constbuf.matWorldViewProj,
		world_view_proj);
	XMStoreFloat4x4(
		&constbuf.matWorldView, //DUE TO ORTHOGONAL MATRIX
		world_view);
	XMStoreFloat4x4(
		&constbuf.matView,
		view);
	memcpy(
		constbufbegin, 		// wskaŸnik do zmapowanej pamiêci (buf. sta³ego)
		&constbuf, 		// zmienna typu vs_const_buffer_t z pkt. 2d
		sizeof(constbuf)	// zmienna typu vs_const_buffer_t z pkt. 2d
	);
}