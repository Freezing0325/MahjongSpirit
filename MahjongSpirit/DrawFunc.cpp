#include "stdafx.h"
#include "DrawFunc.h"
#include "Resource.h"

HFONT CreateMyFont(CString fontname, int size)
{
	return CreateFontW				//引入新字体
					(
					size,				//字体高度
					0,
					0,
					0,
					FW_NORMAL,
					0,				//定义斜体
					0,				//定义输出时带下划线
					0,
					DEFAULT_CHARSET/*GB2312_CHARSET*/,		//所使用的字符集
					OUT_DEFAULT_PRECIS,	CLIP_DEFAULT_PRECIS,
					DEFAULT_QUALITY,DEFAULT_PITCH|FF_DONTCARE,
					fontname	
					);
}

HFONT CreateMyFont(const char* fontname, int size)
{
	return CreateFontA				//引入新字体
					(
					size,				//字体高度
					0,
					0,
					0,
					FW_NORMAL,
					0,				//定义斜体
					0,				//定义输出时带下划线
					0,
					DEFAULT_CHARSET,		//所使用的字符集
					OUT_DEFAULT_PRECIS,	CLIP_DEFAULT_PRECIS,
					DEFAULT_QUALITY,DEFAULT_PITCH|FF_DONTCARE,
					fontname	
					);
}

BOOL SpinImage(CImage &DestImage, BYTE SpinTimes)
{
	if (DestImage.IsNull())
		return false;
	if (SpinTimes == 0)
		return true;
	CImage TempImage;
	int src_width = DestImage.GetWidth(), src_height = DestImage.GetHeight();
	if (SpinTimes == 1 || SpinTimes == 3)
	{
		TempImage.Create(src_height, src_width, 32);
		HDC hTempDC = TempImage.GetDC();
		BitBlt(hTempDC, 0, 0, src_height, src_width, NULL, 0, 0, WHITENESS);
		for (int i = 0; i < src_width; i++)
			for (int j = 0; j < src_height; j++)
			{
				COLORREF ThisColor = DestImage.GetPixel(i, j);
				if (SpinTimes == 1)
					TempImage.SetPixel(j, src_width - 1 - i, ThisColor);
				else
					TempImage.SetPixel(src_height - 1 - j, i, ThisColor);
			}
		DestImage.Destroy();
		DestImage.Create(src_height, src_width, 32);
		HDC hDestDC = DestImage.GetDC();
		TransparentBlt(hDestDC, 0, 0, src_height, src_width, hTempDC, 0, 0, src_height, src_width, RGB(255, 255, 255));
		TempImage.ReleaseDC();
		DestImage.ReleaseDC();
		return true;
	}
	if (SpinTimes == 2)
	{
		TempImage.Create(src_width, src_height, 32);
		HDC hTempDC = TempImage.GetDC();
		BitBlt(hTempDC, 0, 0, src_height, src_width, NULL, 0, 0, WHITENESS);
		for (int i = 0; i < src_width; i++)
			for (int j = 0; j < src_height; j++)
			{
				COLORREF ThisColor = DestImage.GetPixel(i, j);
				TempImage.SetPixel(src_width - 1 - i, src_height - 1 - j, ThisColor);
			}
		DestImage.Destroy();
		DestImage.Create(src_width, src_height, 32);
		HDC hDestDC = DestImage.GetDC();
		TransparentBlt(hDestDC, 0, 0, src_width, src_height, hTempDC, 0, 0, src_width, src_height, RGB(255, 255, 255));
		TempImage.ReleaseDC();
		DestImage.ReleaseDC();
		return true;
	}
	TempImage.Destroy();
	return false;
}

