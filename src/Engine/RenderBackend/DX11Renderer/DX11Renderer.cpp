/*-----------------------------------------------------------------------------------------------
The MIT License (MIT)

Copyright (c) 2015-2020 OSRE ( Open Source Render Engine ) by Kim Kulling

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
-----------------------------------------------------------------------------------------------*/
#include "DX11Renderer.h"
#include <osre/Common/Logger.h>
#include <osre/Platform/AbstractWindow.h>
#include <osre/Platform/AbstractOGLRenderContext.h>
#include <osre/RenderBackend/Shader.h>
#include <src/Engine/Platform/win32/Win32Window.h>

#pragma warning( push )
#   pragma warning( disable : 4005 )
#   include <d3d11.h>
#   include <D3Dcompiler.h>
#pragma warning( pop )

#pragma warning( push )
#   pragma warning( disable : 4201 )
#   include <glm/gtc/matrix_transform.hpp>
#pragma warning( pop )


#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "D3DCompiler.lib")

namespace OSRE {
namespace RenderBackend {

static const c8 *DefaultVertexShader = "";
    
static const c8 *DefaultPixelShader = "";

using namespace ::OSRE::Common;

static const c8 *Tag = "DX11Renderer";

DX11Renderer::DX11Renderer() 
: m_vsync_enabled( true )
, m_videoCardMemory( 0 )
, m_swapChain( nullptr )
, m_device( nullptr )
, m_deviceContext( nullptr )
, m_renderTargetView( nullptr )
, m_depthStencilBuffer( nullptr )
, m_depthStencilState( nullptr )
, m_depthStencilView( nullptr )
, m_rasterState( nullptr ) {
    ::memset(m_videoCardDescription, '\0', 128);
}

DX11Renderer::~DX11Renderer() {
    // empty
}

bool DX11Renderer::create(Platform::AbstractWindow *surface) {
    if (nullptr == surface) {
        return false;
    }

    DXGI_SWAP_CHAIN_DESC swapChainDesc;
    D3D_FEATURE_LEVEL featureLevel;
    ID3D11Texture2D* backBufferPtr;
    D3D11_TEXTURE2D_DESC depthBufferDesc;
    D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
    D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
    D3D11_RASTERIZER_DESC rasterDesc;
    D3D11_VIEWPORT viewport;
    f32 fieldOfView, screenAspect;

    // Create a DirectX graphics interface factory.
    IDXGIFactory *factory;
    HRESULT result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
    if (FAILED(result)) {
        return false;
    }

    // Use the factory to create an adapter for the primary graphics interface (video card).
    IDXGIAdapter *adapter;
    result = factory->EnumAdapters(0, &adapter);
    if (FAILED(result)) {
        return false;
    }

    // Enumerate the primary adapter output (monitor).
    IDXGIOutput *adapterOutput;
    result = adapter->EnumOutputs(0, &adapterOutput);
    if (FAILED(result)) {
        return false;
    }

    // Get the number of modes that fit the DXGI_FORMAT_R8G8B8A8_UNORM display format for the adapter output (monitor).
    unsigned int numModes;
    result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);
    if (FAILED(result)) {
        return false;
    }

    // Create a list to hold all the possible display modes for this monitor/video card combination.
    DXGI_ADAPTER_DESC adapterDesc;
    DXGI_MODE_DESC *displayModeList = new DXGI_MODE_DESC[numModes];
    if (!displayModeList) {
        return false;
    }

    // Now fill the display mode list structures.
    result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList);
    if (FAILED(result)) {
        return false;
    }

    // Now go through all the display modes and find the one that matches the screen width and height.
    // When a match is found store the numerator and denominator of the refresh rate for that monitor.
    ui32 screenWidth = surface->getProperties()->m_width;
    ui32 screenHeight = surface->getProperties()->m_height;
    ui32 numerator(0), denominator( 0 );
    for (ui32 i = 0; i < numModes; i++) {
        if (displayModeList[i].Width == (unsigned int)screenWidth) {
            if (displayModeList[i].Height == (unsigned int)screenHeight) {
                numerator = displayModeList[i].RefreshRate.Numerator;
                denominator = displayModeList[i].RefreshRate.Denominator;
            }
        }
    }

    // Get the adapter (video card) description.
    result = adapter->GetDesc(&adapterDesc);
    if (FAILED(result)) {
        return false;
    }

    // Store the dedicated video card memory in megabytes.
    m_videoCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

    // Convert the name of the video card to a character array and store it.
    size_t stringLength(0);
    i32 error = wcstombs_s(&stringLength, m_videoCardDescription, 128, adapterDesc.Description, 128);
    if ( 0 != error ) {
        return false;
    }

