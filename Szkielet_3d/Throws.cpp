#include <stdexcept>
#include <d3d12.h>
#include "Throws.h"

void ThrowIfFailed(HRESULT error_code, std::string error) {
	if (FAILED(error_code)) {
		throw std::runtime_error("Fail at" + error);
	}
}