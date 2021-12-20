#include "stdafx.h"
#include "Game.h"



Game::Game(UINT width, UINT height, std::wstring name) :
    DXSample(width, height, name),
    m_frameIndex(0),
    m_viewport(0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height)),
    m_scissorRect(0, 0, static_cast<LONG>(width), static_cast<LONG>(height)),
    m_rtvDescriptorSize(0),
    TextureWidth(width),
    TextureHeight(height)

{
    float aspect = static_cast<float>(width) / static_cast<float>(height);
    XMVECTOR origin = XMVectorSet(5.0f, 0.0f, 5.0f, 0.0f);
    XMVECTOR lookAt = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
    XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    m_camera = new Camera(origin, lookAt, up, 60.0f, aspect, 1.0f);
    m_Xprev = static_cast<float>(width) / 2.0f;
    m_Yprev = static_cast<float>(height) / 2.0f;
    m_firstClick = true;
    m_cameraMovementX = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
    m_cameraMovementZ = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
    m_fovState = 0;
    m_prevTime = 0.0f;
    m_barrelPower = 1.0f;
    m_barrelState = 0.0f;
    m_parallel = 1.0f;
    m_enableBarrel = -1.0f;
    m_texturing = -1.0f;
    m_samples = 1;
    m_depth = 1;
}

void Game::OnInit()
{
    


    LoadPipeline();
    LoadAssets();
}

// Load the rendering pipeline dependencies.
void Game::LoadPipeline()
{
    UINT dxgiFactoryFlags = 0;

#if defined(_DEBUG)
    // Enable the debug layer (requires the Graphics Tools "optional feature").
    // NOTE: Enabling the debug layer after device creation will invalidate the active device.
    {
        ComPtr<ID3D12Debug> debugController;
        if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
        {
            debugController->EnableDebugLayer();

            // Enable additional debug layers.
            dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
        }
    }
#endif

    ComPtr<IDXGIFactory4> factory;
    ThrowIfFailed(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&factory)));

    if (m_useWarpDevice)
    {
        ComPtr<IDXGIAdapter> warpAdapter;
        ThrowIfFailed(factory->EnumWarpAdapter(IID_PPV_ARGS(&warpAdapter)));

        ThrowIfFailed(D3D12CreateDevice(
            warpAdapter.Get(),
            D3D_FEATURE_LEVEL_11_0,
            IID_PPV_ARGS(&m_device)
        ));
    }
    else
    {
        ComPtr<IDXGIAdapter1> hardwareAdapter;
        GetHardwareAdapter(factory.Get(), &hardwareAdapter);

        ThrowIfFailed(D3D12CreateDevice(
            hardwareAdapter.Get(),
            D3D_FEATURE_LEVEL_11_0,
            IID_PPV_ARGS(&m_device)
        ));
    }

    // Describe and create the command queue.
    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

    ThrowIfFailed(m_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_commandQueue)));

    // Describe and create the swap chain.
    DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
    swapChainDesc.BufferCount = FrameCount;
    swapChainDesc.Width = m_width;
    swapChainDesc.Height = m_height;
    swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapChainDesc.SampleDesc.Count = 1;

    ComPtr<IDXGISwapChain1> swapChain;
    ThrowIfFailed(factory->CreateSwapChainForHwnd(
        m_commandQueue.Get(),        // Swap chain needs the queue so that it can force a flush on it.
        Win32Application::GetHwnd(),
        &swapChainDesc,
        nullptr,
        nullptr,
        &swapChain
    ));

    // This sample does not support fullscreen transitions.
    ThrowIfFailed(factory->MakeWindowAssociation(Win32Application::GetHwnd(), DXGI_MWA_NO_ALT_ENTER));

    ThrowIfFailed(swapChain.As(&m_swapChain));
    m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();

    // Create descriptor heaps.
    {
        // Describe and create a render target view (RTV) descriptor heap.
        D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
        rtvHeapDesc.NumDescriptors = FrameCount;
        rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        ThrowIfFailed(m_device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_rtvHeap)));

        // Describe and create a shader resource view (SRV) heap for the texture.
        D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
        srvHeapDesc.NumDescriptors = 1;
        srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        ThrowIfFailed(m_device->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&m_srvHeap)));

        m_rtvDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    }

    // Create frame resources.
    {
        CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart());

        // Create a RTV for each frame.
        for (UINT n = 0; n < FrameCount; n++)
        {
            ThrowIfFailed(m_swapChain->GetBuffer(n, IID_PPV_ARGS(&m_renderTargets[n])));
            m_device->CreateRenderTargetView(m_renderTargets[n].Get(), nullptr, rtvHandle);
            rtvHandle.Offset(1, m_rtvDescriptorSize);
        }
    }

    ThrowIfFailed(m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_commandAllocator)));
}