BOOL ChooseColumn(CImage& DestImage, UINT nFlags)
{
	static UINT OldFlags;
	if (nFlags == 0x0)
		nFlags = OldFlags;
	else
		OldFlags = nFlags;
	bool chi = nFlags & CHOOSE_CHI;
	bool pon = nFlags & CHOOSE_PON;
	bool kan = nFlags & CHOOSE_KAN;
	bool riichi = nFlags & CHOOSE_RIICHI;
	bool hu = nFlags & CHOOSE_HU;
	bool draw = nFlags & CHOOSE_DRAW;
	if (!chi && !pon && !kan && !riichi && !hu && !draw)
		return false;
	if (!DestImage.IsNull())
		DestImage.Destroy();
	int ButtonNum = chi + pon + kan + riichi + hu + draw;
	bool ThisButtonValid[] = {chi, pon, kan, riichi, hu, draw};
	CString ButtonText[] = {_T(" 吃 "), _T(" 碰 "), _T(" 杠 "), _T("立直"), _T(" 和 "), _T("流局")};
	int DestWidth = CHOOSE_BUTTON_WIDTH * (4 + ButtonNum * 1.5);
	DestImage.Create(DestWidth, CHOOSE_COLUMN_HEIGHT, 32);
	HDC hDestDC = DestImage.GetDC();

	static CImage AllImage;
	if (AllImage.IsNull())
		LoadImageFromResource(AllImage, IDB_PNG_CHOOSE_COLUMN, _T("PNG"));
	//AllImage.Load(_T("选择栏.png"));
	HDC hAllDC = AllImage.GetDC();

	BitBlt(hDestDC, 0, 0, DestWidth, CHOOSE_COLUMN_HEIGHT, hAllDC, 0, 0, SRCCOPY);
	int ButtonX = CHOOSE_BUTTON_WIDTH;
	HFONT hf = CreateMyFont(_T("华康勘亭流W9(P)"), 30);
	SetTextColor(hDestDC, CHOOSE_TEXT_COLOR);
	SetBkMode(hDestDC, TRANSPARENT);
	SelectObject(hDestDC, hf);
	for (int i = 0; i < ButtonNum; i++)
	{
		TransparentBlt(hDestDC, ButtonX, 0, CHOOSE_BUTTON_WIDTH, CHOOSE_BUTTON_HEIGHT, 
			hAllDC, CHOOSE_COLUMN_WIDTH + 1, 0, CHOOSE_BUTTON_WIDTH, CHOOSE_BUTTON_HEIGHT, RGB(255, 255, 255));
		int ThisIndex = 0;
		for (int ValidNum = 0; ValidNum <= i; ValidNum += ThisButtonValid[ThisIndex ++]);
		ThisIndex --;
		TextOut(hDestDC, ButtonX, 24, ButtonText[ThisIndex], lstrlenW(ButtonText[ThisIndex]));
		ButtonX += 1.5 * CHOOSE_BUTTON_WIDTH;
	}
	ButtonX += 0.5 * CHOOSE_BUTTON_WIDTH;
	TransparentBlt(hDestDC, ButtonX, 0, CHOOSE_BUTTON_WIDTH, CHOOSE_BUTTON_HEIGHT, 
		hAllDC, CHOOSE_COLUMN_WIDTH + 1, 0, CHOOSE_BUTTON_WIDTH, CHOOSE_BUTTON_HEIGHT, RGB(255, 255, 255));
	TextOut(hDestDC, ButtonX, 24, _T("取消"), lstrlenW(_T("取消")));
	DeleteObject(hf);
	DestImage.ReleaseDC();
	AllImage.ReleaseDC();
	AllImage.Destroy();
	return true;
}

void ShowChooseColumn(HDC Hdc, int ShowY, UINT nFlags)
{
	if (nFlags == 0) return;
	CImage ChooseColumnImage;
	ChooseColumn(ChooseColumnImage, nFlags);
	HDC hChooseColumnDC = ChooseColumnImage.GetDC();
	TransparentBlt(Hdc, 1200 - ChooseColumnImage.GetWidth(), ShowY, ChooseColumnImage.GetWidth(), ChooseColumnImage.GetHeight(),
		hChooseColumnDC, 0, 0, ChooseColumnImage.GetWidth(), ChooseColumnImage.GetHeight(), RGB(255, 255, 255));
	ChooseColumnImage.ReleaseDC();
	ChooseColumnImage.Destroy();
}

