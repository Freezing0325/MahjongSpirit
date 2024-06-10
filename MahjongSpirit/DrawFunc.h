#pragma once

#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#ifndef _CRT_NONSTDC_NO_DEPRECATE 
#define _CRT_NONSTDC_NO_DEPRECATE 
#endif

#define TILE_SURFACE_WIDTH 50
#define TILE_SURFACE_HEIGHT 69
#define TILE_STRAIGHT_WIDTH 50
#define TILE_STRAIGHT_HEIGHT 80
#define TILE_STRAIGHT_SURFACE_HEIGHT 49
#define TILE_ACROSS_WIDTH 69
#define TILE_ACROSS_HEIGHT 62
#define TILE_ACROSS_THICKNESS 38
#define TILE_ACROSS_SURFACE_HEIGHT 31
#define TILE_SPACE 1
#define TILE_NEWSPACE 6
#define TILE_SELECTED_UP 6

#define CHOOSE_COLUMN_WIDTH 700
#define CHOOSE_COLUMN_HEIGHT 80
#define CHOOSE_BUTTON_WIDTH 64
#define CHOOSE_BUTTON_HEIGHT 80
#define CHOOSE_BUTTON_COLOR RGB(255, 242, 0)
#define CHOOSE_TEXT_COLOR RGB(5, 5, 5)
#define CHOOSE_CHI 0x1
#define CHOOSE_PON 0x2
#define CHOOSE_KAN 0x4
#define CHOOSE_RIICHI 0x8
#define CHOOSE_HU 0x10
#define CHOOSE_DRAW 0x20

#define GRADIENT_FROMCENTER 0x1
#define GRADIENT_FROMLEFTTOP 0x2
#define GRADIENT_FROMLEFTBOTTOM 0x3
#define GRADIENT_FROMRIGHTTOP 0x4
#define GRADIENT_FROMRIGHTBOTTOM 0x5

#include <Windows.h>
#include <cmath>
#include "structs.h"


// 创建特定字体种类和字体大小的句柄
HFONT CreateMyFont(CString fontname, int size);
HFONT CreateMyFont(const char* fontname, int size);

// 以九十度为间隔逆时针旋转图像，需要传入：需要被旋转的CImage变量，旋转次数。
BOOL SpinImage(CImage &DestImage, BYTE SpinTimes = 1);
// 获取选择框图像，需要传入：最后存储图像的CImage变量，全部选项信息
BOOL ChooseColumn(CImage& DestImage, UINT nFlags);
// 在特定高度显示选择框
void ShowChooseColumn(HDC Hdc, int ShowY, UINT nFlags = 0x0);
// 在特定位置显示有一定透明度，可自定义字体和大小的文字
BOOL ShowTransparentText(CWnd* DestControl, CWnd* BackControl, CString Text, CString Font, int FontSize, COLORREF FontColor, POINT ShowPos, BYTE Alpha, CImage& TextImage, CImage& BackImage, BOOL IfRemake = false);
// 实现两张图像的渐变
BOOL ChangeSurface(CWnd* SurfaceControl, CImage& LastSurface, CImage& NextSurface, int enter_rate);
// 在特定位置显示特定颜色的半透明提示框
BOOL ShowHintBox(HDC Hdc, POINT ShowPos, int Width, int Height, COLORREF BoxColor = RGB(0, 0, 255), BYTE Alpha = 100);
// 从资源文件中读取图片资源
bool LoadImageFromResource(CImage& DestImage, UINT nResID, LPCTSTR lpTyp);
bool CreateDefaultCutScenesImage(CImage& DestImage, int Direction, int DlgWidth, int DlgHeight, const COLORREF &BackColor = RGB(220, 220, 220));
// 加载过场动画
bool ShowCutScenes(HDC Hdc, int Direction, int CutRate, const int& DlgWidth, const int& DlgHeight);
bool ShowCutScenes(HDC Hdc, const CImage &SceneImage, const int& DlgWidth, const int& DlgHeight, const POINT &FullShowPos, double LoadRate, int ForwardAngleByDegree, int Alpha = 100);
// 在特定位置显示有一定透明度，可自定义字体和大小的文字
BOOL ShowTransparentText(const HDC &Hdc, const CString &Text, const CString &Font, const int& FontSize, const COLORREF &FontColor, const POINT &ShowPos, const BYTE &Alpha);
// 获取渐变色图片
bool GetGradientColorImage(CImage& DestImage, const COLORREF &BeginColor, const COLORREF &EndColor, int DestWidth, int DestHeight, BYTE Flag = GRADIENT_FROMCENTER);