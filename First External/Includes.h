#pragma once
#include <Windows.h>
#include <iostream>
#include <stdio.h>
#include <TlHelp32.h>
#include <fstream>
#include <thread>

#include <d3d9.h>
#include <d3dx9.h>
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

#include <dwmapi.h>
#pragma comment(lib, "dwmapi.lib")

#include "DXD/Direct3DManager.h"
#include "DXD/DXDraw.h"
#include "Math.h"

#include "SlimMem.h"
#include "Offsets.h"
#include "Vector.h"
#include "Player.h"

extern int Width;
extern int Height;
extern char lWindowName[256];
extern HWND hWnd;
extern char tWindowName[256];
extern HWND tWnd;
extern RECT tSize;
extern MSG Message;
extern bool Debug_Border;
extern HANDLE pHandle;
extern DWORD pid;
extern MSG Message;

