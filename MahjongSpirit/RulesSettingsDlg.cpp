// RulesSettingsDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MahjongSpirit.h"
#include "RulesSettingsDlg.h"
#include "afxdialogex.h"


// CRulesSettingsDlg 对话框

IMPLEMENT_DYNAMIC(CRulesSettingsDlg, CDialogEx)

CRulesSettingsDlg::CRulesSettingsDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CRulesSettingsDlg::IDD, pParent)
	, MatchSystem(2)
	, DoubleWindTile(true)
	, OpenQuadRenShan(false)
	, DrawMangan(false)
	, NineOrphans(false)
	, NegativeEnd(false)
	, ChanAnKan(true)
	, TopBonus(false)
	, MatchOriginMarks(25000)
	, HorseMarksX(10)
	, HorseMarksY(5)
	, ReturnMarks(30000)
{

}

CRulesSettingsDlg::~CRulesSettingsDlg()
{
}

void CRulesSettingsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_ORIGINMARKS, MatchOriginMarks);
	DDX_Text(pDX, IDC_EDIT_HORSEMARKS_X, HorseMarksX);
	DDX_Text(pDX, IDC_EDIT_HORSEMARKS_Y, HorseMarksY);
	DDX_Text(pDX, IDC_EDIT_RETURNMARKS, ReturnMarks);
	DDX_Control(pDX, IDC_EDIT_RETURNMARKS, m_EditReturnMarks);
}


BEGIN_MESSAGE_MAP(CRulesSettingsDlg, CDialogEx)
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_BUTTON_SAVERULES, &CRulesSettingsDlg::OnBnClickedButtonSaverules)
	ON_BN_CLICKED(IDC_RADIO_TOPBONUS_ON, &CRulesSettingsDlg::OnBnClickedRadioTopbonusOn)
	ON_BN_CLICKED(IDC_RADIO_TOPBONUS_OFF, &CRulesSettingsDlg::OnBnClickedRadioTopbonusOff)
END_MESSAGE_MAP()


// CRulesSettingsDlg 消息处理程序

/*
int CRulesSettingsDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
	UpdateData(false);
	
	CheckRadioButton(IDC_RADIO_EASTONLY, IDC_RADIO_FULLMATCH, IDC_RADIO_EASTONLY + MatchSystem - 1);
	CheckRadioButton(IDC_RADIO_TWOFU, IDC_RADIO_FOURFU, IDC_RADIO_TWOFU + DoubleWindTile);
	CheckRadioButton(IDC_RADIO_RENSHAN_PAYALL, IDC_RADIO_RENSHAN_NOTPAYALL, IDC_RADIO_RENSHAN_NOTPAYALL - OpenQuadRenShan);
	CheckRadioButton(IDC_RADIO_DRAWMANGAN_ON, IDC_RADIO_DRAWMANGAN_OFF, IDC_RADIO_DRAWMANGAN_OFF - DrawMangan);
	CheckRadioButton(IDC_RADIO_ORPHANSDRAW_ON, IDC_RADIO_ORPHANSDRAW_OFF, IDC_RADIO_ORPHANSDRAW_OFF - NineOrphans);
	CheckRadioButton(IDC_RADIO_NEGATIVEMARKSEND_ON, IDC_RADIO_NEGATIVEMARKSEND_OFF, IDC_RADIO_NEGATIVEMARKSEND_OFF - NegativeEnd);
	CheckRadioButton(IDC_RADIO_CHANANKAN_ON, IDC_RADIO_CHANANKAN_OFF, IDC_RADIO_CHANANKAN_OFF - ChanAnKan);
	CheckRadioButton(IDC_RADIO_TOPBONUS_ON, IDC_RADIO_TOPBONUS_OFF, IDC_RADIO_TOPBONUS_OFF - TopBonus);
	m_EditReturnMarks.EnableWindow(TopBonus);
	return 0;
}*/