BOOL ShowHintBox(HDC Hdc, POINT ShowPos, int Width, int Height, COLORREF BoxColor, BYTE Alpha)
{
	CImage BoxImage;
	RECT BoxRect = {0, 0, Width, Height};
	BoxImage.Create(Width, Height, 32);
	HDC hCoverColorDC = BoxImage.GetDC();
	HBRUSH hBr = CreateSolidBrush(BoxColor);
	FillRect(hCoverColorDC, &BoxRect, hBr);
	DeleteObject(hBr);
	BOOL Flag = BoxImage.AlphaBlend(Hdc, ShowPos, Alpha);
	BoxImage.ReleaseDC();
	BoxImage.Destroy();
	return Flag;
}

BOOL ShowTransparentText(CWnd* DestControl, CWnd* BackControl, CString Text, CString Font, int FontSize, COLORREF FontColor, POINT ShowPos, BYTE Alpha, CImage& TextImage, CImage& BackImage, BOOL IfRemake)
{
	CDC* pDestDC = DestControl->GetDC();
	HDC hDestDC = pDestDC->GetSafeHdc();
	
	if (IfRemake || TextImage.IsNull() || BackImage.IsNull())
	{
		if (IfRemake && !TextImage.IsNull())
			TextImage.Destroy();
		if (IfRemake && !BackImage.IsNull())
			BackImage.Destroy();
		CDC* pBackDC = BackControl->GetDC();
		HDC hBackDC = pBackDC->GetSafeHdc();
		CImage image;
		HFONT hf = CreateMyFont(Font, FontSize);
		TEXTMETRIC tm;
		image.Create(1, 1, 32);
		HDC himageDC = image.GetDC();
		SelectObject(himageDC, hf);
		SetBkMode(himageDC, TRANSPARENT);
		SetTextColor(himageDC, FontColor);
		GetTextMetrics(himageDC, &tm);
		SIZE btn_size = {tm.tmMaxCharWidth * lstrlenW(Text), tm.tmHeight};
		DeleteObject(hf);
		image.ReleaseDC();
		image.Destroy();
		
		image.Create(btn_size.cx, btn_size.cy, 32);
		himageDC = image.GetDC();

		BitBlt(himageDC, 0, 0, btn_size.cx, btn_size.cy, hBackDC, ShowPos.x, ShowPos.y, SRCCOPY);
		BackImage.Create(btn_size.cx, btn_size.cy, 32);
		HDC hback_imageDC = BackImage.GetDC();
		BitBlt(hback_imageDC, 0, 0, btn_size.cx, btn_size.cy, himageDC, 0, 0, SRCCOPY);
		BackImage.ReleaseDC();

		hf = CreateMyFont(Font, FontSize);
		SelectObject(himageDC, hf);
		TextOut(himageDC, 0, 0, Text, lstrlenW(Text));
		DeleteObject(hf);
		TextImage.Create(btn_size.cx, btn_size.cy, 32);
		HDC htext_imageDC = TextImage.GetDC();
		BitBlt(htext_imageDC, 0, 0, btn_size.cx, btn_size.cy, himageDC, 0, 0, SRCCOPY);
		TextImage.ReleaseDC();
		image.ReleaseDC();
		image.Destroy();
		DestControl->SetWindowPos(NULL, ShowPos.x, ShowPos.y, btn_size.cx, btn_size.cy, SWP_NOZORDER);
	}
	HDC hback_imageDC = BackImage.GetDC();
	HDC htext_imageDC = TextImage.GetDC();
	BitBlt(hDestDC, 0, 0, TextImage.GetWidth(), TextImage.GetHeight(), hback_imageDC, 0, 0, SRCCOPY);
	BLENDFUNCTION bf = {AC_SRC_OVER, 0, Alpha, AC_SRC_ALPHA};
	AlphaBlend(hDestDC, 0, 0, TextImage.GetWidth(), TextImage.GetHeight(), htext_imageDC, 0, 0, TextImage.GetWidth(), TextImage.GetHeight(), bf);
	BackImage.ReleaseDC();
	TextImage.ReleaseDC();
	return true;
}

