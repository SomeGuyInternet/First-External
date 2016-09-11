/*
Credtis: SOmeGuyInternet, Zat, Grab, synthfx
*/

#include "Includes.h"
#include <sstream>
#include <iomanip>
#include <math.h>

#define SMALL_NUM   0.00000001
#define dot(u,v)   ((u).x * (v).x + (u).y * (v).y + (u).z * (v).z)
#define norm(v)    sqrt(dot(v,v))
#define d(u,v)     norm(u-v)        
#define abs(x)     ((x) >= 0 ? (x) : -(x))

DWORD ClientBase;
DWORD EngineBase;

DWORD LocalPlayer;
DWORD EntityPlayer;

bool bRadar = false,
	bTrigbot = false,
	bBhop = false,
	bdist = false,
	bLine = false,
	bhealth = false,
	bBox = false;

SlimUtils::SlimMem mem;

int Width = 1920;
int Height = 1080;

const MARGINS Margin = { 0, 0, Width, Height };

char lWindowName[256] = "Overlay";
HWND hWnd;

char tWindowName[256] = "Counter-Strike: Global Offensive";
HWND tWnd;
RECT tSize;

DWORD pid;
HANDLE pHandle;

MSG Message;
void SetWindowToTarget();

Playerstr strLocalPlayer;
EntPlayer Ent;

struct matrix3x4_t
{
	matrix3x4_t() {}
	matrix3x4_t(
		float m00, float m01, float m02, float m03,
		float m10, float m11, float m12, float m13,
		float m20, float m21, float m22, float m23)
	{
		m_flMatVal[0][0] = m00;	m_flMatVal[0][1] = m01; m_flMatVal[0][2] = m02; m_flMatVal[0][3] = m03;
		m_flMatVal[1][0] = m10;	m_flMatVal[1][1] = m11; m_flMatVal[1][2] = m12; m_flMatVal[1][3] = m13;
		m_flMatVal[2][0] = m20;	m_flMatVal[2][1] = m21; m_flMatVal[2][2] = m22; m_flMatVal[2][3] = m23;
	}
	void Init(const Vector& xAxis, const Vector& yAxis, const Vector& zAxis, const Vector &vecOrigin)
	{
		m_flMatVal[0][0] = xAxis.x; m_flMatVal[0][1] = yAxis.x; m_flMatVal[0][2] = zAxis.x; m_flMatVal[0][3] = vecOrigin.x;
		m_flMatVal[1][0] = xAxis.y; m_flMatVal[1][1] = yAxis.y; m_flMatVal[1][2] = zAxis.y; m_flMatVal[1][3] = vecOrigin.y;
		m_flMatVal[2][0] = xAxis.z; m_flMatVal[2][1] = yAxis.z; m_flMatVal[2][2] = zAxis.z; m_flMatVal[2][3] = vecOrigin.z;
	}

	matrix3x4_t(const Vector& xAxis, const Vector& yAxis, const Vector& zAxis, const Vector &vecOrigin)
	{
		Init(xAxis, yAxis, zAxis, vecOrigin);
	}
	float *operator[](int i)
	{
		return m_flMatVal[i];
	}
	const float *operator[](int i) const
	{
		return m_flMatVal[i];
	}
	float *Base()
	{
		return &m_flMatVal[0][0];
	}
	const float *Base() const
	{
		return &m_flMatVal[0][0];
	}

	float m_flMatVal[3][4];
};

Vector EntPostion;

void RPM()
{
	std::cout << "RPM Thread Joined" << std::endl;

	while (true)
	{
		strLocalPlayer.iHealth = mem.Read<int>(LocalPlayer + m_iHealth);
		strLocalPlayer.iTeam = mem.Read<int>(LocalPlayer + m_iTeamNum);
		strLocalPlayer.iClip = mem.Read<int>(LocalPlayer + m_iClip1);
		strLocalPlayer.iWepID = mem.Read<int>(LocalPlayer + m_iWeaponID);
		strLocalPlayer.iCrossID = mem.Read<int>(LocalPlayer + m_iCrossHairID);

		strLocalPlayer.bLife = mem.Read<bool>(LocalPlayer + m_lifeState);
		strLocalPlayer.bSpawned = mem.Read<bool>(LocalPlayer + m_bGunGameImmunity);
		strLocalPlayer.bDormant = mem.Read<bool>(LocalPlayer + m_bDormant);

		strLocalPlayer.pos = mem.Read<Vector>(LocalPlayer + 0x134);

		Ent.list = mem.Read<DWORD>(*(DWORD*)ClientBase + m_dwEntityList);
		Ent.EnemyCross = mem.Read<DWORD>(*(DWORD*)ClientBase + m_dwEntityList + ((strLocalPlayer.iCrossID - 1) * 0x10));
		Ent.EnemyHealth = mem.Read<int>(Ent.EnemyCross + m_iHealth);
		Ent.EnemyTeam = mem.Read<int>(Ent.EnemyCross + m_iTeamNum);

		Sleep(100);
	}
}