// Load the sample assets.
void Game::LoadAssets()
{
    // Create the root signature.
    {
        D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = {};

        // This is the highest version the sample supports. If CheckFeatureSupport succeeds, the HighestVersion returned will not be greater than this.
        featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;

        if (FAILED(m_device->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData, sizeof(featureData))))
        {
            featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
        }

        CD3DX12_DESCRIPTOR_RANGE1 ranges[1];
        ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_NONE);

        CD3DX12_ROOT_PARAMETER1 rootParameters[1];
        rootParameters[0].InitAsDescriptorTable(1, &ranges[0], D3D12_SHADER_VISIBILITY_PIXEL);

        D3D12_STATIC_SAMPLER_DESC sampler = {};
        sampler.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
        sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
        sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
        sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
        sampler.MipLODBias = 0;
        sampler.MaxAnisotropy = 0;
        sampler.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
        sampler.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
        sampler.MinLOD = 0.0f;
        sampler.MaxLOD = D3D12_FLOAT32_MAX;
        sampler.ShaderRegister = 0;
        sampler.RegisterSpace = 0;
        sampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

        CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
        rootSignatureDesc.Init_1_1(_countof(rootParameters), rootParameters, 1, &sampler, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

        ComPtr<ID3DBlob> signature;
        ComPtr<ID3DBlob> error;
        ThrowIfFailed(D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, featureData.HighestVersion, &signature, &error));
        ThrowIfFailed(m_device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_rootSignature)));
    }

    // Create the pipeline state, which includes compiling and loading shaders.
    {
        ComPtr<ID3DBlob> vertexShader;
        ComPtr<ID3DBlob> pixelShader;
        ComPtr<ID3DBlob> errors;

        UINT compileFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)  
        compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

        HRESULT hr = S_OK;

       

        
        hr = D3DCompileFromFile(GetAssetFullPath(L"shaders.hlsl").c_str(), nullptr, nullptr, "VSMain", "vs_5_0", compileFlags, 0, &vertexShader, &errors);
        if (errors != nullptr)
            OutputDebugStringA((char*)errors->GetBufferPointer());

        ThrowIfFailed(hr);
        hr = D3DCompileFromFile(GetAssetFullPath(L"shaders.hlsl").c_str(), nullptr, nullptr, "PSMain", "ps_5_0", compileFlags, 0, &pixelShader, &errors);
        if (errors != nullptr)
            OutputDebugStringA((char*)errors->GetBufferPointer());

        ThrowIfFailed(hr);
        // Define the vertex input layout.
        D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
        {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
            { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
        };

        // Describe and create the graphics pipeline state object (PSO).
        D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
        psoDesc.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
        psoDesc.pRootSignature = m_rootSignature.Get();
        psoDesc.VS = CD3DX12_SHADER_BYTECODE(vertexShader.Get());
        psoDesc.PS = CD3DX12_SHADER_BYTECODE(pixelShader.Get());
        psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
        psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
        psoDesc.DepthStencilState.DepthEnable = FALSE;
        psoDesc.DepthStencilState.StencilEnable = FALSE;
        psoDesc.SampleMask = UINT_MAX;
        psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        psoDesc.NumRenderTargets = 1;
        psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
        psoDesc.SampleDesc.Count = 1;
        ThrowIfFailed(m_device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_pipelineState)));
    }

    // Create the command list.
    ThrowIfFailed(m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocator.Get(), m_pipelineState.Get(), IID_PPV_ARGS(&m_commandList)));

    // Create the vertex buffer.
    {
        // Define the geometry for a triangle.
        Vertex triangleVertices[] =
        {
            // Triangle to produce quad for image post-processing
            // 3 |  \
            // 2 |   \
            // 1 |_ _ \
            // 0 |_|_| \
            //-1 |_|_|__\3
            //    0 1 
            { { -1.0f, -1.0f, 0.0f }, { 0.0f, 0.0f } },
            { { -1.0f, 3.0f , 0.0f }, { 0.0f, 2.0f } },
            { { 3.0f, -1.0f , 0.0f }, { 2.0f, 0.0f } }
        };

        const UINT vertexBufferSize = sizeof(triangleVertices);

        // Note: using upload heaps to transfer static data like vert buffers is not 
        // recommended. Every time the GPU needs it, the upload heap will be marshalled 
        // over. Please read up on Default Heap usage. An upload heap is used here for 
        // code simplicity and because there are very few verts to actually transfer.
        ThrowIfFailed(m_device->CreateCommittedResource(
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
            D3D12_HEAP_FLAG_NONE,
            &CD3DX12_RESOURCE_DESC::Buffer(vertexBufferSize),
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&m_vertexBuffer)));

        // Copy the triangle data to the vertex buffer.
        UINT8* pVertexDataBegin;
        CD3DX12_RANGE readRange(0, 0);        // We do not intend to read from this resource on the CPU.
        ThrowIfFailed(m_vertexBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pVertexDataBegin)));
        memcpy(pVertexDataBegin, triangleVertices, sizeof(triangleVertices));
        m_vertexBuffer->Unmap(0, nullptr);

        // Initialize the vertex buffer view.
        m_vertexBufferView.BufferLocation = m_vertexBuffer->GetGPUVirtualAddress();
        m_vertexBufferView.StrideInBytes = sizeof(Vertex);
        m_vertexBufferView.SizeInBytes = vertexBufferSize;
    }

    // Note: ComPtr's are CPU objects but this resource needs to stay in scope until
    // the command list that references it has finished executing on the GPU.
    // We will flush the GPU at the end of this method to ensure the resource is not
    // prematurely destroyed.
    

    // Create the texture.
    {
        // Describe and create a Texture2D.
        D3D12_RESOURCE_DESC textureDesc = {};
        textureDesc.MipLevels = 1;
        textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        textureDesc.Width = TextureWidth;
        textureDesc.Height = TextureHeight;
        textureDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
        textureDesc.DepthOrArraySize = 1;
        textureDesc.SampleDesc.Count = 1;
        textureDesc.SampleDesc.Quality = 0;
        textureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;

        ThrowIfFailed(m_device->CreateCommittedResource(
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
            D3D12_HEAP_FLAG_NONE,
            &textureDesc,
            D3D12_RESOURCE_STATE_COPY_DEST,
            nullptr,
            IID_PPV_ARGS(&m_texture)));

        const UINT64 uploadBufferSize = GetRequiredIntermediateSize(m_texture.Get(), 0, 1);

        // Create the GPU upload buffer.
        ThrowIfFailed(m_device->CreateCommittedResource(
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
            D3D12_HEAP_FLAG_NONE,
            &CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize),
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&m_textureUploadHeap)));

       
        // Describe and create a SRV for the texture.
        D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
        srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        srvDesc.Format = textureDesc.Format;
        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MipLevels = 1;
        m_device->CreateShaderResourceView(m_texture.Get(), &srvDesc, m_srvHeap->GetCPUDescriptorHandleForHeapStart());
    }

    // Close the command list and execute it to begin the initial GPU setup.
    ThrowIfFailed(m_commandList->Close());
    ID3D12CommandList* ppCommandLists[] = { m_commandList.Get() };
    m_commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

    // Create synchronization objects and wait until assets have been uploaded to the GPU.
    {
        ThrowIfFailed(m_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence)));
        m_fenceValue = 1;

        // Create an event handle to use for frame synchronization.
        m_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
        if (m_fenceEvent == nullptr)
        {
            ThrowIfFailed(HRESULT_FROM_WIN32(GetLastError()));
        }

        //m_geometry.push_back(std::shared_ptr<Intersectable>(new Sphere(
        //    XMVectorSet(-1.0f, -0.1f, -4.1f, 0.0f), 0.01f, Material(Material::Type::DIFFUSE, XMVectorSet(240.0f / 255.0f, 128.0f / 255.0f, 128.0f / 255.0f, 0.0f))
        //)));
        //m_geometry.push_back(std::shared_ptr<Intersectable>(new Sphere(
        //    XMVectorSet(-1.9f, -0.5f, -4.1f, 0.0f), 0.01f, Material(Material::Type::DIFFUSE, XMVectorSet(255.0f / 255.0f, 105.0f / 255.0f, 180.0f / 255.0f, 0.0f))
        //)));
        //m_geometry.push_back(std::shared_ptr<Intersectable>(new Sphere(
        //    XMVectorSet(-1.0f, -1.5f, -4.1f, 0.0f), 0.01f, Material(Material::Type::DIFFUSE, XMVectorSet(50.0f / 255.0f, 205.0f / 255.0f, 50.0f / 255.0f, 0.0f))
        //)));
        //m_geometry.push_back(std::shared_ptr<Intersectable>(new Sphere(
        //    XMVectorSet(-1.0f, 1.0f, 4.0f, 0.0f), 0.01f, Material(Material::Type::DIFFUSE, XMVectorSet(255.0f / 255.0f, 215.0f / 255.0f, 0.0f, 0.0f)))));
        //m_geometry.push_back(std::shared_ptr<Intersectable>(new Sphere(
        //    XMVectorSet(4.0f, 2.0f, 2.0f, 0.0f), 0.3f, Material(Material::Type::SPECULAR, XMVectorSet(255.0f / 255.0f, 50.0f / 255.0f, 100.0f / 255.0f, 0.0f)))));
        //m_geometry.push_back(std::shared_ptr<Intersectable>(new Sphere(
        //    XMVectorSet(1.0f, 2.0f, 2.0f, 0.0f), 0.3f, Material(Material::Type::SPECULAR, XMVectorSet(155.0f / 255.0f, 150.0f / 255.0f, 60.0f / 255.0f, 0.0f)))));
        //m_geometry.push_back(std::shared_ptr<Intersectable>(new Sphere(
        //    XMVectorSet(-5.0f, 1.0f, 2.0f, 0.0f), 0.2f, Material(Material::Type::SPECULAR, XMVectorSet(205.0f / 255.0f, 50.0f / 255.0f, 0, 0.0f)))));
        //m_geometry.push_back(std::shared_ptr<Intersectable>(new Sphere(
        //    XMVectorSet(-3.0f, 2.0f, 2.0f, 0.0f), 0.01f, Material(Material::Type::SPECULAR, XMVectorSet(255.0f / 255.0f, 50.0f / 255.0f, 150.0f / 255.0f, 0.0f)))));
        //m_geometry.push_back(std::shared_ptr<Intersectable>(new Sphere(
        //    XMVectorSet(2.0f, 0.0f, 2.0f, 0.0f), 1.0f, Material(Material::Type::DIELECTRIC, XMVectorSet(0.85f, 0.9f, 1.0f, 0.0f), 1.3f, XMVectorSet(0.7, 0.3, 0.7, 0.0f)))));
       
        //m_geometry.push_back(std::shared_ptr<Intersectable>(new Sphere(
        //    XMVectorSet(4.0f, -2.0f, 8.0f, 0.0f), 0.5f, Material(Material::Type::DIELECTRIC, XMVectorSet(0.35f, 0.4f, 1.0f, 0.0f), 1.5f, XMVectorSet(0.9, 0.8, 0.4, 0.0f)))));
       
       
        //m_geometry.push_back(std::shared_ptr<Intersectable>(new Plane(
        //    XMVectorSet(0.0f, -3.0f, 0.0f, 0.0f), XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), 40.0f, 40.0f, Material(Material::Type::DIFFUSE, XMVectorSet(1.0f, 0.0f, 1.0f, 0.0f))
        //)));

        /*m_geometry.push_back(std::shared_ptr<Intersectable>(new Torus(
            0.5f, 0.2f, Material(Material::Type::DIFFUSE, XMVectorSet(255.0f / 255.0f, 105.0f / 255.0f, 180.0f / 255.0f, 0.0f))
        )));*/

        /*m_geometry.push_back(std::shared_ptr<Intersectable>(new Triangle(
            { XMVectorSet(-2.0f, -1.0f, -2.0f, 0.0f), XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f) },
            { XMVectorSet(2.0f, -1.0f, -2.0f, 0.0f), XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f) },
            { XMVectorSet(2.0f, -1.0f, 2.0f, 0.0f), XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f) },
            Material(Material::Type::DIFFUSE, XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f))
        )));

        m_geometry.push_back(std::shared_ptr<Intersectable>(new Triangle(
            { XMVectorSet(-3.0f, -1.0f, -2.0f, 0.0f), XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f) },
            { XMVectorSet(2.0f, -1.0f, -2.0f, 0.0f), XMVectorSet(1.0f, 1.0f, 0.0f, 0.0f) },
            { XMVectorSet(2.0f, -4.0f, 2.0f, 0.0f), XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f) },
            Material(Material::Type::DIFFUSE, XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f))
        )));

        m_geometry.push_back(std::shared_ptr<Intersectable>(new Triangle(
            { XMVectorSet(-2.0f, -1.0f, -1.0f, 0.0f), XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f) },
            { XMVectorSet(2.0f, 0.0f, -2.0f, 0.0f), XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f) },
            { XMVectorSet(2.0f, -1.0f, 2.0f, 0.0f), XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f) },
            Material(Material::Type::DIFFUSE, XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f))
        )));*/
        XMMATRIX rotation = XMMatrixRotationAxis(XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f), -0.5f * XM_PI);
        XMMATRIX translation = XMMatrixTranslation(-1.0f, 2.0f, 0.0f);
        XMMATRIX scale = XMMatrixScaling(0.01f, 0.01f, 0.01f);
        XMMATRIX obj = XMMatrixIdentity();
        XMMATRIX objToWorld = obj  * rotation * scale * translation;
        
        Mesh* bunny = new Mesh(objToWorld, Material(Material::Type::DIFFUSE, XMVectorSet(50.0f / 255.0f, 205.0f / 255.0f, 50.0f / 255.0f, 0.0f)), m_geometry);
        m_BVH = new BVH(m_geometry, 4);
       /* m_lights.push_back(Light(XMVectorSet(-1.0f, 1.f, -4.0f, 0.0f), XMVectorSet(0.0f, -1.0f, 0.0f, 0.0f), XMVectorSet(1.0f, 1.0f, 1.0f, 0.f), 10.0f, Light::Type::SPOT, 15 * XM_PI / 180.0f, 5.0f * XM_PI / 180.0f));
        m_lights.push_back(Light(XMVectorSet(-1.0f, 2.f, -4.0f, 0.0f), XMVectorSet(0.0f, -1.0f, 0.0f, 0.0f), XMVectorSet(0.5f, 0.78f, 0.9f, 0.0f), 10.0f, Light::Type::SPOT, 15 * XM_PI / 180.0f, 5.0f * XM_PI / 180.0f));
        m_lights.push_back(Light(XMVectorSet(8.0f, 1.5f, -4.0f, 0.0f), XMVectorSet(0.0f, -1.0f, 0.0f, 0.0f), XMVectorSet(1.0f, 0.0f, 1.0f, 0.0f), 10.0f, Light::Type::SPOT, 45 * XM_PI / 180.0f, 10.0f * XM_PI / 180.0f));
        m_lights.push_back(Light(XMVectorSet(-8.0f, 1.5f, -4.0f, 0.0f), XMVectorSet(0.0f, -1.0f, 0.0f, 0.0f), XMVectorSet(1.0f, 1.0f, 0.0f, 0.0f), 10.0f, Light::Type::SPOT, 50 * XM_PI / 180.0f, 35.0f * XM_PI / 180.0f));
        */m_lights.push_back(Light(XMVectorSet(2.0f, 4.5f, 0.0f, 0.0f), XMVectorSet(1.0f, 0.7f, 0.9f, 0.0f), 10.0f, Light::Type::POINT));
        m_lights.push_back(Light(XMVectorSet(0.0f, 0.0f, 5.0f, 0.0f), XMVectorSet(0.3f, 0.76f, 1.0f, 0.0f), 10.0f, Light::Type::POINT));
        /*m_lights.push_back(Light(XMVectorSet(100.0f, 100.0f, 0.0f, 0.0f), XMVectorSet(0.5, 0.87, 0.93, 0.0f), 0.25f, Light::Type::DIRECTIONAL));*/

        const UINT rowPitch = TextureWidth * TexturePixelSize;
        const UINT textureSize = rowPitch * TextureHeight;
        m_rtOutput = std::vector<UINT8>(textureSize);
        
        // Wait for the command list to execute; we are reusing the same command 
        // list in our main loop but for now, we just want to wait for setup to 
        // complete before continuing.
        WaitForPreviousFrame();
    }
}

