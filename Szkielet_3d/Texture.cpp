#include <d3d12.h>
#include <wincodec.h>
#include <array>
#include <wrl.h>

#include "Application3D.h"
#include "Throws.h"

using Microsoft::WRL::ComPtr;

/*HRESULT LoadBitmapFromFile(
    PCWSTR uri, UINT& width, UINT& height, BYTE** ppBits, ComPtr<IWICImagingFactory> wic_factory) {
    HRESULT hr;
    ComPtr<IWICBitmapDecoder> pDecoder = nullptr;
    ComPtr<IWICBitmapFrameDecode> pSource = nullptr;
    ComPtr<IWICFormatConverter> pConverter = nullptr;

    hr = wic_factory->CreateDecoderFromFilename(
        uri, nullptr, GENERIC_READ, WICDecodeMetadataCacheOnLoad,
        pDecoder.GetAddressOf());

    if (SUCCEEDED(hr)) {
        hr = pDecoder->GetFrame(0, pSource.GetAddressOf());
    }
    if (SUCCEEDED(hr)) {
        hr = wic_factory->CreateFormatConverter(
            pConverter.GetAddressOf());
    }
    if (SUCCEEDED(hr)) {
        hr = pConverter->Initialize(
            pSource.Get(), GUID_WICPixelFormat32bppRGBA,
            WICBitmapDitherTypeNone, nullptr, 0.0f,
            WICBitmapPaletteTypeMedianCut);
    }
    if (SUCCEEDED(hr)) {
        hr = pConverter->GetSize(&width, &height);
    }
    if (SUCCEEDED(hr)) {
        *ppBits = new BYTE[4 * width * height];
        hr = pConverter->CopyPixels(
            nullptr, 4 * width, 4 * width * height, *ppBits);
    }
    return hr;
}

void RootSignature(ComPtr<ID3D12RootSignature> m_rootSignature, ComPtr<ID3D12Device> m_device) {
    D3D12_DESCRIPTOR_RANGE descriptor_ranges[] = {
  {
    .RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV,
    .NumDescriptors = 1,
    .BaseShaderRegister = 0,
    .RegisterSpace = 0,
    .OffsetInDescriptorsFromTableStart =
        D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND
  }, {
    .RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV,
    .NumDescriptors = 1,
    .BaseShaderRegister = 0,
    .RegisterSpace = 0,
    .OffsetInDescriptorsFromTableStart =
       D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND
  }
    };
    D3D12_ROOT_PARAMETER root_params[] = {
      {
        .ParameterType =
           D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE,
        .DescriptorTable = {1, &descriptor_ranges[0]},
        .ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX
      }, {
        .ParameterType =
           D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE,
        .DescriptorTable = {1, &descriptor_ranges[1]},
        .ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL
      }
    };
    D3D12_STATIC_SAMPLER_DESC tex_sampler_desc = {
      .Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR,
      //D3D12_FILTER_MIN_MAG_MIP_POINT, D3D12_FILTER_ANISOTROPIC
     .AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP,
     //_MODE_MIRROR, _MODE_CLAMP, _MODE_BORDER
   .AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP,
   .AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP,
   .MipLODBias = 0,
   .MaxAnisotropy = 0,
   .ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER,
   .BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK,
   .MinLOD = 0.0f,
   .MaxLOD = D3D12_FLOAT32_MAX,
   .ShaderRegister = 0,
   .RegisterSpace = 0,
   .ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL
    };
    D3D12_ROOT_SIGNATURE_DESC root_signature_desc = {
      .NumParameters = _countof(root_params),
      .pParameters = root_params,
      .NumStaticSamplers = 1,
      .pStaticSamplers = &tex_sampler_desc,
      .Flags =
        D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
        D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
        D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
        D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS
    };
    ComPtr<ID3DBlob> signature;
    ComPtr<ID3DBlob> error;
    ThrowIfFailed(D3D12SerializeRootSignature(&root_signature_desc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error), "serialize root signature");
    ThrowIfFailed(m_device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_rootSignature)), "create root signature");
}

void ConstBuf(ComPtr<ID3D12Device> m_device, ComPtr<ID3D12DescriptorHeap> constbufDescHeap) {
    D3D12_DESCRIPTOR_HEAP_DESC heap_desc = {
      .Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
      .NumDescriptors = 2,
      .Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE,
      .NodeMask = 0
     };
    ThrowIfFailed(m_device->CreateDescriptorHeap(&heap_desc, IID_PPV_ARGS(&constbufDescHeap)), "DescriptorHeapConstBuf");
}

void TexResource(ComPtr<ID3D12Device> m_device, ComPtr<ID3D12Resource> texture_resource, 
    ComPtr<ID3D12GraphicsCommandList> m_commandList, ComPtr<ID3D12PipelineState> m_pipelineState, ComPtr<ID3D12CommandAllocator> m_commandAllocator) {
    D3D12_HEAP_PROPERTIES tex_heap_prop = {
      .Type = D3D12_HEAP_TYPE_DEFAULT,
      .CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN,
      .MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN,
      .CreationNodeMask = 1,
      .VisibleNodeMask = 1
    };
    D3D12_RESOURCE_DESC tex_resource_desc = {
      .Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D,
      .Alignment = 0,
      .Width = bmp_width,
      .Height = bmp_height,
      .DepthOrArraySize = 1,
      .MipLevels = 1,
      .Format = DXGI_FORMAT_R8G8B8A8_UNORM,
      .SampleDesc = {.Count = 1, .Quality = 0},
      .Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN,
      .Flags = D3D12_RESOURCE_FLAG_NONE
    };
    ThrowIfFailed(m_device->CreateCommittedResource(
        &tex_heap_prop, D3D12_HEAP_FLAG_NONE,
        &tex_resource_desc, D3D12_RESOURCE_STATE_COPY_DEST,
        nullptr, IID_PPV_ARGS(&texture_resource)),"resource texture");

    ComPtr<ID3D12Resource> texture_upload_buffer = nullptr;

    UINT64 required_size = 0;
    D3D12_PLACED_SUBRESOURCE_FOOTPRINT layout;
    UINT num_rows;
    UINT64 row_size_in_bytes;
    D3D12_RESOURCE_DESC resource_desc =
        texture_resource.Get()->GetDesc();
    ComPtr<ID3D12Device> helper_device = nullptr;
    texture_resource.Get()->GetDevice(
        __uuidof(helper_device),
        reinterpret_cast<void**>(helper_device.GetAddressOf()));
    helper_device->GetCopyableFootprints(
        &resource_desc, 0, 1, 0, &layout,
        &num_rows, &row_size_in_bytes, &required_size);

    // - utworzenie tego pom. bufora
    D3D12_HEAP_PROPERTIES tex_upload_heap_prop = {
        .Type = D3D12_HEAP_TYPE_UPLOAD,
        .CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN,
        .MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN,
        .CreationNodeMask = 1,
        .VisibleNodeMask = 1
    };
    D3D12_RESOURCE_DESC tex_upload_resource_desc = {
       .Dimension = D3D12_RESOURCE_DIMENSION_BUFFER,
       .Alignment = 0,
       .Width = required_size,
       .Height = 1,
       .DepthOrArraySize = 1,
       .MipLevels = 1,
       .Format = DXGI_FORMAT_UNKNOWN,
       .SampleDesc = {.Count = 1, .Quality = 0},
       .Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR,
       .Flags = D3D12_RESOURCE_FLAG_NONE
    };
    ThrowIfFailed(m_device->CreateCommittedResource(
        &tex_upload_heap_prop, D3D12_HEAP_FLAG_NONE,
        &tex_upload_resource_desc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr, IID_PPV_ARGS(&texture_upload_buffer)), "texture upload buffer resource");

    // - skopiowanie danych tekstury do pom. bufora
    D3D12_SUBRESOURCE_DATA texture_data = {
        .pData = bmp_bits,
        .RowPitch = bmp_width * BMP_PX_SIZE,
        .SlicePitch = bmp_width * bmp_height * BMP_PX_SIZE
    };

    ThrowIfFailed(m_commandList->Reset(m_commandAllocator.Get(), m_pipelineState.Get()), "reset command list");

    UINT8* map_tex_data = nullptr;
    texture_upload_buffer->Map(
        0, nullptr, reinterpret_cast<void**>(&map_tex_data));
    D3D12_MEMCPY_DEST dest_data = {
        .pData = map_tex_data + layout.Offset,
        .RowPitch = layout.Footprint.RowPitch,
        .SlicePitch
            = SIZE_T(layout.Footprint.RowPitch) * SIZE_T(num_rows)
    };
    for (UINT z = 0; z < layout.Footprint.Depth; ++z) {
        auto pDestSlice = static_cast<UINT8*>(dest_data.pData)
            + dest_data.SlicePitch * z;
        auto pSrcSlice
            = static_cast<const UINT8*>(texture_data.pData)
            + texture_data.SlicePitch * LONG_PTR(z);
        for (UINT y = 0; y < num_rows; ++y) {
            memcpy(pDestSlice + dest_data.RowPitch * y,
                pSrcSlice + texture_data.RowPitch * LONG_PTR(y),
                static_cast<SIZE_T>(row_size_in_bytes));
        }
    }
    texture_upload_buffer->Unmap(0, nullptr);

    // -  zlecenie procesorowi GPU skopiowania buf. pom. 
    //    do w³aœciwego zasobu tekstury
    D3D12_TEXTURE_COPY_LOCATION Dst = {
        .pResource = texture_resource.Get(),
        .Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX,
        .SubresourceIndex = 0
    };
    D3D12_TEXTURE_COPY_LOCATION Src = {
        .pResource = texture_upload_buffer.Get(),
        .Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT,
        .PlacedFootprint = layout
    };
    d3d12_cmd_list->CopyTextureRegion(
        &Dst, 0, 0, 0, &Src, nullptr);

    D3D12_RESOURCE_BARRIER tex_upload_resource_barrier = {
        .Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION,
        .Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE,
        .Transition = {
            .pResource = texture_resource.Get(),
            .Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES,
            .StateBefore = D3D12_RESOURCE_STATE_COPY_DEST,
            .StateAfter
                = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE },
    };
    d3d12_cmd_list->ResourceBarrier(
        1, &tex_upload_resource_barrier);
    d3d12_cmd_list->Close();
    ID3D12CommandList* cmd_list = d3d12_cmd_list.Get();
    d3d12_cmd_queue->ExecuteCommandLists(1, &cmd_list);

    // - tworzy SRV (widok zasobu shadera) dla tekstury
    D3D12_SHADER_RESOURCE_VIEW_DESC srv_desc = {
        .Format = tex_resource_desc.Format,
        .ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D,
        .Shader4ComponentMapping
            = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING,
        .Texture2D = {
            .MostDetailedMip = 0,
            .MipLevels = 1,
            .PlaneSlice = 0,
            .ResourceMinLODClamp = 0.0f
        },
    };
    D3D12_CPU_DESCRIPTOR_HANDLE cpu_desc_handle =
        d3d12_descriptor_heap->
        GetCPUDescriptorHandleForHeapStart();
    cpu_desc_handle.ptr +=
        d3d12_device->GetDescriptorHandleIncrementSize(
            D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    d3d12_device->CreateShaderResourceView(
        texture_resource.Get(), &srv_desc, cpu_desc_handle);


}*/