void bhop()
{
	std::cout << "BHOP Thread Joined" << std::endl;
	bool ShouldJump = false;

	while (true)
	{
		if (GetAsyncKeyState(VK_SPACE) & 0x8000 && bBhop)
		{
			float flag = mem.Read<int>(LocalPlayer + m_fFlags);

			if (ShouldJump) {
				if (!(flag == 257)) {
					mem.Write<int>(*(DWORD*)ClientBase + m_dwForceJump, 0);
					ShouldJump = false;
				}
			}
			else if (flag == 257) {
				mem.Write<int>(*(DWORD*)ClientBase + m_dwForceJump, 1);
				ShouldJump = true;
			}
		}
		Sleep(10);
	}
}

void TriggerBot()
{
	std::cout << "Triggerbot Thread Joined" << std::endl;

	while (true)
	{
		if (bTrigbot)
		{
			if (strLocalPlayer.iTeam != Ent.EnemyTeam && Ent.EnemyHealth > 0)
			{
				// Here you can add a delay before shooting, to make it look legit. This is done using Sleep()
				mouse_event(MOUSEEVENTF_LEFTDOWN, NULL, NULL, NULL, NULL);
				// use Sleep() here for shooting several shots with an ak for example. Not usable with pisto
				mouse_event(MOUSEEVENTF_LEFTUP, NULL, NULL, NULL, NULL);
				// use Sleep() here for a 'cooldown' between shots.
			}
		}

		Sleep(100);
	}
}

void Radar()
{
	std::cout << "Radar Thread Joined" << std::endl;

	while (true)
	{
		if (bRadar)
		{
			DWORD dwEntity;

			for (int i = 0; i < 64; i++)
			{
				dwEntity = mem.Read<DWORD>(*(DWORD*)ClientBase + m_dwEntityList + ((i - 1) * 16));

				if (dwEntity == NULL)
					continue;

				bool isspotted = mem.Read<bool>(dwEntity + 0x939);

				if (isspotted)
					continue;

				bool IsDormant = mem.Read<bool>(dwEntity + 0xE9);

				if (IsDormant)
					continue;

				mem.Write(dwEntity + 0x939, 1);

			}
		}
	}
}

void ToggleKeys()
{
	while (true)
	{
		if (GetAsyncKeyState(VK_F1) & 1)
		{
			bBhop = !bBhop;
			std::string stat = bBhop ? "On" : "False";
			std::cout << "Bhop is " << stat << std::endl;
		}

		if (GetAsyncKeyState(VK_F2) & 1)
		{
			bTrigbot = !bTrigbot;
			std::string stat = bTrigbot ? "On" : "False";
			std::cout << "Triggerbot is " << stat << std::endl;
		}

		if (GetAsyncKeyState(VK_F3) & 1)
		{
			bRadar = !bRadar;
			std::string stat = bRadar ? "On" : "False";
			std::cout << "Radar is " << stat << std::endl;
		}

		if (GetAsyncKeyState(VK_F4) & 1)
		{
			bLine = !bLine;
			std::string stat = bLine ? "On" : "False";
			std::cout << "Line ESP is " << stat << std::endl;
		}

		if (GetAsyncKeyState(VK_F5) & 1)
		{
			bdist = !bdist;
			std::string stat = bdist ? "On" : "False";
			std::cout << "Distance ESP is " << stat << std::endl;
		}

		if (GetAsyncKeyState(VK_F6) & 1)
		{
			bhealth = !bhealth;
			std::string stat = bhealth ? "On" : "False";
			std::cout << "Health ESP is " << stat << std::endl;
		}

		if (GetAsyncKeyState(VK_F7))
		{
			bBox = !bBox;
			std::string stat = bBox ? "On" : "False";
			std::cout << "Box ESP is " << stat << std::endl;
		}

		Sleep(500);
	}
}