// Generate a simple black and white checkerboard texture.
void Game::GenerateTextureData()
{
    const UINT rowPitch = TextureWidth * TexturePixelSize;
    const UINT textureSize = rowPitch * TextureHeight;

   
    UINT8* pData = &m_rtOutput[0];
    bool parallel = m_parallel > 0 ? true : false;
//omp_set_num_threads(omp_get_max_threads());
//#pragma omp parallel for schedule(dynamic, 50) num_threads(omp_get_max_threads()) if (parallel)
    for (INT p = 0; p < TextureWidth * TextureHeight; p++)
    {
        UINT i = (UINT)p / TextureHeight;
        UINT j = (UINT)p - i * TextureHeight;
        UINT n = rowPitch * j + i * TexturePixelSize;
      
        srand(12);
        XMVECTOR color = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
        for (int s = 0; s < m_samples; s++)
        {
            
            float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
            float u = (float(i) + r) / (static_cast<float>(TextureWidth) - 1.0f);
            float v = (float(j) + r)/ (static_cast<float>(TextureHeight) - 1.0f);
            XMVECTOR uv = XMVectorSet(u, v, 0.0f, 0.0f);
            
            // Barrel Distorsion
            if (m_enableBarrel)
            {
                XMVECTOR ndc;
                ndc = uv * 2.0f - XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);

                float theta = atan2(XMVectorGetY(ndc), XMVectorGetX(ndc));
                float radius = XMVectorGetX(XMVector2Length(ndc));
                radius = pow(radius, m_barrelPower);
                ndc = XMVectorSet(radius * cos(theta), radius * sin(theta), 0.0f, 0.0f);
                uv = XMVectorSet((XMVectorGetX(ndc) + 1.0) * 0.5f, (XMVectorGetY(ndc) + 1.0) * 0.5f, 0.0f, 0.0f);
            }
            

            Ray ray = m_camera->GetRayDirection(XMVectorGetX(uv), XMVectorGetY(uv));
           
            XMVECTOR result = ClosestHitShade(ray);
            XMVECTOR colorV = color + result;
            color = colorV;

            
        }
        pData[n] = min(1.0f, XMVectorGetX(color) / m_samples) * 255.0f ;
        pData[n + 1] = min(1.0f, XMVectorGetY(color) / m_samples) * 255.0f;
        pData[n + 2] = min(1.0f, XMVectorGetZ(color) / m_samples) * 255.0f;
        pData[n + 3] = 255;
        
        
    }

    

   
}


