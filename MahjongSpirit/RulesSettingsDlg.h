#pragma once
#include "afxwin.h"


// CRulesSettingsDlg 对话框

class CRulesSettingsDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CRulesSettingsDlg)

public:
	CRulesSettingsDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CRulesSettingsDlg();

// 对话框数据
	enum { IDD = IDD_RULESSETTINGSDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	int MatchSystem;		//赛制设定，0：东风战无限循环，1：东风战，2：半庄，3：全庄
	bool DoubleWindTile;	//连风对子符数，true为4符，false为2符
	bool OpenQuadRenShan;	//大明杠岭上开花包牌
	bool DrawMangan;		//流局满贯
	bool NineOrphans;		//九种九牌流局
	bool NegativeEnd;		//击飞（负分结束）
	bool ChanAnKan;			//十三幺抢暗杠
	
	bool TopBonus;			//头名赏

	//afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	int MatchOriginMarks;	//初始分
	int HorseMarksX;
	int HorseMarksY;
	int ReturnMarks;
	CEdit m_EditReturnMarks;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonSaverules();
	afx_msg void OnBnClickedRadioTopbonusOn();
	afx_msg void OnBnClickedRadioTopbonusOff();
};