BOOL CRulesSettingsDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	UpdateData(false);
	
	CheckRadioButton(IDC_RADIO_EASTONLY, IDC_RADIO_FULLMATCH, IDC_RADIO_EASTONLY + MatchSystem - 1);
	CheckRadioButton(IDC_RADIO_TWOFU, IDC_RADIO_FOURFU, IDC_RADIO_TWOFU + DoubleWindTile);
	CheckRadioButton(IDC_RADIO_RENSHAN_PAYALL, IDC_RADIO_RENSHAN_NOTPAYALL, IDC_RADIO_RENSHAN_NOTPAYALL - OpenQuadRenShan);
	CheckRadioButton(IDC_RADIO_DRAWMANGAN_ON, IDC_RADIO_DRAWMANGAN_OFF, IDC_RADIO_DRAWMANGAN_OFF - DrawMangan);
	CheckRadioButton(IDC_RADIO_ORPHANSDRAW_ON, IDC_RADIO_ORPHANSDRAW_OFF, IDC_RADIO_ORPHANSDRAW_OFF - NineOrphans);
	CheckRadioButton(IDC_RADIO_NEGATIVEMARKSEND_ON, IDC_RADIO_NEGATIVEMARKSEND_OFF, IDC_RADIO_NEGATIVEMARKSEND_OFF - NegativeEnd);
	CheckRadioButton(IDC_RADIO_CHANANKAN_ON, IDC_RADIO_CHANANKAN_OFF, IDC_RADIO_CHANANKAN_OFF - ChanAnKan);
	CheckRadioButton(IDC_RADIO_TOPBONUS_ON, IDC_RADIO_TOPBONUS_OFF, IDC_RADIO_TOPBONUS_OFF - TopBonus);
	m_EditReturnMarks.EnableWindow(TopBonus);
	
	return TRUE;  // return TRUE unless you set the focus to a control

	// 异常: OCX 属性页应返回 FALSE
}


void CRulesSettingsDlg::OnBnClickedButtonSaverules()
{
	// TODO: 在此添加控件通知处理程序代码
	int Response = MessageBoxW(_T("此操作将重新开始游戏，确认保存？"), _T("提示"), MB_YESNO);
	if (Response == IDYES)
	{
		UpdateData(true);
		MatchSystem = GetCheckedRadioButton(IDC_RADIO_EASTONLY, IDC_RADIO_FULLMATCH) - IDC_RADIO_EASTONLY + 1;
		DoubleWindTile = GetCheckedRadioButton(IDC_RADIO_TWOFU, IDC_RADIO_FOURFU) - IDC_RADIO_TWOFU;
		OpenQuadRenShan = GetCheckedRadioButton(IDC_RADIO_RENSHAN_PAYALL, IDC_RADIO_RENSHAN_NOTPAYALL) == IDC_RADIO_RENSHAN_PAYALL;
		DrawMangan = GetCheckedRadioButton(IDC_RADIO_DRAWMANGAN_ON, IDC_RADIO_DRAWMANGAN_OFF) == IDC_RADIO_DRAWMANGAN_ON;
		NineOrphans = GetCheckedRadioButton(IDC_RADIO_ORPHANSDRAW_ON, IDC_RADIO_ORPHANSDRAW_OFF) == IDC_RADIO_ORPHANSDRAW_ON;
		NegativeEnd = GetCheckedRadioButton(IDC_RADIO_NEGATIVEMARKSEND_ON, IDC_RADIO_NEGATIVEMARKSEND_OFF) == IDC_RADIO_NEGATIVEMARKSEND_ON;
		ChanAnKan = GetCheckedRadioButton(IDC_RADIO_CHANANKAN_ON, IDC_RADIO_CHANANKAN_OFF) == IDC_RADIO_CHANANKAN_ON;
		TopBonus = GetCheckedRadioButton(IDC_RADIO_TOPBONUS_ON, IDC_RADIO_TOPBONUS_OFF) == IDC_RADIO_TOPBONUS_ON;
		EndDialog(IDOK);
	}
}


void CRulesSettingsDlg::OnBnClickedRadioTopbonusOn()
{
	// TODO: 在此添加控件通知处理程序代码
	m_EditReturnMarks.EnableWindow(true);
}


void CRulesSettingsDlg::OnBnClickedRadioTopbonusOff()
{
	// TODO: 在此添加控件通知处理程序代码
	m_EditReturnMarks.EnableWindow(false);
}