bool Game::Trace(Ray& ray,const Intersectable*& object)
{
    float closestHit = 1000000.0f;
    ray.t = 1000000000000.0f;
    /*
    for (int i = 0; i < m_geometry.size(); i++)
    {
        if (m_geometry[i]->Intersect(ray) && ray.t < closestHit )
        {
            object = m_geometry[i].get();
            closestHit = ray.t;
        }
        
    }
    ray.t = closestHit;
    return (object != nullptr);*/
    return m_BVH->Intersect(ray, object);
}

XMVECTOR Game::ClosestHitShade(Ray& ray)
{
    XMVECTOR color = XMVectorSet(0.6, 1.0f, 1.0f, 0.0f);
    XMVECTOR finalColor = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
    const Intersectable* object = nullptr;
    if (ray.depth > m_depth)
        return color;
    if (Trace(ray, object))
    {
       
        Surface surf;
        object->GetSurfaceData(surf, ray);
        XMVECTOR matColor = object->mat.color * ((m_texturing > 0) ? GetTexture(surf.tex) : XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f));
        XMVECTOR N = surf.normal;
        XMVECTOR P = surf.position;
        
        if (object->mat.type == Material::Type::DIFFUSE)
        {
            
            for (int i = 0; i < m_lights.size(); i++)
            {

                Light light = m_lights[i];
                XMVECTOR L = light.position - P;
                float distance = XMVectorGetX(XMVector3Length(L));
                L = XMVector3Normalize(L);
                distance = light.type == Light::Type::DIRECTIONAL ? 10e9 : distance;
                Ray shadowRay(surf.position, L, Ray::EPSILON, 0, distance, Ray::EPSILON);
               /* float shadowAttenuation = AnyHit(shadowRay) ? 0.0f : 1.0f;*/
                float shadowAttenuation = 1.0f;
                float totalAttenuation = light.CosineAttenuation(surf.position, surf.normal) * shadowAttenuation;
                if (light.type == Light::Type::POINT)
                    totalAttenuation *= light.Attenuate(surf.position);
                else if (light.type == Light::Type::SPOT)
                {
                    totalAttenuation *= (light.Attenuate(surf.position) * light.SpotAttenuation(surf.position));
                }
                finalColor += matColor * light.color * light.intensity *
                    totalAttenuation;
                color = finalColor;
            }
        }
            
            
        else if (object->mat.type == Material::Type::SPECULAR)
        {
            ray.ReflectRay(surf);
            color = ClosestHitShade(ray);
            finalColor = color * matColor;
            color = finalColor;
            
           
        }
        else if (object->mat.type == Material::Type::DIELECTRIC)
        {
            float ior = object->mat.I_IOR;
            float n1 = 1.0f;
            float n2 = object->mat.IOR;
            XMVECTOR dir = ray.direction;
            float cosThetaI = XMVectorGetX(XMVector3Dot(N, -dir));
            bool inside = false;
            if (cosThetaI < 0) // We are inside the object reverse the normal and IOR
            {
                inside = true;
                ior = object->mat.IOR;
                std::swap(n1, n2);
                surf.normal = -N;
                N = surf.normal;
                cosThetaI = -cosThetaI;
            }
            float ior2 = ior * ior;
            float cosThetaI2 = cosThetaI * cosThetaI;
            float k = 1.0f - ior2 * (1 - cosThetaI2);
            
            float Fr = 1.0f;
            if (k > 0)
            {
                float sinThetaCrit = ior * sqrtf(max(0.0f, 1 - cosThetaI2));
                float cosThetaCrit = sqrtf(max(0.0f, (1.0f - sinThetaCrit * sinThetaCrit)));

                float num1 = n1 * cosThetaI - n2 * cosThetaCrit;
                float num2 = n1 * cosThetaCrit - n2 * cosThetaI;
                float denom1 = n1 * cosThetaI + n2 * cosThetaCrit;
                float denom2 = n1 * cosThetaCrit + n2 * cosThetaI;
                float num12 = num1 * num1;
                float num22 = num2 * num2;
                float denom12 = denom1 * denom1;
                float denom22 = denom2 * denom2;
                Fr = 0.5f * (num12 / denom12 + num22 / denom22);
            }
            
            float Ft = 1.0f - Fr;
          
            XMVECTOR reflectColor = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
            Ray reflRay = ray;
            reflRay.ReflectRay(surf);
           
            color = ClosestHitShade(reflRay);
            reflectColor += Fr * color * matColor;
            if (inside) reflectColor = XMVectorMultiply(reflectColor, XMVectorExpE(-reflRay.t * object->mat.extinction));
            XMVECTOR refractedColor = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
            if (Ft > 0)
            { 
               
                Ray refrRay = ray;
                refrRay.RefractRay(surf, cosThetaI, ior, k, !inside);
                XMVECTOR origin = P - Ray::EPSILON * N;
                refrRay.origin = origin;
                refrRay.t = 0.0f;
                color = ClosestHitShade(refrRay);
                refractedColor += Ft * color * matColor;
                float distance = refrRay.t;
                if(!inside) refractedColor = XMVectorMultiply(refractedColor, XMVectorExpE(-distance * object->mat.extinction));
                

            }

            color = reflectColor + refractedColor;
           
           
        }
       
            
    }
    else
    {
        ray.t = 0.0f;
    }
    return color;
}