LRESULT CALLBACK WinProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	switch (Message)
	{
	case WM_PAINT:
		DXManager->Render();
		break;

	case WM_CREATE:
		DwmExtendFrameIntoClientArea(hWnd, &Margin);
		break;

	case WM_DESTROY:
		PostQuitMessage(1);
		return 0;

	default:
		return DefWindowProc(hWnd, Message, wParam, lParam);
	}
	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hSecInstance, LPSTR nCmdLine, INT nCmdShow)
{
	AllocConsole();
	freopen("CONOUT$", "w", stdout);

	DWORD pid;

	std::cout << "Waiting for process..." << std::endl;
	while (!SlimUtils::SlimMem::GetPID(L"csgo.exe", &pid))
		Sleep(500);

	if (!mem.Open(pid, SlimUtils::ProcessAccess::Full))
	{
		std::cout << "Failed to open process, error-code: " << GetLastError() << std::endl;
		return 0;
	}

	auto ClientDLL = mem.GetModule(L"client.dll");
	auto EngineDLL = mem.GetModule(L"engine.dll");

	if (ClientDLL != nullptr && EngineDLL != nullptr)
	{
		ClientBase = (DWORD)ClientDLL;
		EngineBase = (DWORD)EngineDLL;

		LocalPlayer = mem.Read<DWORD>(*(DWORD*)ClientBase + m_dwLocalPlayer);

		std::cout << "Client: " << ClientBase << std::endl << "Engine: " << EngineBase << std::endl << "Local Player: " << LocalPlayer << std::endl;
		
		std::thread RPMThread(RPM);
		std::thread BhopThread(bhop);
		std::thread TriggerBotThread(TriggerBot);
		std::thread RadarThread(Radar);
		std::thread ToggleKeysThread(ToggleKeys);

		CreateThread(nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(SetWindowToTarget), nullptr, 0, nullptr);

		WNDCLASSEX wClass;
		wClass.cbClsExtra = NULL;
		wClass.cbSize = sizeof(WNDCLASSEX);
		wClass.cbWndExtra = NULL;
		wClass.hbrBackground = static_cast<HBRUSH>(CreateSolidBrush(RGB(0, 0, 0)));
		wClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
		wClass.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
		wClass.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);
		wClass.hInstance = hInstance;
		wClass.lpfnWndProc = WinProc;
		wClass.lpszClassName = lWindowName;
		wClass.lpszMenuName = lWindowName;
		wClass.style = CS_VREDRAW | CS_HREDRAW;

		if (!RegisterClassEx(&wClass))
		{
			exit(1);
		}

		tWnd = FindWindow(nullptr, tWindowName);
		if (tWnd)
		{
			GetWindowRect(tWnd, &tSize);
			Width = tSize.right - tSize.left;
			Height = tSize.bottom - tSize.top;
			hWnd = CreateWindowEx(WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_LAYERED, lWindowName, lWindowName, WS_POPUP, 1, 1, Width, Height, nullptr, nullptr, nullptr, nullptr);
			SetLayeredWindowAttributes(hWnd, 0, 1.0f, LWA_ALPHA);
			SetLayeredWindowAttributes(hWnd, 0, RGB(0, 0, 0), LWA_COLORKEY);
			ShowWindow(hWnd, SW_SHOW);

			SetForegroundWindow(tWnd);
		}
		
		std::cout << std::endl << std::endl << "Credits to S0meGuyInternet, Zat, Grab, synthfx (and some others i missed so Whole UC)" << std::endl;
		std::cout << "How to use: " << std::endl;
		std::cout << "F1 BHOP Togggle" << std::endl;
		std::cout << "F2 TriggerBot Toggle" << std::endl;
		std::cout << "F3 Radar Toggle" << std::endl;
		std::cout << "F4 Line ESP Toggle" << std::endl;
		std::cout << "F5 Distance ESP Toggle" << std::endl;
		std::cout << "F6 Health ESP Toggle" << std::endl;
		std::cout << "F7 Box ESP Toggle" << std::endl;
		std::cout << "Disclamer this is my first external. Ill most likley update it with more features later on!" << std::endl;

		DXManager->Direct3DInit(hWnd);


		for (;;)
		{
			if (PeekMessage(&Message, hWnd, 0, 0, PM_REMOVE))
			{
				DispatchMessage(&Message);
				TranslateMessage(&Message);
			}
			Sleep(1);
		}

		if (RPMThread.joinable())
		{
			RPMThread.join();
		}

		if (BhopThread.joinable())
		{
			BhopThread.join();
		}

		if (TriggerBotThread.joinable())
		{
			TriggerBotThread.join();
		}

		if (RadarThread.joinable())
		{
			RadarThread.join();
		}

		if (ToggleKeysThread.joinable())
		{
			ToggleKeysThread.join();
		}

		
	}
}