    // Release the display mode list.
    delete[] displayModeList;
    displayModeList = nullptr;

    // Release the adapter output.
    SafeRelease(adapterOutput);

    // Release the adapter.
    SafeRelease(adapter);

    // Release the factory.
    SafeRelease(factory);

    // Initialize the swap chain description.
    ::ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

    // Set to a single back buffer.
    swapChainDesc.BufferCount = 1;

    // Set the width and height of the back buffer.
    swapChainDesc.BufferDesc.Width = screenWidth;
    swapChainDesc.BufferDesc.Height = screenHeight;

    // Set regular 32-bit surface for the back buffer.
    swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

    // Set the refresh rate of the back buffer.
    if (m_vsync_enabled) {
        swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
        swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
    } else {
        swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
        swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
    }

    // Set the usage of the back buffer.
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

    // Set the handle for the window to render to.
    Platform::Win32Window *osSurface = (Platform::Win32Window*)surface;
    swapChainDesc.OutputWindow = osSurface->getHWnd();

    // Turn multi-sampling off.
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;

    // Set to full screen or windowed mode.
    const bool fullscreen = surface->getProperties()->m_fullscreen;
    if (fullscreen) {
        swapChainDesc.Windowed = false;
    } else {
        swapChainDesc.Windowed = true;
    }

    // Set the scan line ordering and scaling to unspecified.
    swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

    // Discard the back buffer contents after presenting.
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    // Don't set the advanced flags.
    swapChainDesc.Flags = 0;

    // Set the feature level to DirectX 11.
    featureLevel = D3D_FEATURE_LEVEL_11_0;

    // Create the swap chain, Direct3D device, and Direct3D device context.
    result = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, &featureLevel, 1,
        D3D11_SDK_VERSION, &swapChainDesc, &m_swapChain, &m_device, NULL, &m_deviceContext);
    if (FAILED(result)) {
        return false;
    }

    // Get the pointer to the back buffer.
    result = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr);
    if (FAILED(result)) {
        return false;
    }

    // Create the render target view with the back buffer pointer.
    result = m_device->CreateRenderTargetView(backBufferPtr, NULL, &m_renderTargetView);
    if (FAILED(result)) {
        return false;
    }

    // Release pointer to the back buffer as we no longer need it.
    backBufferPtr->Release();
    backBufferPtr = 0;

    // Initialize the description of the depth buffer.
    ::ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

    // Set up the description of the depth buffer.
    depthBufferDesc.Width = screenWidth;
    depthBufferDesc.Height = screenHeight;
    depthBufferDesc.MipLevels = 1;
    depthBufferDesc.ArraySize = 1;
    depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthBufferDesc.SampleDesc.Count = 1;
    depthBufferDesc.SampleDesc.Quality = 0;
    depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    depthBufferDesc.CPUAccessFlags = 0;
    depthBufferDesc.MiscFlags = 0;

    // Create the texture for the depth buffer using the filled out description.
    result = m_device->CreateTexture2D(&depthBufferDesc, NULL, &m_depthStencilBuffer);
    if (FAILED(result)) {
        return false;
    }

    // Initialize the description of the stencil state.
    ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

    // Set up the description of the stencil state.
    depthStencilDesc.DepthEnable = true;
    depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

    depthStencilDesc.StencilEnable = true;
    depthStencilDesc.StencilReadMask = 0xFF;
    depthStencilDesc.StencilWriteMask = 0xFF;

    // Stencil operations if pixel is front-facing.
    depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
    depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    // Stencil operations if pixel is back-facing.
    depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
    depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    // Create the depth stencil state.
    result = m_device->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilState);
    if (FAILED(result)) {
        return false;
    }

    // Set the depth stencil state.
    m_deviceContext->OMSetDepthStencilState(m_depthStencilState, 1);


    // Initialize the depth stencil view.
    ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

    // Set up the depth stencil view description.
    depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    depthStencilViewDesc.Texture2D.MipSlice = 0;

    // Create the depth stencil view.
    result = m_device->CreateDepthStencilView(m_depthStencilBuffer, &depthStencilViewDesc, &m_depthStencilView);
    if (FAILED(result)) {
        return false;
    }

    // Bind the render target view and depth stencil buffer to the output render pipeline.
    m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);

    // Setup the raster description which will determine how and what polygons will be drawn.
    rasterDesc.AntialiasedLineEnable = false;
    rasterDesc.CullMode = D3D11_CULL_BACK;
    rasterDesc.DepthBias = 0;
    rasterDesc.DepthBiasClamp = 0.0f;
    rasterDesc.DepthClipEnable = true;
    rasterDesc.FillMode = D3D11_FILL_SOLID;
    rasterDesc.FrontCounterClockwise = false;
    rasterDesc.MultisampleEnable = false;
    rasterDesc.ScissorEnable = false;
    rasterDesc.SlopeScaledDepthBias = 0.0f;

    // Create the rasterizer state from the description we just filled out.
    result = m_device->CreateRasterizerState(&rasterDesc, &m_rasterState);
    if (FAILED(result)) {
        return false;
    }

    // Now set the rasterizer state.
    m_deviceContext->RSSetState(m_rasterState);

    // Setup the viewport for rendering.
    viewport.Width = (float)screenWidth;
    viewport.Height = (float)screenHeight;
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    viewport.TopLeftX = 0.0f;
    viewport.TopLeftY = 0.0f;

    // Create the viewport.
    m_deviceContext->RSSetViewports(1, &viewport);

    // Setup the projection matrix.
    fieldOfView = (float)XM_PI / 4.0f;
    screenAspect = (float)screenWidth / (float)screenHeight;

    // Create the projection matrix for 3D rendering.
    f32 screenNear = 0.1f, screenDepth = 1000.0f;
    m_projectionMatrix = glm::perspectiveFovLH( fieldOfView, (float)screenWidth, (float)screenHeight, screenNear, screenDepth );
    
    // Initialize the world matrix to the identity matrix.
    m_worldMatrix = glm::mat4( 1 );

    // Create an orthographic projection matrix for 2D rendering.
    m_orthoMatrix = glm::orthoLH( 0.f, (f32) screenWidth, 0.f, (f32) screenHeight, screenNear, screenDepth );

    return true;
}

