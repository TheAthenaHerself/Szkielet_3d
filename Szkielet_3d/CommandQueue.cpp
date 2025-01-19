#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <wincodec.h>
#include <memory>
using std::shared_ptr;
#include "Throws.h"
#include "Application3D.h"
#include "CommandQueue.h"

using Microsoft::WRL::ComPtr;

void CommandQueue(shared_ptr<Application_3d> &app) {
	D3D12_COMMAND_QUEUE_DESC queueDesc = {
			.Type = D3D12_COMMAND_LIST_TYPE_DIRECT,
			.Priority = D3D12_COMMAND_QUEUE_PRIORITY_HIGH,
			.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE,
			.NodeMask = 0,
	};

	ThrowIfFailed(app->m_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&app->m_commandQueue)), "CommandQueue");
}