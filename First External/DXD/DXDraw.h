#pragma once

#define WIN32_LEAN_AND_MEAN

#include "../Includes.h"

class DirectDraw
{
public:
	DirectDraw();
	~DirectDraw();

	void GradientFunc(int x, int y, int w, int h, int r, int g, int b, int a);
	void DrawCenterLine(float x, float y, int width, int r, int g, int b);
	void DrawLine(float x, float y, float xx, float yy, int r, int g, int b, int a);
	void FillRGB(float x, float y, float w, float h, int r, int g, int b, int a);
	void DrawBox(float x, float y, float width, float height, float px, int r, int g, int b, int a);
	void DrawGUIBox(float x, float y, float w, float h, int r, int g, int b, int a, int rr, int gg, int bb, int aa);
	void DrawHealthBar(float x, float y, float w, float h, int r, int g);
	void DrawHealthBarBack(float x, float y, float w, float h);

	void DrawOutlinedFontText(INT X, INT Y, int r, int g, int b, ID3DXFont* ifont, PCHAR szString, ...);

	int DrawString(char* String, int x, int y, int r, int g, int b, ID3DXFont* ifont);
	//int DrawShadowString(char* String, int x, int y, int r, int g, int b, ID3DXFont* ifont);
};

extern DirectDraw* DXDraw;