bool DX11Renderer::destroy() {
    if (nullptr != m_swapChain) {
        m_swapChain->SetFullscreenState(false, NULL);
    }

    SafeRelease(m_rasterState);
    SafeRelease(m_depthStencilView);
    SafeRelease(m_depthStencilState);
    SafeRelease(m_depthStencilBuffer);
    SafeRelease(m_renderTargetView);
    SafeRelease(m_deviceContext);
    SafeRelease(m_device);
    SafeRelease(m_swapChain);
    
    return true;
}

ui32 translateVBEnum2DX11(BufferType type) {
    switch (type) {
        case BufferType::VertexBuffer:
            return D3D11_BIND_VERTEX_BUFFER;
        case BufferType::IndexBuffer:
            return D3D11_BIND_INDEX_BUFFER;
        case BufferType::ConstantBuffer:
            return D3D11_BIND_CONSTANT_BUFFER;
        default:
            break;
    }

    return 0;
}

static UINT translateAccessFlag(BufferAccessType usage) {
    switch (usage) {
        case BufferAccessType::ReadOnly:
            return D3D11_CPU_ACCESS_READ;
        case BufferAccessType::WriteOnly:
            return D3D11_CPU_ACCESS_WRITE;
        case BufferAccessType::ReadWrite:
            return D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
    }

    return 0;
}

ID3D11Buffer *DX11Renderer::createBuffer(BufferType type, BufferData *bd, BufferAccessType usage) {
    // Set up the description for the buffer.
    D3D11_BUFFER_DESC bufferDesc;
    bufferDesc.Usage = D3D11_USAGE_DEFAULT;
    bufferDesc.ByteWidth = (UINT) bd->getSize();
    bufferDesc.BindFlags = translateVBEnum2DX11(type);
    bufferDesc.CPUAccessFlags = translateAccessFlag(usage);
    bufferDesc.MiscFlags = 0;
    bufferDesc.StructureByteStride = 0;

    // Give the subresource structure a pointer to the vertex data.
    D3D11_SUBRESOURCE_DATA bufferData;
    bufferData.pSysMem = bd->getData();
    bufferData.SysMemPitch = 0;
    bufferData.SysMemSlicePitch = 0;

    // Now create the vertex buffer.
    ID3D11Buffer *buffer;
    HRESULT result = m_device->CreateBuffer(&bufferDesc, &bufferData, &buffer);
    if (FAILED(result)) {
        return nullptr;
    }

    return buffer;
}

void DX11Renderer::releaseBuffer(ID3D11Buffer *buffer) {
    if (nullptr == buffer) {
        return;
    }

    SafeRelease(buffer);
}

