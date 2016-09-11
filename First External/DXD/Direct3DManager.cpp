#include "Direct3DManager.h"
Direct3DManager* DXManager = new Direct3DManager();

Direct3DManager::Direct3DManager()
{

}

Direct3DManager::~Direct3DManager()
{

}

int Direct3DManager::Direct3DInit(HWND hWnd)
{
	if (FAILED(Direct3DCreate9Ex(D3D_SDK_VERSION, &p_Object)))
		exit(1);

	ZeroMemory(&p_Params, sizeof(p_Params));
	p_Params.Windowed = TRUE;
	p_Params.SwapEffect = D3DSWAPEFFECT_DISCARD;
	p_Params.hDeviceWindow = hWnd;
	p_Params.MultiSampleQuality = D3DMULTISAMPLE_NONE;
	p_Params.BackBufferFormat = D3DFMT_A8R8G8B8;
	p_Params.BackBufferWidth = Width;
	p_Params.BackBufferHeight = Height;
	p_Params.EnableAutoDepthStencil = TRUE;
	p_Params.AutoDepthStencilFormat = D3DFMT_D16;

	if (FAILED(p_Object->CreateDeviceEx(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &p_Params, nullptr, &p_Device)))
		exit(1);

	if (!p_Line)
		D3DXCreateLine(p_Device, &p_Line);

	D3DXCreateFont(p_Device, 18, 0, 0, 0, false, DEFAULT_CHARSET, OUT_CHARACTER_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, "Calibri", &p_FontSmall);

	return 0;
}



ID3DXLine* Direct3DManager::GetD3DLine()
{
	return p_Line;
}

IDirect3DDevice9Ex* Direct3DManager::GetDevice()
{
	return p_Device;
}

ID3DXFont* Direct3DManager::GetFont()
{
	return p_FontSmall;
}