#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <Windows.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <D3Dcompiler.h>
#include <DirectXMath.h>
#include "d3dx12.h"

#include <string>
#include <sstream>
#include <iomanip>
#include <omp.h>
#include <iostream>
#include <vector>
#include <memory>
#include <wrl.h>
#include <shellapi.h>

#include "StepTimer.h"
#include "Camera.h"
#include "Surface.h"
#include "Ray.h"
#include "Intersectable.h"
#include "Material.h"

#include "Sphere.h"