BOOL ChangeSurface(CWnd* SurfaceControl, CImage& LastSurface, CImage& NextSurface, int enter_rate)
{
	CDC* surfaceDC = SurfaceControl->GetDC();
	HDC hsurfaceDC = surfaceDC->GetSafeHdc();
	RECT surfaceRect;
	SurfaceControl->GetWindowRect(&surfaceRect);
	if (LastSurface.IsNull())
	{
		LastSurface.Create(surfaceRect.right, surfaceRect.bottom, 32);
		HDC hlastSurfaceDC = LastSurface.GetDC();
		BitBlt(hlastSurfaceDC, 0, 0, surfaceRect.right, surfaceRect.bottom, surfaceDC->GetSafeHdc(), 0, 0, SRCCOPY);
		LastSurface.ReleaseDC();
	}
	CImage TempImage;
	TempImage.Create(surfaceRect.right, surfaceRect.bottom, 32);
	
	if (enter_rate == 50)
	{
		LastSurface.Destroy();
		LastSurface.Create(surfaceRect.right, surfaceRect.bottom, 32);
		HDC hlastSurfaceDC = LastSurface.GetDC(), hnextSurfaceDC = NextSurface.GetDC();
		BitBlt(hlastSurfaceDC, 0, 0, surfaceRect.right, surfaceRect.bottom, hnextSurfaceDC, 0, 0, SRCCOPY);
		LastSurface.ReleaseDC();
		NextSurface.ReleaseDC();
	}
	HDC htempimageDC = TempImage.GetDC(), hlastSurfaceDC = LastSurface.GetDC();
	BLENDFUNCTION bf = {AC_SRC_OVER, 0, 255 * abs(50 - enter_rate) / 50, AC_SRC_ALPHA};
	AlphaBlend(htempimageDC, 0, 0, surfaceRect.right, surfaceRect.bottom, hlastSurfaceDC, 0, 0, surfaceRect.right, surfaceRect.bottom, bf);
	LastSurface.ReleaseDC();
	BitBlt(hsurfaceDC, 0, 0, surfaceRect.right, surfaceRect.bottom, htempimageDC, 0, 0, SRCCOPY);
	TempImage.ReleaseDC();
	TempImage.Destroy();
	SurfaceControl->ReleaseDC(surfaceDC);
	
	return true;
}

bool LoadImageFromResource(CImage& DestImage, UINT nResID, LPCTSTR lpTyp)
{
    if (!DestImage.IsNull())
		DestImage.Destroy();
    // 查找资源
    HRSRC hRsrc = ::FindResource(AfxGetResourceHandle(), MAKEINTRESOURCE(nResID), lpTyp);
    if (hRsrc == NULL)
        return false;
    // 加载资源
    HGLOBAL hImgData = ::LoadResource(AfxGetResourceHandle(), hRsrc);
    if (hImgData == NULL)
    {
        //::FreeResource(hImgData);
        return false;
    }
    // 锁定内存中的指定资源
    LPVOID lpVoid = ::LockResource(hImgData);
    LPSTREAM pStream = NULL;
    DWORD dwSize = ::SizeofResource(AfxGetResourceHandle(), hRsrc);
    HGLOBAL hNew = ::GlobalAlloc(GHND, dwSize);
    LPBYTE lpByte = (LPBYTE)::GlobalLock(hNew);
    ::memcpy(lpByte, lpVoid, dwSize);
    // 解除内存中的指定资源
    ::GlobalUnlock(hNew);
    // 从指定内存创建流对象
    HRESULT ht = ::CreateStreamOnHGlobal(hNew, TRUE, &pStream);
    if (ht != S_OK)
    {
        GlobalFree(hNew); 
    }
    else
    {
        // 加载图片
        DestImage.Load(pStream);
        GlobalFree(hNew);
		pStream -> Release(); 
    }
    // 释放资源
    ::FreeResource(hImgData);
    return true;
}

