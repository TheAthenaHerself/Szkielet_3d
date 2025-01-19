#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>

struct vertex_t {
	FLOAT position[3];
	FLOAT color[4];
};

size_t const VERTEX_SIZE = sizeof(vertex_t) / sizeof(FLOAT);
vertex_t data[] = {
  { 0.0f, 1.0f, 0.5f,         0.0f, 1.0f, 0.0f, 1.0f },
  { 1.0f, 0.0f, 0.5f,         1.0f, 0.0f, 0.0f, 1.0f },
  { -1.0f, -1.0f, 0.5f,       0.0f, 0.0f, 1.0f, 1.0f }
};

size_t const VERTEX_BUFFER_SIZE = sizeof(data);
size_t const NUM_VERTICES = VERTEX_BUFFER_SIZE / sizeof(vertex_t);