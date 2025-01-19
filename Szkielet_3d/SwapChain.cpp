#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <memory>
using std::shared_ptr;
#include "Throws.h"
#include "Application3D.h"

using Microsoft::WRL::ComPtr;
#include "SwapChain.h"

void SwapChain(shared_ptr<Application_3d> &app) {
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
	ThrowIfFailed(app->factory->CreateSwapChainForHwnd(
		app->m_commandQueue.Get(),        // Swap chain needs the queue so that it can force a flush on it.
		app->hwnd,
		&swapChainDesc1,
		&swapChainFullscreen,
		nullptr,
		&swapChain
	), "SwapChainHwnd");

	ThrowIfFailed(swapChain.As(&app->m_swapChain), "swapChain3");
}