bool CreateDefaultCutScenesImage(CImage& DestImage, int Direction, int DlgWidth, int DlgHeight, const COLORREF &BackColor)
{
	bool Result = true;
	if (!DestImage.IsNull()) DestImage.Destroy();
	switch(Direction)
	{
	case 0:
	case 2:
	case 4:
		DestImage.Create(DlgWidth, 200, 32);
		break;
	case 1:
	case 3:
		DestImage.Create(200, DlgHeight, 32);
		break;
	}
	POINT PolyPoint[5];
	switch(Direction)
	{
		case 0:
		{
			const POINT ThisPolyPoint[5] = {{0, 100}, {DlgWidth, 0}, {DlgWidth, 200}, {0, 200}, {0, 100}};
			for (int i = 0; i < 5; PolyPoint[i++] = ThisPolyPoint[i]);
		}
		break;
		case 1:
		{
			const POINT ThisPolyPoint[5] = {{0, 0}, {200, 0}, {200, DlgHeight}, {100, DlgHeight}, {0, 0}};
			for (int i = 0; i < 5; PolyPoint[i++] = ThisPolyPoint[i]);
		}
		break;
		case 2:
		{
			const POINT ThisPolyPoint[5] = {{0, 0}, {DlgWidth, 0}, {DlgWidth, 100}, {0, 200}, {0, 0}};
			for (int i = 0; i < 5; PolyPoint[i++] = ThisPolyPoint[i]);
		}
		break;
		case 3:
		{
			const POINT ThisPolyPoint[5] = {{0, 0}, {100, 0}, {200, DlgHeight}, {0, DlgHeight}, {0, 0}};
			for (int i = 0; i < 5; PolyPoint[i++] = ThisPolyPoint[i]);
		}
		break;
		case 4:
		{
			const POINT ThisPolyPoint[5] = {{0, 0}, {DlgWidth, 0}, {DlgWidth, 200}, {0, 200}, {0, 0}};
			for (int i = 0; i < 5; PolyPoint[i++] = ThisPolyPoint[i]);
		}
		break;
	}
	HDC hBoxDC = DestImage.GetDC();
	RECT DestRect = {0, 0, DestImage.GetWidth(), DestImage.GetHeight()};
	FillRect(hBoxDC, &DestRect, (HBRUSH)GetStockObject(WHITE_BRUSH));
	HBRUSH hBr = CreateSolidBrush(BackColor);
	HBRUSH hBr_old = (HBRUSH)SelectObject(hBoxDC, hBr);
	HPEN hP = CreatePen(PS_SOLID, 1, BackColor);
	HPEN hP_old = (HPEN)SelectObject(hBoxDC, hP);
	Polygon(hBoxDC, PolyPoint, 4);
	SelectObject(hBoxDC, hBr_old);
	SelectObject(hBoxDC, hP_old);
	DeleteObject(hBr);
	DeleteObject(hP);
	DestImage.ReleaseDC();
	return Result;
}