static bool getDx11Component(VertexAttribute attrib, String &name, DXGI_FORMAT &dx11Format ) {
    bool result(true);
    switch (attrib) {
        case VertexAttribute::Position:
            name = "POSITION";
            dx11Format = DXGI_FORMAT_R32G32B32_FLOAT;
            break;
        case VertexAttribute::Normal:
            name = "NORMAL";
            dx11Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
            break;
        case VertexAttribute::TexCoord0:
            name = "TEXCOORD0";
            dx11Format = DXGI_FORMAT_R32G32B32_FLOAT;
            break;
        case VertexAttribute::TexCoord1:
            name = "TEXCOORD1";
            dx11Format = DXGI_FORMAT_R32G32B32_FLOAT;
            break;
        case VertexAttribute::TexCoord2:
            name = "TEXCOORD2";
            dx11Format = DXGI_FORMAT_R32G32B32_FLOAT;
            break;
        case VertexAttribute::TexCoord3:
            name = "TEXCOORD3";
            dx11Format = DXGI_FORMAT_R32G32B32_FLOAT;
            break;
        case VertexAttribute::Tangent:
            name = "TANGENT";
            dx11Format = DXGI_FORMAT_R32G32B32_FLOAT;
            break;
        case VertexAttribute::Binormal:
            name = "BINORMAL";
            dx11Format = DXGI_FORMAT_R32G32B32_FLOAT;
            break;
        case VertexAttribute::Weights:
            name = "BLENDWEIGHT0";
            dx11Format = DXGI_FORMAT_R32G32B32_FLOAT;
            break;
        case VertexAttribute::Indices:
            name = "BLENDINDICES0";
            dx11Format = DXGI_FORMAT_R32G32B32_FLOAT;
            break;
        case VertexAttribute::Color0:
            name = "COLOR0";
            dx11Format = DXGI_FORMAT_R32G32B32_FLOAT;
            break;
        case VertexAttribute::Color1:
            name = "COLOR1";
            dx11Format = DXGI_FORMAT_R32G32B32_FLOAT;
            break;
        case VertexAttribute::Instance0:
            name = "NONE";
            result = false;
            dx11Format = DXGI_FORMAT_R32G32B32_FLOAT;
            break;
        case VertexAttribute::Instance1:
            name = "NONE";
            result = false;
            dx11Format = DXGI_FORMAT_R32G32B32_FLOAT;
            break;
        case VertexAttribute::Instance2:
            name = "NONE";
            dx11Format = DXGI_FORMAT_R32G32B32_FLOAT;
            result = false;
            break;
        case VertexAttribute::Instance3:
            name = "NONE";
            dx11Format = DXGI_FORMAT_R32G32B32_FLOAT;
            result = false;
            break;
        default:
            result = false;
            break;
    }

    return result;
}

DX11VertexLayout *DX11Renderer::createVertexLayout(VertexLayout *layout, DX11Shader *shader ) {
    if (nullptr == layout || nullptr == shader) {
        return nullptr;
    }

    String src;
    HRESULT result;
    D3D11_BUFFER_DESC matrixBufferDesc;

    // Create the vertex shader from the buffer.
    String name;
    DXGI_FORMAT dx11Format;
    const size_t numComps = layout->m_components.size();
    D3D11_INPUT_ELEMENT_DESC *dx11VertexDecl = new D3D11_INPUT_ELEMENT_DESC[numComps];
    for (size_t i = 0; i < numComps; ++i) {
        VertComponent &comp = layout->getAt( i );
        getDx11Component(comp.m_attrib, name, dx11Format);
        dx11VertexDecl[ i ].SemanticName = name.c_str();
        dx11VertexDecl[ i ].SemanticIndex = 0;
        dx11VertexDecl[ i ].Format = dx11Format;
        dx11VertexDecl[ i ].InputSlot = 0;
        if (0 == i) {
            dx11VertexDecl[i].AlignedByteOffset = 0;
        } else {
            dx11VertexDecl[i].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
        }
        dx11VertexDecl[i].InputSlot = D3D11_INPUT_PER_VERTEX_DATA;
        dx11VertexDecl[i].InputSlot = 0;
    }
    ID3D11InputLayout *dx11Layout( nullptr );
    result = m_device->CreateInputLayout(dx11VertexDecl, (UINT) numComps, shader->m_vsBuffer->GetBufferPointer(),
        shader->m_vsBuffer->GetBufferSize(), &dx11Layout);
    if (FAILED(result)) {
        return nullptr;
    }
    
    // Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
    matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    matrixBufferDesc.ByteWidth = sizeof( MatrixBufferType );
    matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    matrixBufferDesc.MiscFlags = 0;
    matrixBufferDesc.StructureByteStride = 0;

    // Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
    result = m_device->CreateBuffer( &matrixBufferDesc, NULL, &m_matrixBuffer );
    if (FAILED( result )) {
        return nullptr;
    }

    DX11VertexLayout *vl = new DX11VertexLayout;
    vl->m_desc = dx11VertexDecl;
    
    return vl;
}

