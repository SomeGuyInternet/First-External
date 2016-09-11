#pragma once

#define WIN32_LEAN_AND_MEAN

#include "../Includes.h"

class Direct3DManager
{
private:
	IDirect3D9Ex* p_Object;
	IDirect3DDevice9Ex* p_Device;
	D3DPRESENT_PARAMETERS p_Params;
	ID3DXLine* p_Line;
	ID3DXFont* p_FontSmall;

public:
	Direct3DManager();
	~Direct3DManager();

	int Direct3DInit(HWND hWnd);
	int Render();

public:
	ID3DXLine* GetD3DLine();
	IDirect3DDevice9Ex* GetDevice();
	ID3DXFont* GetFont();
};

extern Direct3DManager* DXManager;