bool ShowCutScenes(HDC Hdc, int Direction, int CutRate, const int& DlgWidth, const int& DlgHeight)
{
	static CImage *pBoxImage;
	HDC hBoxDC;
	bool Result = true;
	const int AllShowRate = 4;

	const COLORREF BackColor = RGB(220, 220, 220);
	if (CutRate == 1)
	{
		pBoxImage = new CImage;
		if (pBoxImage == nullptr) return false;
		switch(Direction)
		{
		case 0:
		case 2:
		case 4:
			pBoxImage->Create(DlgWidth, 200, 32);
			break;
		case 1:
		case 3:
			pBoxImage->Create(200, DlgHeight, 32);
			break;
		}
	}
	if (pBoxImage == nullptr) return false;
	POINT PolyPoint[5];
	POINT BoxShowPos;
	switch(Direction)
	{
		case 0:
		{
			const POINT ThisPolyPoint[5] = {{0, 100}, {DlgWidth, 0}, {DlgWidth, 200}, {0, 200}, {0, 100}};
			for (int i = 0; i < 5; PolyPoint[i++] = ThisPolyPoint[i]);
			BoxShowPos.x = 0; BoxShowPos.y = DlgHeight - 200;
			if (CutRate < AllShowRate) BoxShowPos.x -= (AllShowRate - CutRate) * DlgWidth / AllShowRate;
			else if (CutRate > 100 - AllShowRate) BoxShowPos.x += (CutRate - 100 + AllShowRate) * DlgWidth / AllShowRate;
		}
		break;
		case 1:
		{
			const POINT ThisPolyPoint[5] = {{0, 0}, {200, 0}, {200, DlgHeight}, {100, DlgHeight}, {0, 0}};
			for (int i = 0; i < 5; PolyPoint[i++] = ThisPolyPoint[i]);
			BoxShowPos.x = DlgWidth - 200; BoxShowPos.y = 0;
			if (CutRate < AllShowRate) BoxShowPos.y += (AllShowRate - CutRate) * DlgHeight / AllShowRate;
			else if (CutRate > 100 - AllShowRate) BoxShowPos.y -= (CutRate - 100 + AllShowRate) * DlgHeight / AllShowRate;
		}
		break;
		case 2:
		{
			const POINT ThisPolyPoint[5] = {{0, 0}, {DlgWidth, 0}, {DlgWidth, 100}, {0, 200}, {0, 0}};
			for (int i = 0; i < 5; PolyPoint[i++] = ThisPolyPoint[i]);
			BoxShowPos.x = 0; BoxShowPos.y = 0;
			if (CutRate < AllShowRate) BoxShowPos.x += (AllShowRate - CutRate) * DlgWidth / AllShowRate;
			else if (CutRate > 100 - AllShowRate) BoxShowPos.x -= (CutRate - 100 + AllShowRate) * DlgWidth / AllShowRate;
		}
		break;
		case 3:
		{
			const POINT ThisPolyPoint[5] = {{0, 0}, {100, 0}, {200, DlgHeight}, {0, DlgHeight}, {0, 0}};
			for (int i = 0; i < 5; PolyPoint[i++] = ThisPolyPoint[i]);
			BoxShowPos.x = 0; BoxShowPos.y = 0;
			if (CutRate < AllShowRate) BoxShowPos.y -= (AllShowRate - CutRate) * DlgHeight / AllShowRate;
			else if (CutRate > 100 - AllShowRate) BoxShowPos.y += (CutRate - 100 + AllShowRate) * DlgHeight / AllShowRate;
		}
		break;
		case 4:
		{
			const POINT ThisPolyPoint[5] = {{0, 0}, {DlgWidth, 0}, {DlgWidth, 200}, {0, 200}, {0, 0}};
			for (int i = 0; i < 5; PolyPoint[i++] = ThisPolyPoint[i]);
			BoxShowPos.x = 0; BoxShowPos.y = DlgHeight / 2 - 100;
			if (CutRate < AllShowRate) BoxShowPos.x -= (AllShowRate - CutRate) * DlgWidth / AllShowRate;
			else if (CutRate > 100 - AllShowRate) BoxShowPos.x += (CutRate - 100 + AllShowRate) * DlgWidth / AllShowRate;
		}
		break;
	}
	hBoxDC = pBoxImage->GetDC();
	Result &= BitBlt(hBoxDC, 0, 0, pBoxImage->GetWidth(), pBoxImage->GetHeight(), Hdc, BoxShowPos.x, BoxShowPos.y, SRCCOPY);
	HBRUSH hBr = CreateSolidBrush(BackColor);
	SelectObject(hBoxDC, hBr);
	HPEN hP = CreatePen(PS_SOLID, 1, BackColor);
	SelectObject(hBoxDC, hP);
	Polygon(hBoxDC, PolyPoint, 4);
	DeleteObject(hBr);
	DeleteObject(hP);
	Result &= pBoxImage->AlphaBlend(Hdc, BoxShowPos, 100);
	pBoxImage->ReleaseDC();

	if (CutRate == 99)
	{
		pBoxImage->Destroy();
		delete[] pBoxImage;
		pBoxImage = nullptr;
	}
	return Result;
}

bool ShowCutScenes(HDC Hdc, const CImage &SceneImage, const int& DlgWidth, const int& DlgHeight, const POINT &FullShowPos, double LoadRate, int ForwardAngleByDegree, int Alpha)
{
	bool Result = true;
	CImage FullImage;
	const double PI = 3.14159265359;
	int ImageWidth = SceneImage.GetWidth(), ImageHeight = SceneImage.GetHeight();
	FullImage.Create(ImageWidth, ImageHeight, 32);
	HDC hFullDC = FullImage.GetDC();
	POINT TrueShowPos = FullShowPos;
	double DiagLen = sqrt(pow(DlgHeight, 2) + pow(DlgWidth, 2));
	TrueShowPos.x += LoadRate * DiagLen * cos(ForwardAngleByDegree / 180.0 * PI);
	TrueShowPos.y -= LoadRate * DiagLen * sin(ForwardAngleByDegree / 180.0 * PI);
	Result &= BitBlt(hFullDC, 0, 0, ImageWidth, ImageHeight, Hdc, TrueShowPos.x, TrueShowPos.y, SRCCOPY);
	Result &= SceneImage.TransparentBlt(hFullDC, 0, 0, ImageWidth, ImageHeight, RGB(255, 255, 255));
	Result &= FullImage.AlphaBlend(Hdc, TrueShowPos, Alpha);
	FullImage.ReleaseDC();
	FullImage.Destroy();
	return Result;
}