static void ShowCompileError(ID3D10Blob *errorMessage) {
    if (nullptr == errorMessage) {
        return;
    }

    const char *compileErrors = static_cast<char*>(errorMessage->GetBufferPointer());
    const size_t size( errorMessage->GetBufferSize() );
    if (nullptr != compileErrors) {
        osre_error(Tag, compileErrors);
    }
}

DX11Shader *DX11Renderer::createShader(Shader *shader) {
    if (nullptr == shader) {
        return nullptr;
    }

    HRESULT result;
    DX11Shader *dx11Shader(nullptr);
    String src;
    ID3D10Blob *buffer, *errorMessage;
    for (ui32 i = 0; i < static_cast<ui32>(ShaderType::NumShaderTypes); ++i) {
        src = shader->m_src[ i ];
        result = D3DCompile(src.c_str(), src.size(), NULL, NULL, NULL, "ColorVertexShader", "vs_5_0", 
            D3D10_SHADER_ENABLE_STRICTNESS, 0, &buffer, &errorMessage);
        if (FAILED(result)) {
            if (nullptr != errorMessage) {
                ShowCompileError(errorMessage);
            } 

            return dx11Shader;
        }

        dx11Shader = new DX11Shader;
        const ShaderType type(static_cast<ShaderType>(i));
        switch( type ) {
            case ShaderType::SH_VertexShaderType:
                result = m_device->CreateVertexShader(buffer, buffer->GetBufferSize(), NULL, &dx11Shader->m_vertexShader);
                dx11Shader->m_vsBuffer = buffer;
                break;
            case ShaderType::SH_GeometryShaderType:
            case ShaderType::SH_TesselationShaderType:
                break;
            case ShaderType::SH_FragmentShaderType:
                result = m_device->CreatePixelShader(buffer, buffer->GetBufferSize(), NULL, &dx11Shader->m_pixelShader);
                break;
        }
    }

    return dx11Shader;
}

void DX11Renderer::setMatrix(MatrixType type, const glm::mat4 &mat) {
    switch (type) {
        case MatrixType::Model:
            m_worldMatrix = mat;
            break;
        case MatrixType::View:
            m_orthoMatrix = mat;
            break;
        case MatrixType::Projection:
            m_projectionMatrix = mat;
            break;
        default:
            break;
    }
}

static const glm::mat4 Dummy(1);

const glm::mat4 &DX11Renderer::getMatrix(MatrixType type) const {
    switch (type) {
        case MatrixType::Model:
            return m_worldMatrix;

        case MatrixType::View:
            return m_orthoMatrix;

        case MatrixType::Projection:
            return m_projectionMatrix;
        default:
            break;
    }
    return Dummy;
}

void DX11Renderer::setConstantBuffer(ui32 bufferNumber, ID3D11Buffer *constantBuffer) {
    if (nullptr == constantBuffer) {
        return;
    }

    m_deviceContext->CSSetConstantBuffers(bufferNumber, 1, &constantBuffer);
}

void DX11Renderer::beginScene(Color4 &clearColor) {
    // Setup the color to clear the buffer to.
    float color[4];
    color[0] = clearColor.m_r;
    color[1] = clearColor.m_g;
    color[2] = clearColor.m_b;
    color[3] = clearColor.m_a;

    // Clear the back buffer.
    m_deviceContext->ClearRenderTargetView(m_renderTargetView, color);

    // Clear the depth buffer.
    m_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void DX11Renderer::render(RenderCmd *cmd) {
    // Set vertex buffer stride and offset.
    ui32 stride = sizeof(RenderVert);
    ui32 offset = 0;

    // Set the vertex buffer to active in the input assembler so it can be rendered.
    
    m_deviceContext->IASetVertexBuffers(0, 1, &cmd->m_vb, &stride, &offset);

    // Set the index buffer to active in the input assembler so it can be rendered.
    m_deviceContext->IASetIndexBuffer(cmd->m_ib, DXGI_FORMAT_R32_UINT, 0);

    // Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
    m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void DX11Renderer::endScene() {
    // Present the back buffer to the screen since rendering is complete.
    if (m_vsync_enabled) {
        // Lock to screen refresh rate.
        m_swapChain->Present(1, 0);
    } else {
        // Present as fast as possible.
        m_swapChain->Present(0, 0);
    }
}

} // Namespace RenderBackend
} // Namespace OSRE
