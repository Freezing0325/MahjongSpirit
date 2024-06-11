
// MahjongSpiritDlg.h : 头文件
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


// CMahjongSpiritDlg 对话框
class CMahjongSpiritDlg : public CDialogEx
{
// 构造
public:
	CMahjongSpiritDlg(CWnd* pParent = NULL);	// 标准构造函数
	~CMahjongSpiritDlg();
// 对话框数据
	enum { IDD = IDD_MAHJONGSPIRIT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	
public:
//	afx_msg void OnBnClickedStart();
//	afx_msg void OnStnClickedBtnstart();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);

//	CStatic m_coverpic;
	//封面图片
	LPPICTURE m_pPicture;			//图片
	int dlg_width;					//窗口初始宽度
	int dlg_height;					//窗口初始高度
	int NowDlgWidth;				//显示窗口当前宽度
	int NowDlgHeight;				//显示窗口当前高度
	int RealDlgWidth;				//窗口实际宽度
	int RealDlgHeight;				//窗口实际宽度

	enum {ShowNone, ShowCover, ShowWelcome, ReadyEnter, MahjongStart} ShowType;		//主界面显示的信息

	RECT StartBtn;						//开始按钮的位置信息
	unsigned char startbtn_alpha;		//开始按钮透明度
	bool startbtn_alpha_up;				//开始按钮透明度变化趋势
	unsigned char hintpic_alpha;		//提示图片透明度
	bool hintpic_alpha_up;				//提示图片的透明度变化趋势
	bool login_succeed;					//登录是否成功
	int enter_rate;						//载入进度
	int showtilenum;					//配牌阶段已获取的牌数量
	bool DlgResize;
	
	
	int tile_selected;			//当前鼠标停留的位置
	bool get_all_tiles;			//是否已经完成配牌
	int tile_out;				//本次出牌
	bool tileout_completed;
	direction MySeat;
	BYTE WinPaiStatus;

	tiles temptiles[4];
	pai mypai[4];				//四家牌
	bool PaiVisible[4];			//四家牌的可见性
	bool AllVisible;			
	pai NewTile;				//新摸的牌
	bool analysis_completed;	//分析是否结束
	bool me_analysis_completed;	//自家分析是否结束
	singletile should_tile;		//分析应当打的牌
	tiles remaintiles;			//余牌
	matchinfo match_info;		//牌局信息
	hupaiinfo *pHupaiInfo;		//当前和牌信息的指针
	pai tempmypai;				//自家牌
	tiles isee_remaintiles;		//自家所见余牌
	bool Choosing;				//是否处于选择状态
	UINT ChooseFlags;		//选择栏的选项
	POINT ChooseColumnPos;		//选择栏位置
	POINT pai_pos[4];			//牌位置
	POINT paihe_pos[4];			//牌河位置
	RECT MarksBoxSize;			//分数框的大小参数
	int Marks[10][4];			//分数
	int MatchNum;				//已经结束的局数

	int OriginPoints;			//初始点数
	int RankHorse_1,
		RankHorse_2;			//顺位马
	bool TopBonus;			//头名赏
	int BackPoints;				//返点
	BYTE MatchFormat;			//赛制
	bool DrawMangan;		//流局满贯
	bool NineOrphans;		//九种九牌流局
	bool NegativeEnd;		//击飞（负分结束）
	bool ChanAnKan;			//十三幺抢暗杠

	int FinalChoice;			//最终选项
	bool frame_start;			//是否重开一局
	bool RobotInfoInitialize;	//机器人是否完成初始化

	MahjongRobot* pTempRobot;	//需要进行分析的电脑
	MahjongRobot* AllRobot;		//所有的机器人指针
	fuluinfo all_fulu[4];		//全体副露
	paihe all_paihe[4];			//全体牌河
	robotresponse TempResponse;	//电脑响应
	MahjongRobot* pMyRobot;		//自家机器
	robotresponse MyResponse;	//自家响应
	bool OneRoundForesee;		//一巡先知模式
	bool MeAutoMode;			//托管模式
	bool DebugMode;				//调试模式
	bool DrawMode;				//画图模式
	bool NoNeedLogin;			//无需登录
	const singletile* MyTiles;	//初始配牌（调试用）
	bool AllOrphans;			//是否全部摸幺九牌（调试用）
	

	bool IfShowChooseColumn;	//是否显示选择栏	
	bool IfShowChiFuluHint;		//是否显示吃牌提示
	bool IfShowTenpaiHint;		//是否显示听牌提示
	bool IfShowMarks;			//是否显示得分情况

	bool IfShowCutScenes;		//是否显示过场动画
	int CutRate;				//过场动画显示进度
	int CutScenesDirection;		//过场动画显示方向
	enum {None, ScenesEnd, ShowMatchInfo, ShowAction, ShowPointsChange, ShowMatchEnd} CutScenesType;		//过场动画的类别
	struct {direction ActionDirection; BYTE ActionType;} SpecialAction;

	POINT MousePoint;		//当前鼠标位置

	unsigned int ThisSeed;		//当前游戏随机数的种子

	CRulesSettingsDlg* pRulesSettingsDlg;	//规则设定窗口的指针

private:
	HACCEL hAccKey[4];
	/*
	四个快捷键：
	Ctrl + V：全员可视化
	Ctrl + F：预知
	Ctrl + M：代打
	Ctrl + D：画图模式
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
	void UpdateRules(bool FromMain);
	afx_msg void OnSize(UINT nType, int cx, int cy);
//	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};
