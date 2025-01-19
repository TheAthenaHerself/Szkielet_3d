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
using std::shared_ptr;

#include "Throws.h"
#include "Direct3d.h"
#include "rework.h"
#include "Application3D.h"
#include "ConstantBuffer.h"

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
	shared_ptr<Application_3d> app = nullptr;

	FLOAT angle = 0.0f;

	/*void LoadAssets() {
		//todo ended here

		/*D3D12_HEAP_PROPERTIES heap_prop = {
		  .Type = D3D12_HEAP_TYPE_UPLOAD,
		  .CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN,
		  .MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN,
		  .CreationNodeMask = 1,
		  .VisibleNodeMask = 1
		};
		D3D12_RESOURCE_DESC resource_desc = {
		  .Dimension = D3D12_RESOURCE_DIMENSION_BUFFER,
		  .Alignment = 0,
		  .Width = INSTANCE_BUFFER_SIZE,
		  .Height = 1,
		  .DepthOrArraySize = 1,
		  .MipLevels = 1,
		  .Format = DXGI_FORMAT_UNKNOWN,
		  .SampleDesc = {.Count = 1, .Quality = 0},
		  .Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR,
		  .Flags = D3D12_RESOURCE_FLAG_NONE
		};
		ThrowIfFailed(m_device->CreateCommittedResource(
			&heap_prop,
			D3D12_HEAP_FLAG_NONE,
			&resource_desc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&instance_buffer)
		), "create commited resource instance_buffer.");

		UINT8* dst_data = nullptr;
		D3D12_RANGE read_range = { 0, 0 };
		instance_buffer->Map(
			0, &read_range, reinterpret_cast<void**>(&dst_data));
		memcpy(dst_data, stalk_instances.data(), INSTANCE_BUFFER_SIZE);
		instance_buffer->Unmap(0, nullptr);

		instance_buffer_view.BufferLocation =
			instance_buffer->GetGPUVirtualAddress();
		instance_buffer_view.SizeInBytes = INSTANCE_BUFFER_SIZE;
		instance_buffer_view.StrideInBytes = sizeof(XMFLOAT4X4);*/

		/*LoadBitmapFromFile(
			TEXT("Obraz.jpg"), bmp_width, bmp_height, &bmp_bits, wic_factory);

	}*/
}

void OnInit(HWND _hwnd) {
	SetApp(app);
	app->hwnd = _hwnd;
	LoadPipeline();
	LoadAssets();
}

void OnUpdate(){
	angle += 1.0f / 512.0f;
	UpdateBuffer(app, angle);
}

void OnRender()
{
	PopulateCommandList();

	ID3D12CommandList* ppCommandLists[] = { app->m_commandList[app->m_frameIndex].Get() };
	app->m_commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

	ThrowIfFailed(app->m_swapChain->Present(1, 0), "Present");

	WaitForPreviousFrame();
}

void OnDestroy() {
	WaitForPreviousFrame();
	CloseHandle(app->m_fenceEvent);
}

void InitTimer(HWND hwnd) {
	app->timer = SetTimer(hwnd, 0, 0, 0);
}

void ReleaseTimer(HWND hwnd) {
	KillTimer(hwnd, app->timer);
}
