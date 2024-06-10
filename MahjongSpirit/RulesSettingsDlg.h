#pragma once
#include "afxwin.h"


// CRulesSettingsDlg �Ի���

class CRulesSettingsDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CRulesSettingsDlg)

public:
	CRulesSettingsDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CRulesSettingsDlg();

// �Ի�������
	enum { IDD = IDD_RULESSETTINGSDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	int MatchFormat;		//�����趨��0������ս����ѭ����1������ս��2����ׯ��3��ȫׯ
	bool DoubleWindTile;	//������ӷ�����trueΪ4����falseΪ2��
	bool OpenQuadRenShan;	//���������Ͽ�������
	bool DrawMangan;		//��������
	bool NineOrphans;		//���־�������
	bool NegativeEnd;		//���ɣ����ֽ�����
	bool ChanAnKan;			//ʮ����������
	
	bool TopBonus;			//ͷ����

	//afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	int BackMarks;
	CEdit m_EditReturnMarks;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonSaverules();
	afx_msg void OnBnClickedRadioTopbonusOn();
	afx_msg void OnBnClickedRadioTopbonusOff();
	int RankHorse_1;
	int RankHorse_2;
	int OriginPoints;
};