bool Game::AnyHit(Ray& ray)
{
    for (int i = 0; i < m_geometry.size(); i++)
    {
        if (m_geometry[i]->Intersect(ray) && ray.t > ray.tMin && ray.t <= ray.tMax)
        {
            return true;
        }

    }
    return false;
    

}
XMVECTOR Game::GetTexture(XMVECTOR& tex)
{
    float size = 8;
    UINT x = (UINT)(XMVectorGetX(tex) * size);
    UINT y = (UINT)(XMVectorGetY(tex) * size);
    if (x % 2 == y % 2)
        return XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f);

    return XMVectorSet(0.5f, 0.5f, 0.5f, 0.0f);
}
// Update frame-based values.
void Game::OnUpdate()
{
    m_timer.Tick();
    CalculateFrameStats();
    float delta = static_cast<float>(m_timer.GetElapsedSeconds());
    m_camera->MoveCamera(m_cameraMovementX, m_cameraMovementZ, delta);
    m_camera->ModifyFOV(delta, m_fovState);
    m_barrelPower += m_barrelState * delta;
    ThrowIfFailed(m_commandAllocator->Reset());
    ThrowIfFailed(m_commandList->Reset(m_commandAllocator.Get(), m_pipelineState.Get()));
   
    GenerateTextureData();
   
    D3D12_SUBRESOURCE_DATA textureData = {};
    textureData.pData = &m_rtOutput[0];
    textureData.RowPitch = TextureWidth * TexturePixelSize;
    textureData.SlicePitch = textureData.RowPitch * TextureHeight;

    UpdateSubresources(m_commandList.Get(), m_texture.Get(), m_textureUploadHeap.Get(), 0, 0, 1, &textureData);
    m_commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_texture.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));

}
void Game::CalculateFrameStats()
{
   
    
    double time = m_timer.GetElapsedSeconds();
    float timeMsec = 1000 * time;
  
    std::wstringstream windowText;
    windowText << std::setprecision(2) << std::fixed
        << L"    Time(msec): " << timeMsec;
    SetCustomWindowText(windowText.str().c_str());
    
}

