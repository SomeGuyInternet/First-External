#include "DXDraw.h"
DirectDraw* DXDraw = new DirectDraw();

DirectDraw::DirectDraw()
{

}

DirectDraw::~DirectDraw()
{

}

void DirectDraw::GradientFunc(int x, int y, int w, int h, int r, int g, int b, int a)
{
	int iColorR, iColorG, iColorB;
	for (int i = 1; i < h; i++)
	{
		iColorR = (int)((float)i / h*r);
		iColorG = (int)((float)i / h*g);
		iColorB = (int)((float)i / h*b);
		FillRGB(x, y + i, w, 1, r - iColorR, g - iColorG, b - iColorB, a);
	}
}

void DirectDraw::DrawCenterLine(float x, float y, int width, int r, int g, int b)
{
		D3DXVECTOR2 dPoints[2];
		dPoints[0] = D3DXVECTOR2(x, y);
		dPoints[1] = D3DXVECTOR2(Width / 2, Height);
		DXManager->GetD3DLine()->SetWidth(width);
		DXManager->GetD3DLine()->Draw(dPoints, 2, D3DCOLOR_RGBA(r, g, b, 255));
}

void DirectDraw::DrawLine(float x, float y, float xx, float yy, int r, int g, int b, int a)
{
	D3DXVECTOR2 dLine[2];

	DXManager->GetD3DLine()->SetWidth(1);

	dLine[0].x = x;
	dLine[0].y = y;

	dLine[1].x = xx;
	dLine[1].y = yy;

	DXManager->GetD3DLine()->Draw(dLine, 2, D3DCOLOR_ARGB(a, r, g, b));
}

void DirectDraw::FillRGB(float x, float y, float w, float h, int r, int g, int b, int a)
{
	D3DXVECTOR2 vLine[2];

	DXManager->GetD3DLine()->SetWidth(w);

	vLine[0].x = x + w / 2;
	vLine[0].y = y;
	vLine[1].x = x + w / 2;
	vLine[1].y = y + h;

	DXManager->GetD3DLine()->Begin();
	DXManager->GetD3DLine()->Draw(vLine, 2, D3DCOLOR_RGBA(r, g, b, a));
	DXManager->GetD3DLine()->End();
}

void DirectDraw::DrawBox(float x, float y, float width, float height, float px, int r, int g, int b, int a)
{
	D3DXVECTOR2 points[5];
	points[0] = D3DXVECTOR2(x, y);
	points[1] = D3DXVECTOR2(x + width, y);
	points[2] = D3DXVECTOR2(x + width, y + height);
	points[3] = D3DXVECTOR2(x, y + height);
	points[4] = D3DXVECTOR2(x, y);
	DXManager->GetD3DLine()->SetWidth(1);
	DXManager->GetD3DLine()->Draw(points, 5, D3DCOLOR_RGBA(r, g, b, a));
}

void DirectDraw::DrawGUIBox(float x, float y, float w, float h, int r, int g, int b, int a, int rr, int gg, int bb, int aa)
{
	DrawBox(x, y, w, h, 1, r, g, b, a);
	FillRGB(x, y, w, h, rr, gg, bb, a);
}

void DirectDraw::DrawHealthBar(float x, float y, float w, float h, int r, int g)
{
	FillRGB(x, y, w, h, r, g, 0, 255);
}

void DirectDraw::DrawHealthBarBack(float x, float y, float w, float h)
{
	FillRGB(x, y, w, h, 0, 0, 0, 255);
}

int DirectDraw::DrawString(char* String, int x, int y, int r, int g, int b, ID3DXFont* ifont)
{
		RECT ShadowPos;
		ShadowPos.left = x + 1;
		ShadowPos.top = y + 1;
		RECT FontPos;
		FontPos.left = x;
		FontPos.top = y;
		ifont->DrawTextA(0, String, strlen(String), &ShadowPos, DT_NOCLIP, D3DCOLOR_ARGB(255, r / 3, g / 3, b / 3));
		ifont->DrawTextA(0, String, strlen(String), &FontPos, DT_NOCLIP, D3DCOLOR_ARGB(255, r, g, b));
		return 0;
}

void DirectDraw::DrawOutlinedFontText(INT X, INT Y, int r, int g, int b, ID3DXFont* ifont, PCHAR szString, ...)
{
	CHAR szBuffer[101] = "";

	va_list ArgumentList;
	va_start(ArgumentList, szString);
	vsnprintf_s(szBuffer, sizeof(szBuffer), sizeof(szBuffer) - strlen(szBuffer), szString, ArgumentList);
	va_end(ArgumentList);

	RECT Rect1 = { X, Y, X + 500, Y + 50 };
	RECT Rect2 = { X + 1, Y, X + 500, Y + 50 };
	RECT Rect3 = { X, Y + 1, X + 500, Y + 50 };

	ifont->DrawTextA(0, szString, strlen(szString), &Rect1, DT_NOCLIP, D3DCOLOR_ARGB(255, r / 3, g / 3, b / 3));
	ifont->DrawTextA(0, szString, strlen(szString), &Rect2, DT_NOCLIP, D3DCOLOR_ARGB(255, r / 3, g / 3, b / 3));
	ifont->DrawTextA(0, szString, strlen(szString), &Rect3, DT_NOCLIP, D3DCOLOR_ARGB(255, r / 3, g / 3, b / 3));
}

/*
int DirectDraw::DrawShadowString(int x, int y, int r, int g, int b, ID3DXFont* ifont,char* String, ...)
{
	RECT Font;
	Font.left = x;
	Font.top = y;
	RECT Fonts;
	Fonts.left = x + 1;
	Fonts.top = y;
	RECT Fonts1;
	Fonts1.left = x - 1;
	Fonts1.top = y;
	RECT Fonts2;
	Fonts2.left = x;
	Fonts2.top = y + 1;
	RECT Fonts3;
	Fonts3.left = x;
	Fonts3.top = y - 1;
	ifont->DrawTextA(0, String, strlen(String), &Fonts3, DT_NOCLIP, D3DCOLOR_ARGB(255, 1, 1, 1));
	ifont->DrawTextA(0, String, strlen(String), &Fonts2, DT_NOCLIP, D3DCOLOR_ARGB(255, 1, 1, 1));
	ifont->DrawTextA(0, String, strlen(String), &Fonts1, DT_NOCLIP, D3DCOLOR_ARGB(255, 1, 1, 1));
	ifont->DrawTextA(0, String, strlen(String), &Fonts, DT_NOCLIP, D3DCOLOR_ARGB(255, 1, 1, 1));
	ifont->DrawTextA(0, String, strlen(String), &Font, DT_NOCLIP, D3DCOLOR_ARGB(255, r, g, b));
	return 0;
}*/