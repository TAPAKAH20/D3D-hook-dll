// dllmain.cpp : Определяет точку входа для приложения DLL.
#include "pch.h"
#include "hooker.h"

Hooker d3dhooker;

// hook destination for function for D3D11CreateDeviceAndSwapChain
HRESULT swapChainHook(IDXGIAdapter* pAdapter,
    D3D_DRIVER_TYPE            DriverType,
    HMODULE                    Software,
    UINT                       Flags,
    const D3D_FEATURE_LEVEL* pFeatureLevels,
    UINT                       FeatureLevels,
    UINT                       SDKVersion,
    const DXGI_SWAP_CHAIN_DESC* pSwapChainDesc,
    IDXGISwapChain** ppSwapChain,
    ID3D11Device** ppDevice,
    D3D_FEATURE_LEVEL* pFeatureLevel,
    ID3D11DeviceContext** ppImmediateContext
) {

    MessageBox(NULL, _T("Succsess"), _T("Hook succsessful "), MB_OK);
    d3dhooker.unhook();
    HRESULT retVal =  D3D11CreateDeviceAndSwapChain(pAdapter, DriverType, Software, Flags, pFeatureLevels, FeatureLevels, SDKVersion, pSwapChainDesc, ppSwapChain, ppDevice, pFeatureLevel, ppImmediateContext);
    d3dhooker.rehook();

    return retVal;
}


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        new Hooker(d3dhooker);
        if (d3dhooker.hook(L"D3D11.dll", "D3D11CreateDeviceAndSwapChain", swapChainHook) != 0) {
            return FALSE;
        }
        break;
    case DLL_PROCESS_DETACH:
        d3dhooker.unhook();
        break;
    }
    return TRUE;
    
}