// Render the scene.
void Game::OnRender()
{
    // Record all the commands we need to render the scene into the command list.
    PopulateCommandList();

    // Execute the command list.
    ID3D12CommandList* ppCommandLists[] = { m_commandList.Get() };
    m_commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

    // Present the frame.
    ThrowIfFailed(m_swapChain->Present(0, 0));

    WaitForPreviousFrame();
}

void Game::OnDestroy()
{
    // Ensure that the GPU is no longer referencing resources that are about to be
    // cleaned up by the destructor.
    WaitForPreviousFrame();

    CloseHandle(m_fenceEvent);
}

void Game::OnKeyDown(UINT8 key)
{
    switch (key)
    {
    case 'W':
        m_cameraMovementZ = -m_camera->viewDirection;
        break;
    case 'S' :
        m_cameraMovementZ = m_camera->viewDirection;
        break;
    case 'A':
        m_cameraMovementX = -m_camera->right;
        break;
    case 'D':
        m_cameraMovementX = m_camera->right;
        break;
    case VK_UP:
        m_fovState = -1;
        break;
    case VK_DOWN:
        m_fovState = 1;
        break;
    case 'B':
        m_barrelState = 1.0f;
        m_barrelState = m_enableBarrel > 0 ? m_barrelState : 0.0f;
        break;
    case 'N':
        m_barrelState = -1.0;
        m_barrelState = m_enableBarrel > 0 ? m_barrelState : 0.0f;
        break;
    }
}



