
// MahjongSpirit.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CMahjongSpiritApp:
// �йش����ʵ�֣������ MahjongSpirit.cpp
//

class CMahjongSpiritApp : public CWinApp
{
public:
	CMahjongSpiritApp();
	
// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CMahjongSpiritApp theApp;