void SetWindowToTarget()
{
	while (true)
	{
		tWnd = FindWindow(nullptr, tWindowName);

		if (tWnd)
		{
			GetWindowThreadProcessId(tWnd, &pid);
			pHandle = OpenProcess(PROCESS_ALL_ACCESS, false, pid);

			GetWindowRect(tWnd, &tSize);
			Width = tSize.right - tSize.left;
			Height = tSize.bottom - tSize.top;
			DWORD dwStyle = GetWindowLong(tWnd, GWL_STYLE);
			if (dwStyle & WS_BORDER)
			{
				tSize.top += 23;
				Height -= 23;
			}
			MoveWindow(hWnd, tSize.left, tSize.top, Width, Height, true);
		}
		else
		{
			char ErrorMsg[125];
			sprintf_s(ErrorMsg, "Make sure %s is running!", tWindowName);
			MessageBox(nullptr, ErrorMsg, "Error - Cannot find the game!", MB_OK | MB_ICONERROR);
			exit(1);
		}
		Sleep(100);
	}
}

struct Bones {
	matrix3x4_t bones[128];
};

Vector getBonePosition(DWORD p, int boneId) {
	Bones bones = mem.Read<Bones>(p + 0x2698);

	Vector pos;
	pos.x = bones.bones[boneId][0][3];
	pos.y = bones.bones[boneId][1][3];
	pos.z = bones.bones[boneId][2][3];
	return pos;
}

static std::string to_string_with_precision(float val)
{
	std::ostringstream out;
	out << std::setprecision(4) << val;
	return out.str();
}

bool ScreenTransform(const Vector& point, Vector& screen, D3DXMATRIX worldToScreen, int width, int height)
{
	screen.x = worldToScreen.m[0][0] * point.x + worldToScreen.m[0][1] * point.y + worldToScreen.m[0][2] * point.z + worldToScreen.m[0][3];
	screen.y = worldToScreen.m[1][0] * point.x + worldToScreen.m[1][1] * point.y + worldToScreen.m[1][2] * point.z + worldToScreen.m[1][3];
	//screen.z = worldToScreen.m[2][0] * point.x + worldToScreen.m[2][1] * point.y + worldToScreen.m[2][2] * point.z + worldToScreen.m[2][3];
	float w = worldToScreen.m[3][0] * point.x + worldToScreen.m[3][1] * point.y + worldToScreen.m[3][2] * point.z + worldToScreen.m[3][3];

	screen.x /= w;
	screen.y /= w;
	//screen.z /= w;

	if (w < 0.001f)
		return false;
	else
	{
		float fScreenX = width / 2;
		float fScreenY = height / 2;

		fScreenX += 0.5f * screen.x * width + 0.5f;
		fScreenY -= 0.5f * screen.y * height + 0.5f;

		screen.x = fScreenX;
		screen.y = fScreenY;

		return true;
	}
}

Vector MatMul(Vector& in, matrix3x4_t& boneMat)
{
	Vector out;
	out.x = boneMat.m_flMatVal[0][0] * in.x + boneMat.m_flMatVal[0][1] * in.y + boneMat.m_flMatVal[0][2] * in.z + boneMat.m_flMatVal[0][3];
	out.y = boneMat.m_flMatVal[1][0] * in.x + boneMat.m_flMatVal[1][1] * in.y + boneMat.m_flMatVal[1][2] * in.z + boneMat.m_flMatVal[1][3];
	out.z = boneMat.m_flMatVal[2][0] * in.x + boneMat.m_flMatVal[2][1] * in.y + boneMat.m_flMatVal[2][2] * in.z + boneMat.m_flMatVal[2][3];

	return out;
}