void Game::OnKeyUp(UINT8 key)
{
    switch (key)
    {
    case 'W':
        m_cameraMovementZ = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
        break;
    case 'S':
        m_cameraMovementZ = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
        break;
    case 'A':
        m_cameraMovementX = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
        break;
    case 'D':
        m_cameraMovementX = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
        break;
    case VK_UP:
        m_fovState = 0;
        break;
    case VK_DOWN:
        m_fovState = 0;
        break;
    case VK_LEFT:
        m_depth = max(1, m_depth - 1);
        break;
    case VK_RIGHT:
        m_depth = min(16, m_depth + 1);
        break;
    case 'B':
        m_barrelState = 0.0f;
        break;
    case 'N':
        m_barrelState = 0.0;
        break;
    case 'P':
        m_parallel *= -1.0f;
        break;
    case 'O':
        m_enableBarrel *= -1.0f;
        break;
    case 'T':
        m_texturing *= -1.0f;
    case 'Z':
        m_samples = min(m_samples + 1, 16);
        break;
    case 'X':
        m_samples = max(m_samples - 1, 1);
        break;
    }
}

void Game::OnMouseMove(UINT x, UINT y)
{
    if (m_firstClick)
    {
        m_Xprev = (float)x;
        m_Yprev = (float)y;
        m_firstClick = false;
    }
    float xOffset = m_Xprev - (float)x;
    float yOffset = (float)y - m_Yprev;
    m_Xprev = (float)x;
    m_Yprev = (float)y;
    m_camera->RotateCamera(xOffset, yOffset);
}