BOOL ShowTransparentText(const HDC &Hdc, const CString &Text, const CString &Font, const int& FontSize, const COLORREF &FontColor, const POINT &ShowPos, const BYTE &Alpha)
{
	CImage TextImage;
	BOOL Result = true;
	HFONT hf = CreateMyFont(Font, FontSize);
	HFONT hOldFont = (HFONT)SelectObject(Hdc, hf);
	TEXTMETRIC tm;
	GetTextMetrics(Hdc, &tm);
	SelectObject(Hdc, hOldFont);
	int TextWidth = tm.tmMaxCharWidth * Text.GetLength(), TextHeight = tm.tmHeight;
	TextImage.Create(TextWidth, TextHeight, 32);
	HDC hTextImageDC = TextImage.GetDC();
	Result &= BitBlt(hTextImageDC, 0, 0, TextWidth, TextHeight, Hdc, ShowPos.x, ShowPos.y, SRCCOPY);

	SelectObject(hTextImageDC, hf);
	SetBkMode(hTextImageDC, TRANSPARENT);
	SetTextColor(hTextImageDC, FontColor);
	Result &= TextOut(hTextImageDC, 0, 0, Text, Text.GetLength());
	Result &= TextImage.AlphaBlend(Hdc, ShowPos, Alpha);
	DeleteObject(hf);
	TextImage.ReleaseDC();
	TextImage.Destroy();
	return Result;
}

bool GetGradientColorImage(CImage& DestImage, const COLORREF &BeginColor, const COLORREF &EndColor, int DestWidth, int DestHeight, BYTE Flag)
{
	if (!DestImage.IsNull()) DestImage.Destroy();
	DestImage.Create(DestWidth, DestHeight, 32);
	BYTE* pSourceData = (BYTE*)DestImage.GetBits();
	int ImagePitch = DestImage.GetPitch();
	int StartRed = GetRValue(BeginColor), StartGreen = GetGValue(BeginColor), StartBlue = GetBValue(BeginColor);
	int EndRed = GetRValue(EndColor), EndGreen = GetGValue(EndColor), EndBlue = GetBValue(EndColor);
	switch (Flag)
	{
	case GRADIENT_FROMCENTER:
		{
			double End = sqrt(DestHeight*DestHeight / 4.0 + DestWidth*DestWidth / 4.0);
			double DistanceSquare = 0.0;
			for (int i = 0; i < DestWidth; i++)
			{
				DistanceSquare = (i - DestWidth / 2.0)*(i - DestWidth / 2.0);
				for (int j = 0; j < DestHeight; j++)
				{
					double ThisDistanceSquare = DistanceSquare + (j - DestHeight / 2.0)*(j - DestHeight / 2.0);
					double GradientNum = sqrt(ThisDistanceSquare) / End;
					

					COLORREF ThisColor = RGB(GradientNum * EndRed + (1 - GradientNum) * StartRed,
											 GradientNum * EndGreen + (1 - GradientNum) * StartGreen,
											 GradientNum * EndBlue + (1 - GradientNum) * StartBlue);
					*(pSourceData + j * ImagePitch + i * 4) = GetBValue(ThisColor);
					*(pSourceData + j * ImagePitch + i * 4 + 1) = GetGValue(ThisColor);
					*(pSourceData + j * ImagePitch + i * 4 + 2) = GetRValue(ThisColor);
					*(pSourceData + j * ImagePitch + i * 4 + 3) = 255;
					//DestImage.SetPixel(i, j, ThisColor);
				}
			}
		}
		break;
	default:
		break;
	}
	return true;
}