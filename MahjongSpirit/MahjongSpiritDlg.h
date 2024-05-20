
// MahjongSpiritDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"
#include "pai.h"
#include "tiles.h"
#include "structs.h"
#include "functions.h"
#include "DrawFunc.h"
#include "MahjongRobot.h"
#include "RulesSettingsDlg.h"


// CMahjongSpiritDlg �Ի���
class CMahjongSpiritDlg : public CDialogEx
{
// ����
public:
	CMahjongSpiritDlg(CWnd* pParent = NULL);	// ��׼���캯��
	~CMahjongSpiritDlg();
// �Ի�������
	enum { IDD = IDD_MAHJONGSPIRIT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	
public:
	afx_msg void OnBnClickedStart();
	afx_msg void OnStnClickedBtnstart();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);

	CStatic m_coverpic;				//����ͼƬ
	LPPICTURE m_pPicture;			//ͼƬ
	int dlg_width;					//���ڿ���
	int dlg_height;					//���ڸ߶�
	bool ifshowcover;				//�Ƿ���ʾ����
	
	unsigned char startbtn_alpha;		//��ʼ��ť͸����
	bool startbtn_alpha_up;				//��ʼ��ť͸���ȱ仯����
	bool ifshowwelcome;					//�Ƿ���ʾ��ӭ����
	unsigned char hintpic_alpha;		//��ʾͼƬ͸����
	bool hintpic_alpha_up;				//��ʾͼƬ��͸���ȱ仯����
	bool ready_enter;					//�Ƿ���Խ�����Ϸ
	bool login_succeed;					//��¼�Ƿ�ɹ�
	int enter_rate;						//�������
	bool mahjong_start;					//�ƾ��Ƿ�ʼ
	int showtilenum;					//���ƽ׶��ѻ�ȡ��������
	
	
	int tile_selected;			//��ǰ���ͣ����λ��
	bool get_all_tiles;			//�Ƿ��Ѿ��������
	int tile_out;				//���γ���
	bool tileout_completed;
	direction MySeat;
	BYTE WinPaiStatus;

	tiles temptiles[4];
	pai mypai[4];				//�ļ���
	bool PaiVisible[4];			//�ļ��ƵĿɼ���
	bool AllVisible;			
	pai NewTile;				//��������
	bool analysis_completed;	//�����Ƿ����
	bool me_analysis_completed;	//�Լҷ����Ƿ����
	singletile should_tile;		//����Ӧ�������
	tiles remaintiles;			//����
	matchinfo match_info;		//�ƾ���Ϣ
	hupaiinfo *pHupaiInfo;		//��ǰ������Ϣ��ָ��
	pai tempmypai;				//�Լ���
	tiles isee_remaintiles;		//�Լ���������
	bool Choosing;				//�Ƿ���ѡ��״̬
	UINT ChooseFlags;		//ѡ������ѡ��
	POINT ChooseColumnPos;		//ѡ����λ��
	POINT pai_pos[4];			//��λ��
	POINT paihe_pos[4];			//�ƺ�λ��

	int FinalChoice;			//����ѡ��
	bool frame_start;			//�Ƿ��ؿ�һ��

	MahjongRobot* pTempRobot;	//��Ҫ���з����ĵ���
	MahjongRobot* AllRobot;		//���еĻ�����ָ��
	fuluinfo all_fulu[4];		//ȫ�帱¶
	paihe all_paihe[4];			//ȫ���ƺ�
	robotresponse TempResponse;	//������Ӧ
	MahjongRobot* pMyRobot;		//�Լһ���
	robotresponse MyResponse;	//�Լ���Ӧ
	bool OneRoundForesee;		//һѲ��֪ģʽ
	bool MeAutoMode;			//�й�ģʽ
	bool DebugMode;				//����ģʽ
	bool DrawMode;				//��ͼģʽ
	

	bool IfShowChooseColumn;	//�Ƿ���ʾѡ����	
	bool IfShowChiFuluHint;		//�Ƿ���ʾ������ʾ
	bool IfShowTenpaiHint;		//�Ƿ���ʾ������ʾ

	bool IfShowCutScenes;		//�Ƿ���ʾ��������
	int CutRate;				//����������ʾ����
	int CutScenesDirection;		//����������ʾ����
	enum {None, ScenesEnd, ShowMatchInfo, ShowAction, ShowPointsChange} CutScenesType;		//�������������
	struct {direction ActionDirection; BYTE ActionType;} SpecialAction;

	POINT MousePoint;		//��ǰ���λ��

	unsigned int ThisSeed;		//��ǰ��Ϸ�����������

	CRulesSettingsDlg* pRulesSettingsDlg;	//�����趨���ڵ�ָ��

private:
	HACCEL hAccKey[4];
	/*
	�ĸ���ݼ���
	Ctrl + V��ȫԱ���ӻ�
	Ctrl + F��Ԥ֪
	Ctrl + M������
	Ctrl + D����ͼģʽ
	*/
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
//	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnBnClickedChangeVisible();
	
	afx_msg void OnBnClickedButtonForesee();
	afx_msg void OnBnClickedButtonManaged();
	afx_msg void OnBnClickedButtonDrawmode();
	afx_msg void OnGamesettingRules();
	afx_msg void OnGamesettingMarks();
	afx_msg void OnGamesettingReset();
};