void Game::OnMouseUp()
{
    ReleaseCapture();
    m_firstClick = true;
}

void Game::OnMouseDown()
{
    SetCapture(Win32Application::GetHwnd());
}

void Game::PopulateCommandList()
{
    

    // Set necessary state.
    m_commandList->SetGraphicsRootSignature(m_rootSignature.Get());

    ID3D12DescriptorHeap* ppHeaps[] = { m_srvHeap.Get() };
    m_commandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

    m_commandList->SetGraphicsRootDescriptorTable(0, m_srvHeap->GetGPUDescriptorHandleForHeapStart());
    m_commandList->RSSetViewports(1, &m_viewport);
    m_commandList->RSSetScissorRects(1, &m_scissorRect);

    // Indicate that the back buffer will be used as a render target.
    m_commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_renderTargets[m_frameIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart(), m_frameIndex, m_rtvDescriptorSize);
    m_commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

    // Record commands.
    const float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
    m_commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
    m_commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    m_commandList->IASetVertexBuffers(0, 1, &m_vertexBufferView);
    m_commandList->DrawInstanced(3, 1, 0, 0);

    // Indicate that the back buffer will now be used to present.
    m_commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_renderTargets[m_frameIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));
    m_commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_texture.Get(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_COPY_DEST));

    ThrowIfFailed(m_commandList->Close());
}

void Game::WaitForPreviousFrame()
{
    // WAITING FOR THE FRAME TO COMPLETE BEFORE CONTINUING IS NOT BEST PRACTICE.
    // This is code implemented as such for simplicity. The D3D12HelloFrameBuffering
    // sample illustrates how to use fences for efficient resource usage and to
    // maximize GPU utilization.

    // Signal and increment the fence value.
    const UINT64 fence = m_fenceValue;
    ThrowIfFailed(m_commandQueue->Signal(m_fence.Get(), fence));
    m_fenceValue++;

    // Wait until the previous frame is finished.
    if (m_fence->GetCompletedValue() < fence)
    {
        ThrowIfFailed(m_fence->SetEventOnCompletion(fence, m_fenceEvent));
        WaitForSingleObject(m_fenceEvent, INFINITE);
    }

    m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();
}
