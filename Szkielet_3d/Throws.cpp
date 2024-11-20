#include <stdexcept>
#include <d3d12.h>
#include "Throws.h"

void ThrowIfFailed(HRESULT error_code) {
	if (error_code != S_OK) {
		throw std::runtime_error("Failed to create factory.");
	}
}