int Direct3DManager::Render()
{
	p_Device->Clear(0, nullptr, D3DCLEAR_TARGET, 0, 1.0f, 0);
	p_Device->BeginScene();

	if (tWnd == GetForegroundWindow())
	{
		DXDraw->DrawOutlinedFontText(10, 10, 255, 255, 255, DXManager->GetFont(), "Credtits: SomeGuyInternet, Zat, Grab, synthfx");

		DWORD dwEntity;
		int r, g, b;

		if (bLine || bdist || bhealth || bBox)
		{
			#pragma region ESP
			for (int i = 0; i < 32; i++)
			{
				dwEntity = mem.Read<DWORD>(*(DWORD*)ClientBase + m_dwEntityList + ((i - 1) * 16));

				if (dwEntity == NULL)
					continue;

				if (dwEntity == LocalPlayer)
					continue;

				bool IsDormant = mem.Read<bool>(dwEntity + 0xE9);

				if (IsDormant)
					continue;

				int Health = mem.Read<int>(dwEntity + m_iHealth);
				int entteam = mem.Read<int>(dwEntity + m_iTeamNum);

				if (entteam == 2)
				{
					r = 255;
					g = 0;
					b = 0;
				}
				else {
					r = 0;
					g = 0;
					b = 255;
				}

				auto transformMat = mem.Read<matrix3x4_t>(dwEntity + 0x440);
				auto min = mem.Read<Vector>(dwEntity + (0x318 + 0x8));
				auto max = mem.Read<Vector>(dwEntity + (0x318 + 0x14));
				D3DXMATRIX& mat = mem.Read<D3DXMATRIX>(*(DWORD*)ClientBase + 0x4A49A44);

				Vector points[] = {
					Vector(min.x, min.y, min.z),
					Vector(min.x, max.y, min.z),
					Vector(max.x, max.y, min.z),
					Vector(max.x, min.y, min.z),
					Vector(max.x, max.y, max.z),
					Vector(min.x, max.y, max.z),
					Vector(min.x, min.y, max.z),
					Vector(max.x, min.y, max.z),
					Vector(max.x, min.y, max.z + 10),
					Vector(max.x, min.y, max.z - 5),
				};

				Vector transformedMat[10];
				for (int k = 0; k < 10; k++)
					transformedMat[k] = MatMul(points[k], transformMat);

				Vector transformedScreen[10];
				for (int k = 0; k < 10; k++)
					if (!ScreenTransform(transformedMat[k], transformedScreen[k], mat, 1920, 1080))
						continue;

				float left = transformedScreen[0].x;
				float right = transformedScreen[0].x;
				float top = transformedScreen[0].y;
				float bottom = transformedScreen[0].y;

				for (int i = 1; i < 8; i++)
				{
					if (left > transformedScreen[i].x)
						left = transformedScreen[i].x;
					if (top < transformedScreen[i].y)
						top = transformedScreen[i].y;
					if (right < transformedScreen[i].x)
						right = transformedScreen[i].x;
					if (bottom > transformedScreen[i].y)
						bottom = transformedScreen[i].y;
				}

				int off = 0;
				if (Health > 0)
				{
					//Distance ESP
					if (bdist)
					{
						float Dist = (EntPostion - strLocalPlayer.pos).length() * 0.01905f;

						char distbuff[50];
						sprintf(distbuff, "%.1f m", Dist);

						DXDraw->DrawString(distbuff, right + 5, bottom + off, r, g, b, DXManager->GetFont());

						off += 15;
					}

					//Health ESP
					if (bhealth)
					{
						char healthbuff[50];
						sprintf(healthbuff, "HP: %d", Health);


						DXDraw->DrawString(healthbuff, right + 5, bottom + off, r, g, b, DXManager->GetFont());

						off += 15;
					}

					//Box ESP
					if(bBox)
						DXDraw->DrawBox(left, bottom, abs(left - right), abs(top - bottom), 1, r, g, b, 255);

					//Line ESP
					if (bLine)
						DXDraw->DrawLine(1920 / 2, 1080, right / 2, bottom / 2, r, g, b, 255);

				}
			}
			#pragma endregion
		}
	}

	p_Device->EndScene();
	p_Device->PresentEx(nullptr, nullptr, nullptr, nullptr, 0);
	return 0;
}