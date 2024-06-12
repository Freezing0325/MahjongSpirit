// RulesSettingsDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "MahjongSpirit.h"
#include "RulesSettingsDlg.h"
#include "afxdialogex.h"


// CRulesSettingsDlg �Ի���

IMPLEMENT_DYNAMIC(CRulesSettingsDlg, CDialogEx)

CRulesSettingsDlg::CRulesSettingsDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CRulesSettingsDlg::IDD, pParent)
	, MatchFormat(2)
	, DoubleWindTile(true)
	, OpenQuadRinShan(true)
	, DrawMangan(true)
	, NineOrphans(true)
	, NegativeEnd(true)
	, ChanAnKan(true)
	, TopBonus(false)
	, BackMarks(30000)
	, RankHorse_1(10)
	, RankHorse_2(5)
	, OriginPoints(25000)
{

}

CRulesSettingsDlg::~CRulesSettingsDlg()
{
}

void CRulesSettingsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_ORIGINMARKS, OriginPoints);
	DDX_Text(pDX, IDC_EDIT_HORSEMARKS_X, RankHorse_1);
	DDX_Text(pDX, IDC_EDIT_HORSEMARKS_Y, RankHorse_2);
	DDX_Text(pDX, IDC_EDIT_RETURNMARKS, BackMarks);
	DDX_Control(pDX, IDC_EDIT_RETURNMARKS, m_EditReturnMarks);
}


BEGIN_MESSAGE_MAP(CRulesSettingsDlg, CDialogEx)
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_BUTTON_SAVERULES, &CRulesSettingsDlg::OnBnClickedButtonSaverules)
	ON_BN_CLICKED(IDC_RADIO_TOPBONUS_ON, &CRulesSettingsDlg::OnBnClickedRadioTopbonusOn)
	ON_BN_CLICKED(IDC_RADIO_TOPBONUS_OFF, &CRulesSettingsDlg::OnBnClickedRadioTopbonusOff)
END_MESSAGE_MAP()


// CRulesSettingsDlg ��Ϣ�������

/*
int CRulesSettingsDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  �ڴ������ר�õĴ�������
	UpdateData(false);
	
	CheckRadioButton(IDC_RADIO_EASTONLY, IDC_RADIO_FULLMATCH, IDC_RADIO_EASTONLY + MatchSystem - 1);
	CheckRadioButton(IDC_RADIO_TWOFU, IDC_RADIO_FOURFU, IDC_RADIO_TWOFU + DoubleWindTile);
	CheckRadioButton(IDC_RADIO_RinShan_PAYALL, IDC_RADIO_RinShan_NOTPAYALL, IDC_RADIO_RinShan_NOTPAYALL - OpenQuadRinShan);
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
	
	CheckRadioButton(IDC_RADIO_EASTONLY, IDC_RADIO_FULLMATCH, IDC_RADIO_EASTONLY + MatchFormat - 1);
	CheckRadioButton(IDC_RADIO_TWOFU, IDC_RADIO_FOURFU, IDC_RADIO_TWOFU + DoubleWindTile);
	CheckRadioButton(IDC_RADIO_RINSHAN_PAYALL, IDC_RADIO_RINSHAN_NOTPAYALL, IDC_RADIO_RINSHAN_NOTPAYALL - OpenQuadRinShan);
	CheckRadioButton(IDC_RADIO_DRAWMANGAN_ON, IDC_RADIO_DRAWMANGAN_OFF, IDC_RADIO_DRAWMANGAN_OFF - DrawMangan);
	CheckRadioButton(IDC_RADIO_ORPHANSDRAW_ON, IDC_RADIO_ORPHANSDRAW_OFF, IDC_RADIO_ORPHANSDRAW_OFF - NineOrphans);
	CheckRadioButton(IDC_RADIO_NEGATIVEMARKSEND_ON, IDC_RADIO_NEGATIVEMARKSEND_OFF, IDC_RADIO_NEGATIVEMARKSEND_OFF - NegativeEnd);
	CheckRadioButton(IDC_RADIO_CHANANKAN_ON, IDC_RADIO_CHANANKAN_OFF, IDC_RADIO_CHANANKAN_OFF - ChanAnKan);
	CheckRadioButton(IDC_RADIO_TOPBONUS_ON, IDC_RADIO_TOPBONUS_OFF, IDC_RADIO_TOPBONUS_OFF - TopBonus);
	m_EditReturnMarks.EnableWindow(TopBonus);
	
	return TRUE;  // return TRUE unless you set the focus to a control

	// �쳣: OCX ����ҳӦ���� FALSE
}


void CRulesSettingsDlg::OnBnClickedButtonSaverules()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	int Response = MessageBoxW(_T("�˲��������¿�ʼ��Ϸ��ȷ�ϱ��棿"), _T("��ʾ"), MB_YESNO);
	if (Response == IDYES)
	{
		UpdateData(true);
		MatchFormat = GetCheckedRadioButton(IDC_RADIO_EASTONLY, IDC_RADIO_FULLMATCH) - IDC_RADIO_EASTONLY + 1;
		DoubleWindTile = GetCheckedRadioButton(IDC_RADIO_TWOFU, IDC_RADIO_FOURFU) - IDC_RADIO_TWOFU;
		OpenQuadRinShan = GetCheckedRadioButton(IDC_RADIO_RINSHAN_PAYALL, IDC_RADIO_RINSHAN_NOTPAYALL) == IDC_RADIO_RINSHAN_PAYALL;
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_EditReturnMarks.EnableWindow(true);
}


void CRulesSettingsDlg::OnBnClickedRadioTopbonusOff()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_EditReturnMarks.EnableWindow(false);
}
