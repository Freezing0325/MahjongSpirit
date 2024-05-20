
// MahjongSpiritDlg.cpp : 实现文件
//20230128_2037：开两个杠有问题，立直之前有杠可开但不开，立直之后仍有开杠选项。
//20230129_1546：听牌后显示卡顿，判断听牌可以优化。
//20230129_2252：听牌卡顿问题没找到解决方法。解决了鼠标不拖动时不刷新的问题。
//麻将class的小目标：整出一个只需要一部分接口的机器。输入当前四家的牌河、副露，局势的总体情况，输入出牌者和所出的牌，直接刷新状态。
//20230204_2320：MahjongRobot已初步构建，接下来需要完善pai类的print_tiles（上家与下家，seat=1与seat=3）的行为与print_river（其他各家）的行为，尽快进行测试。
//20230205_1851：print_tiles大致逻辑已构建完毕，但SpinImage函数性能极低，且麻将牌比例有一定错误，需要细节。
//20230205_2357：print_river大致逻辑已构建完毕。
//20230207_1129：print_tiles和print_river基本测试完毕，后期可以调整位置。麻将牌加入缓存机制，但在后期仍明显较慢，需要改进print_river机制。出现m_hBitmap != 0的错误，但触发原因不明。
//20230207_2243：基本机制建立完毕，副露显示问题较大，自身吃碰杠和他人未建立，他家点和与自摸机制未建立，显示、刷新任重道远，但可以打了。还新出现数组溢出的问题。
//20230213_1134：自己没法响应他人牌（已解决），他家不加杠（已解决），听牌时乱副露（已解决），副露显示位置不对（已解决，需要今后扩大桌面），不明原因自摸切立直（已解决），流局机制未建立。
//20230214_1755：只差自己抢他人杠还未设定。
//20230219_0203：牌河打印机制优化（引入缓存），但在自己有过选项后，其他人容易出现摸牌闪现情况（已解决）。
//20230223_0004：多线程处理有一定问题。
//20230224_0053：多线程处理大多数情况下反而降低效率，可以先行判断能否响应。
//20230513_2240：心血来潮，加上了过场小动画的函数。
//20230715_0116：各种动作均有了动画效果，美术性差不多得了:D，他家之间的吃牌碰牌会出现“幻影牌”的BUG，已知是在动画后mypai=robot.getpai时两者不同的缘故，需要细查为什么robot出牌后牌还在（已解决）；可以的话还是希望解决一下听牌后听牌提示于出牌前后闪现的问题（已解决）；有的时候他家吃完牌就无响应了（暂时没出现了）。截止现在一共7536行代码。
//20230716_0202：前一天的各种问题已经解决，如今体验较好，下一步：记分板！！！
//20230717_0053：解决了里宝牌闪现的问题，代打时无法正常立直的问题，把桌面图片改成代码绘图，增加了绘图辅助模式，增加了玩家名和分数的显示，大致写了分数更改的框架，具体没实现

#include "stdafx.h"

#include "MahjongSpirit.h"
#include "MahjongSpiritDlg.h"
#include "LoginDlg.h"
#include "afxdialogex.h"
#include "atlimage.h"
#include "resource.h"
#include <time.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define HIMETRIC_INCH 2540
// CMahjongSpiritDlg 对话框

UINT AnalysisThreadProc(LPVOID pParam);
UINT GetResponseThreadProc(LPVOID pParam);
UINT GetMyResponseThreadProc(LPVOID pParam);

char painumname[9][3] = {"一", "二", "三", "四", "五", "六", "七", "八", "九"};
char paitypename[3][3] = {"万", "筒", "条"};
char painame[4][9][5] = {{}, {}, {}, {"东风", "南风", "西风", "北风", "红中", "白板", "发财"}};
char seatname[4][3] = {"东", "南", "西", "北"};
char zipainame_simple[7][3] = {"东", "南", "西", "北", "中", "白", "发"};
char fanzhong_1[][9] = {"立直", "一发", "门清自摸", "平和", "断幺九", "一杯口", "役牌", "场风", "自风", "岭上开花", "海底捞月", "河底捞鱼", "抢杠", ""};
char fanzhong_2[][11] = {"双立直", "对对和", "一气通贯", "混全带幺九", "小三元", "七对子", "三色同顺", "三色同刻", "混幺九", "三杠子", "三暗刻", ""};
char fanzhong_3[][11] = {"混一色", "两杯口", "纯全带幺九", ""};
char fanzhong_6[][7] = {"清一色", ""};
char fanzhong_13[][9] = {"十三幺", "四暗刻", "大三元", "清幺九", "九莲宝灯", "大四喜", "小四喜", "四杠子", "绿一色", "字一色", "地和", "天和", ""};
char fanzhong_sub1[][11] = {"一气通贯", "混全带幺九", "三色同顺", "混一色", "纯全带幺九", "清一色", ""};
char playername[][20] = {"极地严寒", "电脑一号", "电脑二号", "电脑三号"};

const singletile defaulttile(-1, -1);
const fuluinfo menqianqing = fuluinfo();
const fuluinfo yakupaiexample = fuluinfo(1, &fulugroup(ke, 3, 4));
const fulugroup YakupaiFulu = fulugroup(ke, 3, 4);
const matchinfo default_match = {noneed, noneed, 0, 0, {defaulttile}, 0};

CMahjongSpiritDlg::CMahjongSpiritDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMahjongSpiritDlg::IDD, pParent)
	, dlg_width(0)
	, dlg_height(0)
	, ifshowcover(false)
	, startbtn_alpha(0)
	, startbtn_alpha_up(false)
	, ifshowwelcome(false)
	, hintpic_alpha(0)
	, ready_enter(false)
	, login_succeed(false)
	, enter_rate(0)
	, mahjong_start(false)
	, showtilenum(0)
	, hintpic_alpha_up(false)
	, tile_selected(0)
	, get_all_tiles(false)
	, tile_out(0)
	, analysis_completed(false)
	, Choosing(false)
	, FinalChoice(0)
	, OneRoundForesee(false)
	, IfShowCutScenes(false)
	, CutRate(0)
	, pRulesSettingsDlg(nullptr)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pPicture = NULL;
	hAccKey[0] = LoadAccelerators(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_ACCELERATOR1));
	hAccKey[1] = LoadAccelerators(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_ACCELERATOR2));
	hAccKey[2] = LoadAccelerators(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_ACCELERATOR3));
	hAccKey[3] = LoadAccelerators(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_ACCELERATOR4));
}

CMahjongSpiritDlg::~CMahjongSpiritDlg()
{
	if (m_pPicture)
		m_pPicture->Release();
	if (pRulesSettingsDlg)
		delete pRulesSettingsDlg;
}




void CMahjongSpiritDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COVERPIC, m_coverpic);
	//  DDX_Control(pDX, ID_BTNSTART, m_startpic);
}

BEGIN_MESSAGE_MAP(CMahjongSpiritDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_START, &CMahjongSpiritDlg::OnBnClickedStart)
	ON_STN_CLICKED(ID_BTNSTART, &CMahjongSpiritDlg::OnStnClickedBtnstart)
	ON_WM_TIMER()
	ON_WM_ERASEBKGND()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
//	ON_WM_CHAR()
	ON_BN_CLICKED(IDC_CHANGE_VISIBLE, &CMahjongSpiritDlg::OnBnClickedChangeVisible)
	ON_BN_CLICKED(IDC_BUTTON_FORESEE, &CMahjongSpiritDlg::OnBnClickedButtonForesee)
	ON_BN_CLICKED(IDC_BUTTON_MANAGED, &CMahjongSpiritDlg::OnBnClickedButtonManaged)
	ON_BN_CLICKED(IDC_BUTTON_DRAWMODE, &CMahjongSpiritDlg::OnBnClickedButtonDrawmode)
	ON_COMMAND(ID_GAMESETTING_RULES, &CMahjongSpiritDlg::OnGamesettingRules)
	ON_COMMAND(ID_GAMESETTING_MARKS, &CMahjongSpiritDlg::OnGamesettingMarks)
	ON_COMMAND(ID_GAMESETTING_RESET, &CMahjongSpiritDlg::OnGamesettingReset)
END_MESSAGE_MAP()


// CMahjongSpiritDlg 消息处理程序

BOOL CMahjongSpiritDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标
	
	// TODO: 在此添加额外的初始化代码
	//AllocConsole();
	RECT client_rect;
	GetClientRect(&client_rect);
	dlg_width = client_rect.right - client_rect.left;
	dlg_height = client_rect.bottom - client_rect.top;
	ifshowcover = true;
	startbtn_alpha = 255;
	tile_selected = -1;
	tile_out = -1;
	MeAutoMode = false;
	AllVisible = false;
	DebugMode = false;
	DrawMode = false;
	match_info.frame_status = 0;
	match_info.match_wind = east;
	match_info.game_num = 0;
	match_info.thisdealer_num = 0;
	match_info.RiichiBarSum = 0;
	frame_start = true;
	IfShowChooseColumn = false;
	IfShowChiFuluHint = false;
	IfShowTenpaiHint = false;
	MousePoint.x = MousePoint.y = 0;
	CutScenesType = None;
	SetTimer(1, 100, NULL);
	AllRobot = nullptr;
	pHupaiInfo = nullptr;
	pRulesSettingsDlg = new CRulesSettingsDlg;
	ThisSeed = (unsigned int)time(NULL);
	//ThisSeed = 1689609958;
	/*
	一些种子：
	1689440075：开局会有很多碰牌
	1689441128：开局混一色
	1689442464：流局，自己有机会立直，南家会立直
	1689442849：第二局有好牌
	1689481232：第一局西家有长考，第二局有人和牌拦截吃牌
	*/
	srand(ThisSeed);
	match_info.this_dealer = direction(rand() % 4);
	
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 获取麻将桌面的图片
void GetTableImage(CImage& TableImage, int dlg_width, int dlg_height)
{
	GetGradientColorImage(TableImage, RGB(111, 181, 63), RGB(61, 104, 33), dlg_width, dlg_height);
	HDC hTableDC = TableImage.GetDC();
	HPEN hp = CreatePen(PS_SOLID, 3, 0);(HPEN)GetStockObject(BLACK_PEN);
	SelectObject(hTableDC, hp);
	const int AngleSquareLen = 27;
	const int OutFrameLen = 230;
	const int InnerFrameLen = 70;
	const int LeanFrameLen = (OutFrameLen - InnerFrameLen) / 2;
	const POINT StartPos = {480, 190};

	POINT ThisPos = StartPos;
	MoveToEx(hTableDC, ThisPos.x, ThisPos.y, NULL);
	ThisPos.x += OutFrameLen;
	LineTo(hTableDC, ThisPos.x, ThisPos.y);
	ThisPos.y += AngleSquareLen;
	LineTo(hTableDC, ThisPos.x, ThisPos.y);
	ThisPos.x += AngleSquareLen;
	LineTo(hTableDC, ThisPos.x, ThisPos.y);
	ThisPos.y += OutFrameLen;
	LineTo(hTableDC, ThisPos.x, ThisPos.y);
	ThisPos.x -= AngleSquareLen;
	LineTo(hTableDC, ThisPos.x, ThisPos.y);
	ThisPos.y += AngleSquareLen;
	LineTo(hTableDC, ThisPos.x, ThisPos.y);
	ThisPos.x -= OutFrameLen;
	LineTo(hTableDC, ThisPos.x, ThisPos.y);
	ThisPos.y -= AngleSquareLen;
	LineTo(hTableDC, ThisPos.x, ThisPos.y);
	ThisPos.x -= AngleSquareLen;
	LineTo(hTableDC, ThisPos.x, ThisPos.y);
	ThisPos.y -= OutFrameLen;
	LineTo(hTableDC, ThisPos.x, ThisPos.y);
	ThisPos.x += AngleSquareLen;
	LineTo(hTableDC, ThisPos.x, ThisPos.y);
	ThisPos.y -= AngleSquareLen;
	LineTo(hTableDC, ThisPos.x, ThisPos.y);
	ThisPos.y += AngleSquareLen;
	MoveToEx(hTableDC, ThisPos.x, ThisPos.y, NULL);
	ThisPos.x += LeanFrameLen;
	ThisPos.y += LeanFrameLen;
	LineTo(hTableDC, ThisPos.x, ThisPos.y);
	ThisPos.x += InnerFrameLen;
	LineTo(hTableDC, ThisPos.x, ThisPos.y);
	ThisPos.x += LeanFrameLen;
	ThisPos.y -= LeanFrameLen;
	LineTo(hTableDC, ThisPos.x, ThisPos.y);
	ThisPos.x -= LeanFrameLen;
	ThisPos.y += LeanFrameLen;
	MoveToEx(hTableDC, ThisPos.x, ThisPos.y, NULL);
	ThisPos.y += InnerFrameLen;
	LineTo(hTableDC, ThisPos.x, ThisPos.y);
	ThisPos.x += LeanFrameLen;
	ThisPos.y += LeanFrameLen;
	LineTo(hTableDC, ThisPos.x, ThisPos.y);
	ThisPos.x -= LeanFrameLen;
	ThisPos.y -= LeanFrameLen;
	MoveToEx(hTableDC, ThisPos.x, ThisPos.y, NULL);
	ThisPos.x -= InnerFrameLen;
	LineTo(hTableDC, ThisPos.x, ThisPos.y);
	ThisPos.x -= LeanFrameLen;
	ThisPos.y += LeanFrameLen;
	LineTo(hTableDC, ThisPos.x, ThisPos.y);
	ThisPos.x += LeanFrameLen;
	ThisPos.y -= LeanFrameLen;
	MoveToEx(hTableDC, ThisPos.x, ThisPos.y, NULL);
	ThisPos.y -= InnerFrameLen;
	LineTo(hTableDC, ThisPos.x, ThisPos.y);

	DeleteObject(hp);
	TableImage.ReleaseDC();
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CMahjongSpiritDlg::OnPaint()
{
	//Invalidate(false);
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
	
	if (ifshowcover)
	{
		//CImage cover_image;				//使用图片类
		RECT rect = {0};

		//加载封面
		static bool ifcover_loaded = false;
		if (!ifcover_loaded)
		{
			CImage image;
			LoadImageFromResource(image, IDB_PNG_COVER, _T("PNG"));
			//image.Load(_T("封面.png")); 
			rect.right = image.GetWidth();
			rect.bottom = image.GetHeight();
			CDC* pDC = GetDlgItem(IDC_COVERPIC)->GetDC();
			m_coverpic.SetWindowPos(NULL, 0, 0, rect.right, rect.bottom, SWP_NOZORDER);
			image.Draw(pDC->m_hDC, rect);
			image.Destroy();
			ReleaseDC(pDC);
			ifcover_loaded = true;
		}

		//加载开始按钮
		CString start_hint = _T("—— start ——");
		CWnd* m_startpic = GetDlgItem(ID_BTNSTART);
		POINT startbtn_pos = {450, 530};
		static CImage start_textimage, start_backimage;
		ShowTransparentText(m_startpic, GetDlgItem(IDC_COVERPIC), start_hint, _T("Times New Roman"), 48, RGB(255, 174, 201), startbtn_pos, startbtn_alpha, start_textimage, start_backimage);
		if (ifshowwelcome)
		{
			CString welcome_hint = _T("欢迎回来，极地严寒");
			CWnd* m_hintpic = GetDlgItem(IDC_HINTPIC);
			POINT hintinfo_pos = {420, 600};
			static CImage welcome_textimage, welcome_backimage;
			ShowTransparentText(m_hintpic, GetDlgItem(IDC_COVERPIC), welcome_hint, _T("华康勘亭流W9(P)"), 48, RGB(255, 174, 201), hintinfo_pos, hintpic_alpha, welcome_textimage, welcome_backimage);
		}
		
	}

	if (ready_enter)
	{
		static CImage lastSurface, nextSurface, nextSurface_copy;
		HDC hNextDC;
		if (nextSurface.IsNull())
		{
			//LoadImageFromResource(nextSurface, IDB_PNG_TABLE, _T("PNG"));
			//nextSurface.Load(_T("麻将桌.png"));
			GetTableImage(nextSurface, dlg_width, dlg_width);
		}
		ChangeSurface(GetDlgItem(IDC_COVERPIC), lastSurface, nextSurface, enter_rate);
		if (enter_rate == 100)
		{
			frame_start = true;
			if (!lastSurface.IsNull())
				lastSurface.Destroy();
			if (!nextSurface.IsNull())
				nextSurface.Destroy();
		}
	}
	//双缓冲技术
	CRect rc;				
	GetClientRect(&rc);
	int nWidth = rc.Width();  
	int nHeight = rc.Height();   
	CDC *pCDC = GetDC();		//定义设备上下文
		
	CDC MemDC;					//定义一个内存显示设备对象
	CBitmap MemBitmap;			//定义一个位图对象
	//建立与屏幕显示兼容的内存显示设备
	MemDC.CreateCompatibleDC(pCDC);
	//建立一个与屏幕显示兼容的位图，位图的大小可选用窗口客户区的大小
	MemBitmap.CreateCompatibleBitmap(pCDC, nWidth, nHeight);
	//将位图选入到内存显示设备中，只有选入了位图的内存显示设备才有地方绘图，画到指定的位图上
	CBitmap *pOldBitmap = MemDC.SelectObject(&MemBitmap);
	//MemDC.FillSolidRect(0,0,nWidth,nHeight,RGB(255,255,255));
	HDC hdc = MemDC.GetSafeHdc();

	if (mahjong_start)
	{
		//加载牌桌
		static CImage tableimage;
		if (tableimage.IsNull())
		{
			//LoadImageFromResource(tableimage, IDB_PNG_TABLE, _T("PNG"));
			//tableimage.Load(_T("麻将桌.png"));
			GetTableImage(tableimage, dlg_width, dlg_width);
		}
		tableimage.BitBlt(hdc, 0, 0, SRCCOPY);
		POINT hintbox_pos = {0, 0};
		
		pai_pos[0].x = 260;
		pai_pos[0].y = 570;
		pai_pos[1].x = 1125;
		pai_pos[1].y = 600;
		pai_pos[2].x = 940;
		pai_pos[2].y = 90;
		pai_pos[3].x = 75;
		pai_pos[3].y = 70;
		paihe_pos[0].x = 437;
		paihe_pos[0].y = 375;
		paihe_pos[1].x = 800;
		paihe_pos[1].y = 475;
		paihe_pos[2].x = 742;
		paihe_pos[2].y = 250;
		paihe_pos[3].x = 400;
		paihe_pos[3].y = 200;
		ChooseColumnPos.x = 600;
		ChooseColumnPos.y = 450;
		if (!frame_start)
		{
			//-----------------------------------显示宝牌及其它场上标志物-------------------------------------
			{
				const POINT dora_pos = {463, 260};
				const POINT dealer_label_pos[4] = {{260, 530}, {1020, 500}, {840, 130}, {100, 100}};
				const POINT RobotInfoPos[4] = {{350, 520}, {1010, 430}, {930, 120}, {90, 145}};
				POINT ThisDealerLabelPos = dealer_label_pos[match_info.this_dealer];

				static BYTE RollStatus = 0;		//翻转宝牌的进度
				if (match_info.frame_status == FRAME_WIN && ((WinPaiStatus & PAI_STATUS_RIICHI) != 0))
				{
					match_info.print_dora(hdc, dora_pos, RollStatus);
					if (RollStatus < 2)
					{
						RollStatus ++;
						int doraimage_width = 5 * (TILE_STRAIGHT_WIDTH + TILE_SPACE) - TILE_SPACE;
						int doraimage_height = TILE_STRAIGHT_HEIGHT + TILE_STRAIGHT_SURFACE_HEIGHT;
						RECT DoraRect = {dora_pos.x, dora_pos.y, dora_pos.x + doraimage_width, dora_pos.y + doraimage_height};
						InvalidateRect(&DoraRect, false);
					}
				}
				else
				{
					match_info.print_dora(hdc, dora_pos);
					RollStatus = 0;
				}

				//-----------------------------显示庄家-------------------------------------
				CString dealer_label = _T("Dealer");
				HFONT hf = CreateMyFont(_T("华文新魏"), 24);
				SelectObject(hdc, hf);
				SetBkMode(hdc, TRANSPARENT);
				SetTextColor(hdc, RGB(255, 174, 201));
				hintbox_pos.x = ThisDealerLabelPos.x - 10;
				hintbox_pos.y = ThisDealerLabelPos.y - 10;
				ShowHintBox(hdc, hintbox_pos, 90, 45, RGB(255, 0, 0));
				TextOutW(hdc, ThisDealerLabelPos.x, ThisDealerLabelPos.y, dealer_label, lstrlenW(dealer_label));
				DeleteObject(hf);
				
				//-----------------------------显示分数与名称-------------------------------------
				if (AllRobot)
				{
					hf = CreateMyFont(_T("华文中宋"), 24);
					SelectObject(hdc, hf);
					SetBkMode(hdc, TRANSPARENT);
					SetTextColor(hdc, RGB(255, 255, 255));
					for (int i = 0; i < 4; i++)
					{
						CString RobotInfoString = AllRobot[i].GetRobotName();
						RobotInfoString.AppendFormat(_T("\n%6d"), AllRobot[i].GetPoints());
						RECT RobotInfoRect = {RobotInfoPos[i].x, RobotInfoPos[i].y, RobotInfoPos[i].x + 100, RobotInfoPos[i].y + 80};
						DrawTextW(hdc, RobotInfoString, lstrlenW(RobotInfoString), &RobotInfoRect, DT_TOP | DT_LEFT);
						//TextOutW(hdc, RobotInfoPos[i].x, RobotInfoPos[i].y, RobotInfoString, lstrlenW(RobotInfoString));
					}
					DeleteObject(hf);
				}
			}
			//-----------------------------------配牌阶段-------------------------------------
			if (!get_all_tiles)
			{
				for (int i = 0; i < (showtilenum + 3) / 4; i++)
					if (temptiles[i].get_tilesum() > 0)
						temptiles[i].print_tiles(hdc, pai_pos[0].x + (4 * i) * (TILE_STRAIGHT_WIDTH + TILE_SPACE), pai_pos[0].y);
			}
			//----------------------------------配牌完成，开始打牌----------------------------------
			else
			{
				//牌面打印
				if (tileout_completed)
				{
					//mypai[match_info.active_direction] = robot[match_info.active_direction].get_pai();
					for (int seat = 0; seat < 4; seat++)
					{
						mypai[seat].print_tiles(hdc, pai_pos[seat], seat, PaiVisible[seat]/*seat != 0*/, tile_selected);
						mypai[seat].print_river(hdc, paihe_pos[seat], seat);
					}
				}
				else
				{
					direction NewTileDirection = match_info.active_direction;
					bool NewTile_visible = PaiVisible[NewTileDirection];
					POINT NewTile_pos = pai_pos[NewTileDirection];
					switch (NewTileDirection)
					{
					case east:
						NewTile_pos.x += mypai[0].get_tilesum() * (TILE_STRAIGHT_WIDTH + TILE_SPACE) + TILE_NEWSPACE;
						break;
					case south:
						NewTile_pos.y -= (mypai[1].get_tilesum()) * (TILE_ACROSS_SURFACE_HEIGHT + TILE_SPACE) + TILE_NEWSPACE;
						break;
					case west:
						NewTile_pos.x -= mypai[2].get_tilesum() * (TILE_STRAIGHT_WIDTH + TILE_SPACE) + TILE_NEWSPACE;
						break;
					case north:
						NewTile_pos.y += (mypai[3].get_tilesum()) * (TILE_ACROSS_SURFACE_HEIGHT + TILE_SPACE) + TILE_NEWSPACE;
						break;
					default:
						break;
					}
					if (match_info.frame_status != FRAME_NORMAL)
					{
						/*if(match_info.active_direction != match_info.win_direction)
							NewTile.reset_all();*/
						NewTile.remove_tile_status(TILE_RECOMMEND);
					}
					if (match_info.active_direction != north)
						NewTile.print_tiles(hdc, NewTile_pos, NewTileDirection, NewTile_visible, tile_selected - mypai[0].get_tilesum());
					for (int seat = 0; seat < 4; seat++)
					{
						mypai[seat].print_tiles(hdc, pai_pos[seat], seat, PaiVisible[seat]/*seat != 0*/, tile_selected);
						mypai[seat].print_river(hdc, paihe_pos[seat], seat);
					}
					if (match_info.active_direction == north)
						NewTile.print_tiles(hdc, NewTile_pos, NewTileDirection, NewTile_visible, tile_selected - mypai[0].get_tilesum());
				}
				if (IfShowChiFuluHint)
				{
					hintbox_pos.x = pai_pos[0].x;
					hintbox_pos.y = pai_pos[0].y - TILE_STRAIGHT_HEIGHT * 2;
					ShowHintBox(hdc, hintbox_pos, 5 * TILE_STRAIGHT_WIDTH, TILE_STRAIGHT_HEIGHT * 1.5);
					temptiles[0].print_tiles(hdc, hintbox_pos.x + TILE_STRAIGHT_WIDTH, hintbox_pos.y + TILE_STRAIGHT_HEIGHT * 0.25);
				}
				if (IfShowTenpaiHint)
				{
					hintbox_pos.x = pai_pos[0].x;
					hintbox_pos.y = pai_pos[0].y - TILE_STRAIGHT_HEIGHT * 2;
					ShowHintBox(hdc, hintbox_pos, (temptiles[1].get_tilesum() + 2) * TILE_STRAIGHT_WIDTH, TILE_STRAIGHT_HEIGHT * 1.5);
					temptiles[1].print_tiles(hdc, hintbox_pos.x + TILE_STRAIGHT_WIDTH, hintbox_pos.y + TILE_STRAIGHT_HEIGHT * 0.25);
				}
				if (OneRoundForesee && match_info.active_direction == MySeat)
				{
					hintbox_pos.x = pai_pos[0].x + 13 * (TILE_STRAIGHT_WIDTH + TILE_SPACE);
					hintbox_pos.y = pai_pos[0].y - TILE_STRAIGHT_HEIGHT * 1.5;
					ShowHintBox(hdc, hintbox_pos, 3 * TILE_STRAIGHT_WIDTH, TILE_STRAIGHT_HEIGHT * 1.5, RGB(0, 255, 0));
					temptiles[2].print_tiles(hdc, pai_pos[0].x + 14 * (TILE_STRAIGHT_WIDTH + TILE_SPACE), pai_pos[0].y - TILE_STRAIGHT_HEIGHT * 1.25);
				}
				if (IfShowChooseColumn) 
					ShowChooseColumn(hdc, ChooseColumnPos.y, ChooseFlags);
				if (match_info.frame_status != FRAME_NORMAL && CutScenesType == None)
				{
					IfShowChooseColumn = false;
					IfShowTenpaiHint = false;
					CString hint_info;
					int hint_info_lines = 1;
					if (match_info.win_direction == noneed)
					{
						hint_info = _T("流局！");
						if (!AllVisible) for (int seat = 0; seat < 4; PaiVisible[seat++] = mypai[seat].iftingpai());
					}
					else
					{
					// 获取和牌的信息
						if (!pHupaiInfo)
						{
							pHupaiInfo = new hupaiinfo;
							//static hupaiinfo my_hupai_info;
							BYTE HuFlags = 0;
							HuFlags |= HU_RIICHI * ((WinPaiStatus & PAI_STATUS_RIICHI) != 0);
							HuFlags |= HU_IPPATSU * ((WinPaiStatus & PAI_STATUS_IPPATSU) != 0);
							HuFlags |= HU_TENHOU * match_info.tenhou_possible;
							HuFlags |= HU_RINSHAN * ((WinPaiStatus & PAI_STATUS_RINSHAN) != 0);
							HuFlags |= HU_HAITEI * (remaintiles.get_tilesum() == 4);
							HuFlags |= HU_CHANKAN * match_info.chankan_possible;
							HuFlags |= HU_TSUMO * (match_info.win_direction == match_info.active_direction);
							//tempmypai.change_tilenum(match_info.active_tile, 1);
							*pHupaiInfo = tempmypai.ifhu(match_info.active_tile, false, HuFlags, match_info, match_info.win_direction);
						}
						// 生成和牌信息的字符串
						CString my_hupai_string;
						int FanZhongLen = strlen(pHupaiInfo->fanzhong), HuTypeLen = strlen(pHupaiInfo->hutype);
						int length = FanZhongLen + HuTypeLen;
						CHAR* my_hupai_char = new CHAR[length + 1];
						strcpy_s(my_hupai_char, FanZhongLen + 1, pHupaiInfo->fanzhong);
						strcat_s(my_hupai_char, length + 1, pHupaiInfo->hutype);
						WCHAR* my_hupai_wchar = new WCHAR[length + 1];
						memset(my_hupai_wchar, 0, sizeof(WCHAR) * (length + 1));
						MultiByteToWideChar(CP_ACP, 0, my_hupai_char, length, my_hupai_wchar, length);
						for (int i = 0; i < wcslen(my_hupai_wchar); i++)
							if (my_hupai_wchar[i] == ' ')
							{
								my_hupai_wchar[i] = '\n';
								hint_info_lines ++;
							}
						my_hupai_string.Format(_T("%s"), my_hupai_wchar);
						delete[] my_hupai_char;
						delete[] my_hupai_wchar;
						hint_info = my_hupai_string;
					}
					// 输出和牌信息
					
					POINT hintinfo_pos = {120, 100};
					HFONT hf = CreateMyFont(_T("楷体"), 48);
					SelectObject(hdc, hf);
					SetBkMode(hdc, TRANSPARENT);
					SetTextColor(hdc, RGB(255, 174, 201));
					TEXTMETRIC tm;
					GetTextMetricsW(hdc, &tm);
					hintbox_pos.x = 600;
					hintbox_pos.y = 80;
					RECT hintinfo_rect = {620, 100, 620 + 7 * tm.tmMaxCharWidth, 100 + tm.tmHeight * hint_info_lines};
					ShowHintBox(hdc, hintbox_pos, 8 * tm.tmMaxCharWidth, tm.tmHeight * hint_info_lines + 40, 0);
					DrawTextW(hdc, hint_info, lstrlenW(hint_info), &hintinfo_rect, DT_TOP | DT_RIGHT);
					//TextOut(hdc, hintinfo_pos.x, hintinfo_pos.y, hint_info, lstrlenW(hint_info));
					DeleteObject(hf);

					// 摊牌，宣告和牌
					mypai[0].remove_tile_status(TILE_INVALID);
					if (DebugMode)
						frame_start = true;
				}			
			}
		}
		
	}
	if (IfShowCutScenes)
	{
		switch (CutScenesType)
		{
		case CMahjongSpiritDlg::ShowAction:
			{
				//CImage BackImage;
				/*CreateDefaultCutScenesImage(BackImage, CutScenesDirection, dlg_width, dlg_height);
				double LoadRate = 0;
				if (CutRate <= 4) LoadRate = (CutRate - 4) / 4.0;
				if (CutRate >= 96) LoadRate = (CutRate - 96) / 4.0;
				POINT ShowPos = {0, dlg_height / 2 - 100};
				ShowCutScenes(hdc, BackImage, dlg_width, dlg_height, ShowPos, LoadRate, 0);*/
				ShowCutScenes(hdc, CutScenesDirection, CutRate, dlg_width, dlg_height);
				if (CutRate > 4 && CutRate < 96)
				{
					CString ActionInfoText; 
					const wchar_t ActionName[6][3] = {_T("吃"), _T("碰"), _T("杠"), _T("立直"), _T("和"), _T("自摸")};
					//const wchar_t NumName[4][2] = {_T("一"), _T("二"), _T("三"), _T("四")};
					//const int ActionDefine[4] = {};
					int ActionNo = -1;
					switch (SpecialAction.ActionType)
					{
					case RESPONSE_CHI:
					case RESPONSE_CHI_1:
					case RESPONSE_CHI_2:
					case RESPONSE_CHI_3:
						ActionNo = 0;
						break;
					case RESPONSE_PON:
						ActionNo = 1;
						break;
					case RESPONSE_KAN:
						ActionNo = 2;
						break;
					case RESPONSE_RIICHI:
						ActionNo = 3;
						break;
					case RESPONSE_WIN:
						if (SpecialAction.ActionDirection == match_info.active_direction) ActionNo = 5;
						else ActionNo = 4;
						break;
					default:
						break;
					}
					ActionInfoText.Format(_T("%s"), ActionName[ActionNo]);
					COLORREF TextColor = RGB(75, 75, 128);
					if ((CutRate > 40 && CutRate < 60) && (CutRate / 4) % 2 == 0) TextColor = RGB(0, 0, 0);
					POINT ShowPos[5] = {{500, 575}, {1075, 300}, {500, 25}, {10, 300}, {500, 300}};
					ShowTransparentText(hdc, ActionInfoText, _T("楷体"), 60, TextColor, ShowPos[CutScenesDirection], 100);
				}
			}
			break;
		case CMahjongSpiritDlg::ShowMatchInfo:
			{
				CImage BackImage;
				CreateDefaultCutScenesImage(BackImage, 4, dlg_width, dlg_height);
				double LoadRate = 0;
				if (CutRate <= 4) LoadRate = (CutRate - 4) / 4.0;
				if (CutRate >= 96) LoadRate = (CutRate - 96) / 4.0;
				POINT ShowPos = {0, dlg_height / 2 - 100};
				ShowCutScenes(hdc, BackImage, dlg_width, dlg_height, ShowPos, LoadRate, 0);
				//ShowCutScenes(hdc, CutScenesDirection, CutRate, dlg_width, dlg_height);
				if (CutRate > 4 && CutRate < 96)
				{
					CString MatchInfoText; 
					const wchar_t MatchWindName[4][2] = {_T("东"), _T("南"), _T("西"), _T("北")};
					const wchar_t NumName[10][2] = {_T("一"), _T("二"), _T("三"), _T("四"), _T("五"), _T("六"), _T("七"), _T("八"), _T("九"), _T("十")};
			
					MatchInfoText.Format(_T("%s%s局"), MatchWindName[match_info.match_wind], NumName[match_info.game_num]);
					//MatchInfoText.Append(_T("局"));
					COLORREF TextColor = RGB(75, 75, 128);
					if ((CutRate > 40 && CutRate < 60) && (CutRate / 4) % 2 == 0) TextColor = RGB(0, 0, 0);
					POINT MainShowPos = {500, 300}, ExtraShowPos = {700, 330};
					ShowTransparentText(hdc, MatchInfoText, _T("楷体"), 60, TextColor, MainShowPos, 100);
					if (match_info.thisdealer_num > 0)
					{
						if (match_info.thisdealer_num <= 10)
							MatchInfoText.Format(_T("%s本场"), NumName[match_info.thisdealer_num - 1]);
						else if (match_info.thisdealer_num % 10 == 0)
							MatchInfoText.Format(_T("%s十本场"), NumName[match_info.thisdealer_num / 10 - 1]);
						else if (match_info.thisdealer_num <= 19)
							MatchInfoText.Format(_T("十%s本场"), NumName[match_info.thisdealer_num - 11]);
						else 
							MatchInfoText.Format(_T("%s十%s本场"), NumName[match_info.thisdealer_num / 10 - 1], NumName[match_info.thisdealer_num % 10 - 1]);
						ShowTransparentText(hdc, MatchInfoText, _T("楷体"), 30, TextColor, ExtraShowPos, 100);
					}

				}
				//BackImage.Destroy();
			}
			break;
		case CMahjongSpiritDlg::ShowPointsChange:
			{
				static int* pChangePoints = nullptr;
				static int* pOriginPoints = nullptr;
				if (CutRate < 99 && !pChangePoints)
				{
					pOriginPoints = new int[4];
					pChangePoints = new int[4];
					for (int i = 0; i < 4; i++) pChangePoints[i] = 0;
					if (match_info.win_direction != noneed)
					{
						getpointinfo AllGetPointInfo = pHupaiInfo->getpoint();
						if (match_info.active_direction == match_info.win_direction) // 自摸
						{
							pChangePoints[match_info.win_direction] = 1000 * match_info.RiichiBarSum;
							match_info.RiichiBarSum = 0;
							if (match_info.this_dealer == match_info.win_direction)
							{
								pChangePoints[match_info.win_direction] += 3 * (AllGetPointInfo.dealer_point + 100 * match_info.thisdealer_num);
								for (int i = 0; i < 4; i++)
									if (match_info.win_direction != i) 
										pChangePoints[i] = -(AllGetPointInfo.dealer_point + 100 * match_info.thisdealer_num);
							}
							else
							{
								pChangePoints[match_info.win_direction] += (AllGetPointInfo.dealer_point + 100 * match_info.thisdealer_num);
								pChangePoints[match_info.win_direction] += 2 * (AllGetPointInfo.normal_point + 100 * match_info.thisdealer_num);
								for (int i = 0; i < 4; i++)
									if (match_info.win_direction != i) 
										pChangePoints[i] = -((i == match_info.this_dealer ? AllGetPointInfo.dealer_point : AllGetPointInfo.normal_point) + 100 * match_info.thisdealer_num);
							}
						}
						else
						{
							if ((AllRobot[match_info.active_direction].get_paistatus() & PAI_STATUS_IPPATSU) != 0)
							{
								match_info.RiichiBarSum --;
								AllRobot[match_info.active_direction].ChangePoints(1000);
							}
							pChangePoints[match_info.win_direction] = 1000 * match_info.RiichiBarSum;
							match_info.RiichiBarSum = 0;
							if (match_info.this_dealer == match_info.win_direction)
							{
								pChangePoints[match_info.active_direction] = -AllGetPointInfo.ron_point_dealer;
								pChangePoints[match_info.win_direction] += AllGetPointInfo.ron_point_dealer;
							}
							else
							{
								pChangePoints[match_info.active_direction] = -AllGetPointInfo.ron_point_normal;
								pChangePoints[match_info.win_direction] += AllGetPointInfo.ron_point_normal;
							}
						}
					}
					else if (match_info.frame_status == FRAME_DRAW_NO_REMAINING)
					{
						bool IfTenPai[4];
						int TenPaiSum = 0;
						for (int i = 0; i < 4; i++) 
						{
							IfTenPai[i] = mypai[i].iftingpai();
							TenPaiSum += IfTenPai[i];
						}
						if (TenPaiSum > 0 && TenPaiSum < 4)
						{
							int TenPaiGetPoints = 3000 / TenPaiSum, NoTenLosePoints = -3000 / (4 - TenPaiSum);
							for (int i = 0; i < 4; i++) pChangePoints[i] = IfTenPai[i] ? TenPaiGetPoints : NoTenLosePoints;
						}
					}
					for (int i = 0; i < 4; i++) pOriginPoints[i] = AllRobot[i].GetPoints();
				}
				if (pChangePoints)
				{
					for (int i = 0; i < 4; i++) AllRobot[i].SetPoints(pOriginPoints[i] + (pChangePoints[i] * CutRate / 9900) * 100);
					if (CutRate == 99)
					{
						for (int i = 0; i < 4; i++) AllRobot[i].SetPoints(pOriginPoints[i] + pChangePoints[i]);
						delete[] pOriginPoints;
						pOriginPoints = nullptr;
						delete[] pChangePoints;
						pChangePoints = nullptr;
					}
				}
			}
			break;
		default:
			break;
		}
		
	}

	if (DrawMode)
	{
		CString PointString;
		PointString.Format(_T("(%4ld, %4ld)"), MousePoint.x, MousePoint.y);
		POINT MousePointBox = {485, 275};
		HFONT hf = CreateMyFont(_T("Times New Roman"), 48);
		SelectObject(hdc, hf);
		SetBkMode(hdc, TRANSPARENT);
		SetTextColor(hdc, RGB(255, 174, 201));
		ShowHintBox(hdc, MousePointBox, 215, 65, 0);
		TextOutW(hdc, MousePointBox.x + 10, MousePointBox.y + 10, PointString, lstrlenW(PointString));
		CString PointColorString;
		COLORREF PointColor = GetPixel(hdc, MousePoint.x, MousePoint.y);
		PointColorString.Format(_T("(%3d, %3d, %3d)"), GetRValue(PointColor), GetGValue(PointColor), GetBValue(PointColor));
		POINT PointColorBox = {450, 375};
		ShowHintBox(hdc, PointColorBox, 280, 65, 0);
		TextOutW(hdc, PointColorBox.x + 10, PointColorBox.y + 10, PointColorString, lstrlenW(PointColorString));
		DeleteObject(hf);
	}

	//将内存中的图拷贝到屏幕上进行显示
	if (mahjong_start)
		pCDC->BitBlt(0, 0, nWidth, nHeight, &MemDC, 0, 0, SRCCOPY);
		
	//绘图完成后的清理
	MemDC.SelectObject(pOldBitmap);
	MemBitmap.DeleteObject();
	MemDC.DeleteDC();
	ReleaseDC(pCDC);
}

UINT AnalysisThreadProc(LPVOID pParam)
{
	CMahjongSpiritDlg* tempDlg = (CMahjongSpiritDlg*)pParam;
	singletile temp_should_tile = tempDlg->tempmypai.analysis_should(tempDlg->isee_remaintiles, tempDlg->match_info, tempDlg->MySeat);
	if (!(temp_should_tile == defaulttile))
	{
		tempDlg->should_tile = temp_should_tile;
		tempDlg->analysis_completed = true;
		tempDlg->Invalidate(false);
		return 0;
	}
	return 1;
}

UINT GetResponseThreadProc(LPVOID pParam)
{
	CMahjongSpiritDlg* tempDlg = (CMahjongSpiritDlg*)pParam;
	tempDlg->TempResponse = tempDlg->pTempRobot->get_response(tempDlg->match_info, tempDlg->all_fulu, tempDlg->all_paihe);
	tempDlg->analysis_completed = true;
	tempDlg->Invalidate(false);
	return 0;
}

UINT GetMyResponseThreadProc(LPVOID pParam)
{
	CMahjongSpiritDlg* tempDlg = (CMahjongSpiritDlg*)pParam;
	tempDlg->MyResponse = tempDlg->pMyRobot->get_response(tempDlg->match_info, tempDlg->all_fulu, tempDlg->all_paihe);
	tempDlg->pMyRobot->abandon_response();
	tempDlg->me_analysis_completed = true;
	if (tempDlg->Choosing)
	{
		BYTE ChoiceByte = 0x0;
		if ((tempDlg->MyResponse.response & RESPONSE_CHI) != 0)
			ChoiceByte = CHOOSE_CHI;
		else if (tempDlg->MyResponse.response != RESPONSE_PASS && tempDlg->MyResponse.response != RESPONSE_OUT)
			ChoiceByte = 1 << (tempDlg->MyResponse.response - 1);
		if ((tempDlg->ChooseFlags & CHOOSE_HU) != 0)
			bool flag = true;
		for (int Choice = ChoiceByte; Choice > 0; Choice >>= 1)
			tempDlg->FinalChoice += ((Choice & tempDlg->ChooseFlags) != 0);
		if (tempDlg->FinalChoice == 0)
			tempDlg->FinalChoice = -1;
	}
	tempDlg->Invalidate(false);
	return 0;
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CMahjongSpiritDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CMahjongSpiritDlg::OnBnClickedStart()
{
	// TODO: 在此添加控件通知处理程序代码
	ifshowcover = true;
	Invalidate();
}


void CMahjongSpiritDlg::OnStnClickedBtnstart()
{
	// TODO: 在此添加控件通知处理程序代码
	if (!login_succeed)
	{
		CLoginDlg logindlg;
		INT_PTR login_result = logindlg.DoModal();
		if (login_result == IDOK)
		{
			KillTimer(2);
			ifshowwelcome = true;
			hintpic_alpha = 0;
			SetTimer(3, 75, NULL);
			login_succeed = true;
		}
	}
}


void CMahjongSpiritDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	switch(nIDEvent)
	{
	case 1:
		{
			KillTimer(1);
			//SetTimer(2, 75, NULL);
			login_succeed = true;
			ready_enter = true;
			ifshowwelcome = false;
			ifshowcover = false;
			enter_rate = 0;
			SetTimer(4, 50, NULL);
			Invalidate();
		}
		break;
	case 2:
		{
			if (startbtn_alpha > 250)
				startbtn_alpha_up = false;
			if (startbtn_alpha < 50)
				startbtn_alpha_up = true;
			if(startbtn_alpha_up)
				startbtn_alpha += 10;
			else
				startbtn_alpha -= 10;
			RECT startbtn_rect;
			GetDlgItem(ID_BTNSTART)->GetWindowRect(&startbtn_rect);
			this->ScreenToClient(&startbtn_rect);
			InvalidateRect(&startbtn_rect, false);
		}
		break;
	case 3:
		{
			hintpic_alpha += 10;
			if (hintpic_alpha > 240)
			{
				KillTimer(3);
				ready_enter = true;
				ifshowwelcome = false;
				ifshowcover = false;
				enter_rate = 0;
				SetTimer(4, 50, NULL);
			}
			RECT hintpic_rect;
			GetDlgItem(IDC_HINTPIC)->GetWindowRect(&hintpic_rect);
			this->ScreenToClient(&hintpic_rect);
			InvalidateRect(&hintpic_rect, false);
		}
		break;
	case 4:
		{
			enter_rate += 5;
			if (enter_rate == 100)
			{
				KillTimer(4);
				ready_enter = false;
				mahjong_start = true;
				SetTimer(6, 50, NULL);
				//showtilenum = 0;
			}
			else
				Invalidate(false);
		}
		break;
	case 5:
		{
			RECT tempRect;
			tempRect.left = pai_pos[0].x + showtilenum * (TILE_STRAIGHT_WIDTH + TILE_SPACE);
			tempRect.top = pai_pos[0].y - TILE_SELECTED_UP;
			if (showtilenum == 13)
			{
				KillTimer(5);
				tempRect.left = pai_pos[0].x;
			}
			else if (showtilenum == 12)
				showtilenum += 1;
			else if (showtilenum < 12)
				showtilenum += 4;
			tempRect.right = pai_pos[0].x + showtilenum * (TILE_STRAIGHT_WIDTH + TILE_SPACE);
			tempRect.bottom = pai_pos[0].y + TILE_STRAIGHT_HEIGHT;		
			InvalidateRect(&tempRect, false);
		}
		break;
	case 6:
		{
			if (mahjong_start && (CutScenesType == None || CutScenesType == ScenesEnd))
			{
				static MahjongRobot robot[4];
				static bool win = false;
				static bool Chi = false;
				static bool Pon = false;
				static bool Kan = false;
				static bool mopai = true;					//是否处于摸牌状态
				static bool analysis_begin = false;			//是否已经开始分析出牌
				static bool arrange_completed = false;		//最初的理牌是否结束
				static bool mopai_completed = false;		//摸牌阶段是否结束
				static bool tileout = false;				//是否出牌
				static singletile next_gettile;
				static bool choose_completed = false;		//选择是否结束
				static bool SetRiichi = false;				//是否已经设置好立直
				static bool tenpai_analysis = false;		//是否完成听牌分析
				static bool RobotInfoInitialize = false;

				if (!RobotInfoInitialize)
				{
					AllRobot = robot;
					const CString RobotName[4] = {_T("极地严寒"), _T("电脑一号"), _T("电脑二号"), _T("电脑三号")};
					for (int i = 0; i < 4; i++) 
					{
						robot[i].SetPoints(25000);
						robot[i].SetRobotName(RobotName[i]);
					}
					RobotInfoInitialize = true;
				}

				bool MakeRefresh = false;
		
				UINT HuFlags = 0x0;
				MySeat = east;
				singletile out_singletile = defaulttile;

				if (frame_start)
				{
					if ((match_info.win_direction == match_info.this_dealer && match_info.frame_status > 0)
						|| (match_info.win_direction == noneed && (remaintiles.get_tilesum() > 4 || mypai[match_info.this_dealer].iftingpai())))
					{
						match_info.thisdealer_num ++;
					}
					else if (match_info.frame_status > 0)
					{
						match_info.thisdealer_num = 0;
						match_info.this_dealer = direction((match_info.this_dealer + 1) % 4);
						match_info.game_num = (match_info.game_num + 1) % 4;
					}

					for (int type_num = 0; type_num < 37; type_num ++)
						if (type_num % 10 != 9)
						{
							remaintiles.set_tilenum(type_num / 10, type_num % 10, 4);
							isee_remaintiles.set_tilenum(type_num / 10, type_num % 10, 4);
						}
					match_info.kansum = 0;
					match_info.round = 0;
					match_info.frame_status = FRAME_NORMAL;
					match_info.active_direction = direction((match_info.this_dealer + 3) % 4);
					match_info.active_tile = defaulttile;
					match_info.chankan_possible = false;
					
					for (int havegettilenum = 0; havegettilenum < 10; havegettilenum++)
					{
						singletile thistile = remaintiles.get_randomtile();
						remaintiles.change_tilenum(thistile, -1);
						match_info.dora[havegettilenum] = thistile;
					}
					
					//remaintiles.change_tilenum(next_gettile, -1);
					for (int seat = 0; seat < 4; seat++)
					{
						mypai[seat].reset_all();
						mypai[seat].set_fulu(menqianqing);
						mypai[seat].reset_paihe();
						mypai[seat].remove_tile_status();
						robot[seat].reset_pai();
						robot[seat].set_seat(direction(seat));
						PaiVisible[seat] = AllVisible;
					}
					NewTile.reset_all();
					NewTile.remove_tile_status();
					tempmypai.reset_all();
					Choosing = false;
			
					mopai = true;
					tileout = false;
					analysis_begin = false;
					arrange_completed = false;
					mopai_completed = false;
					choose_completed = false;
					ChooseFlags = 0x0;
					SetRiichi = false;				//是否已经设置好立直
					win = false;
					HuFlags = 0x0;
					analysis_completed = false;
					get_all_tiles = false;
					showtilenum = 0;
					
					frame_start = false;
					SpecialAction.ActionType = RESPONSE_PASS;
					SpecialAction.ActionDirection = noneed;
					if (pHupaiInfo)
					{
						delete pHupaiInfo;
						pHupaiInfo = nullptr;
					}
					match_info.win_direction = noneed;
					WinPaiStatus = PAI_STATUS_NORMAL;
					CutScenesType = ShowMatchInfo;
					SetTimer(7, 10, NULL);
					//SetTimer(5, 250, NULL);
					break;
				}
				if (!get_all_tiles)
				{
					static int LastShowTilenum = -1;
					if (LastShowTilenum != showtilenum)
					{
						LastShowTilenum = showtilenum;
						if (showtilenum == 0)
							for (int i = 0; i < 4; i++)
								temptiles[i].reset_all();
						singletile addtile;
						int addtilenum = 0;
						if (showtilenum == 4 || showtilenum == 8 || showtilenum == 12)
							addtilenum = 4;
						else if (showtilenum == 13)
						{
							addtilenum = 1;
							get_all_tiles = true;
						}
						for (int i = 0; i < addtilenum; i++)
						{
							for (int seat = 0; seat < 4; seat ++)
							{
								addtile = remaintiles.get_randomtile();
								remaintiles.change_tilenum(addtile, -1);
								mypai[seat].change_tilenum(addtile, 1);
								if (seat == 0)
									temptiles[(showtilenum + 3) / 4 - 1].change_tilenum(addtile, 1);
							}
						}
						if (showtilenum == 13)
						{
							//mypai[0].iftingpai();
							for (int seat = 0; seat < 4; seat ++)
								robot[seat].set_pai(mypai[seat]);
							next_gettile = remaintiles.get_randomtile();
						}

						Invalidate(false);
					}
				}
				else
				{
					if (tileout_completed && CutScenesType == None)
					{
						static bool AnalysisCompleted[3];
						static robotresponse OtherResponse[3];
						static direction PonDirection = noneed;
						static bool RefreshMatchInfo = false;
						if (!RefreshMatchInfo)
						{
							NewTile.reset_all();
							all_fulu[0] = mypai[0].get_fuluinfo();
							all_paihe[0] = mypai[0].get_paihe();
							for (int seat = 1; seat < 4; seat++)
							{
								mypai[seat] = robot[seat].get_pai();
								all_fulu[seat] = mypai[seat].get_fuluinfo();
								all_paihe[seat] = mypai[seat].get_paihe();
							}
							RefreshMatchInfo = true;
						}
						for (int i = 0; i < 3; i++)
						{
							if (AnalysisCompleted[i]) continue;
							int seat = (match_info.active_direction + i + 1) % 4;
							BYTE MyPossibleResponse = 0x0;
							if (MySeat == direction(seat))
							{
								if (!Choosing && !choose_completed)
								{
									MyPossibleResponse = mypai[0].get_possible_response(match_info, MySeat);
									if (((MyPossibleResponse & POSSIBILITY_RON) != 0) && ((robot[0].get_paistatus() & PAI_STATUS_FURITEN) == 0))
									{
										pai TempPai = mypai[0];
										TempPai.change_tilenum(match_info.active_tile, 1);
										UINT TempHuFlags = 0;
										TempHuFlags |= HU_HAITEI * (remaintiles.get_tilesum() == 4);
										TempHuFlags |= HU_RIICHI * ((robot[0].get_paistatus() & PAI_STATUS_RIICHI) != 0);
										TempHuFlags |= HU_CHANKAN * match_info.chankan_possible;
										hupaiinfo TempHuInfo = TempPai.ifhu(match_info.active_tile, false, TempHuFlags, match_info, MySeat);
										if (strcmp(TempHuInfo.fanzhong, "") != 0)
										{
											Choosing = true;
											ChooseFlags |= CHOOSE_HU;
										}
									}
									if (((robot[0].get_paistatus() & PAI_STATUS_RIICHI) == 0) && !match_info.chankan_possible)
									{
										if (match_info.active_direction == (MySeat + 3) % 4 && match_info.active_tile.type != 3)
										{
											if (((MyPossibleResponse & POSSIBILITY_CHI) != 0))
											{
												Choosing = true;
												ChooseFlags |= CHOOSE_CHI;
											}
										}
										if (((MyPossibleResponse & POSSIBILITY_PON) != 0))
										{
											Choosing = true;
											ChooseFlags |= CHOOSE_PON;
											if (((MyPossibleResponse & POSSIBILITY_KAN) != 0))
											{
												ChooseFlags |= CHOOSE_KAN;
											}
										}
									}
								}
								if (!Choosing || ((ChooseFlags & ~CHOOSE_CHI) == 0))
									AnalysisCompleted[i] = true;
							}
							else
							{
								MyPossibleResponse = mypai[seat].get_possible_response(match_info, direction(seat));
								if (MyPossibleResponse == POSSIBILITY_NULL)
								{
									AnalysisCompleted[i] = true;
									OtherResponse[i].response = RESPONSE_PASS;
									continue;
								}
								if (!analysis_begin)
								{
									analysis_completed = false;
									pTempRobot = &robot[seat];
									AfxBeginThread(GetResponseThreadProc, (LPVOID)this);
									analysis_begin = true;
								}
								if (analysis_completed)
								{
									OtherResponse[i] = TempResponse;
									analysis_begin = false;
									analysis_completed = false;
									AnalysisCompleted[i] = true;
								}
								else
									break;
								if (OtherResponse[i].response == RESPONSE_WIN)
								{
									match_info.frame_status = FRAME_WIN;
									SpecialAction.ActionDirection = direction(seat);
									SpecialAction.ActionType = RESPONSE_WIN;
									Choosing = false;
									break;
									//他家点和
								}
								if (OtherResponse[i].response == RESPONSE_PON || OtherResponse[(seat - match_info.active_direction) % 4 - 1].response == RESPONSE_KAN)
								{
									PonDirection = direction(seat);
								}
							}
						}
			
						bool AnalysisAllResponse = (AnalysisCompleted[0] && AnalysisCompleted[1] && AnalysisCompleted[2]) || (SpecialAction.ActionType == RESPONSE_WIN);
						if (AnalysisAllResponse)
						{
							if (!Choosing || ((ChooseFlags & ~CHOOSE_CHI) == 0))		
							{
								tileout_completed = false;
								if (match_info.frame_status == FRAME_WIN)
								{
								}
								else if (PonDirection != noneed) 
								{
									SpecialAction.ActionDirection = PonDirection;
									SpecialAction.ActionType = RESPONSE_PON;
									if ((OtherResponse[0].response & RESPONSE_CHI) != 0) robot[(match_info.active_direction + 1) % 4].abandon_response();
									Chi = false;
									ChooseFlags = 0x0;
									Choosing = false;
								}
								else if ((OtherResponse[0].response & RESPONSE_CHI) != 0)
								{
									SpecialAction.ActionDirection = direction((match_info.active_direction + 1) % 4);
									SpecialAction.ActionType = RESPONSE_CHI;
								}
								else if (Chi)
								{
									match_info.active_direction = MySeat;
								}
								else if (!Choosing)
								{
									if (match_info.chankan_possible)
									{
										for (int seat = 0; seat < 4; seat ++)
											robot[seat].remove_paistatus(PAI_STATUS_IPPATSU);
										if (match_info.active_direction != MySeat)
											mypai[match_info.active_direction] = robot[match_info.active_direction].get_pai();
										match_info.chankan_possible = false;
									}
									else 
										match_info.active_direction = direction((match_info.active_direction + 1) % 4);
									mopai = true;
								}
								else
									tileout_completed = true;
								if (!tileout_completed)
								{
									for (int i = 0; i < 3; i++)
									{
										AnalysisCompleted[i] = false;
										OtherResponse[i].response = RESPONSE_PASS;
									}
									PonDirection = noneed;
									RefreshMatchInfo = false;
								}
							}
							choose_completed = false;
						}
					}
					if (match_info.frame_status == FRAME_NORMAL && SpecialAction.ActionType == RESPONSE_PASS/* CutScenesType == None*/)
					{
						if (Choosing)			//正在选择
						{
							static bool me_analysis_begin = false;
							if (FinalChoice == 0)
							{
								IfShowChooseColumn = true;		//弹出选择栏
								if (MeAutoMode)
								{
									if (!me_analysis_begin)
									{
										robot[0].set_pai(tempmypai);
										//me_analysis_completed = false;
										pMyRobot = &robot[0];
										AfxBeginThread(GetMyResponseThreadProc, (LPVOID)this);
										me_analysis_begin = true;
									}
								}
							}
							else
							{
								//已做出选择，获取选择项
								me_analysis_begin = false;
								me_analysis_completed = false;
								bool riichi = false;
								if (FinalChoice != -1)
								{
									SpecialAction.ActionDirection = MySeat;
									bool* AllFlags[5] = {&Chi, &Pon, &Kan, &riichi, &win};
									for (int i = 4, tmpChoiceNum = 0; i >= 0; i--)
									{
										if (ChooseFlags & (1 << i))
											tmpChoiceNum ++;
										if (tmpChoiceNum == FinalChoice)
										{
											*AllFlags[i] = true;
											break;
										}
									}
								}
								if (win)
								{
									match_info.frame_status = FRAME_WIN;
									SpecialAction.ActionType = RESPONSE_WIN;
								}
								if (riichi)
									SpecialAction.ActionType = RESPONSE_RIICHI;
								if (Pon)
								{
									SpecialAction.ActionType = RESPONSE_PON;
									Pon = false;
								}
								if (Kan && match_info.active_direction != MySeat)
								{
									SpecialAction.ActionType = RESPONSE_KAN;
									Kan = false;
								}
								if ((ChooseFlags & CHOOSE_HU) && !win)
									robot[0].add_paistatus(PAI_STATUS_FURITEN);
								FinalChoice = 0;
								ChooseFlags = 0x0;
								choose_completed = true;
								Choosing = false;
							}
							MakeRefresh = true;
						}
						// ------------------------------------摸牌----------------------------------------
						if (mopai && arrange_completed)
						{
							// 正常流局
							if (remaintiles.get_tilesum() <= 4)		
							{
								match_info.frame_status = FRAME_DRAW_NO_REMAINING;
								match_info.win_direction = noneed;
								MakeRefresh = true;
							}
							// 四杠流局
							else if (match_info.kansum == 4 && ((robot[match_info.active_direction].get_paistatus() & PAI_STATUS_RINSHAN) == 0))
							{
								bool FourKanFromOne = false;
								for (int seat = 0; seat < 4; seat ++)
								{
									if (all_fulu[seat].groupsum < 4) continue;
									bool FourKan = true;
									for (int GroupNum = 0; GroupNum < all_fulu[seat].groupsum; GroupNum++)
										if (all_fulu[seat].allgroup[GroupNum].thistype != kan && all_fulu[seat].allgroup[GroupNum].thistype != ankan)
											FourKan = false;
									FourKanFromOne |= FourKan;
								}
								if (!FourKanFromOne)
								{
									MakeRefresh = true;
									match_info.frame_status = FRAME_DRAW_FOUR_KANS;
									match_info.win_direction = noneed;
								}
							}
							if (match_info.frame_status == FRAME_NORMAL)
							{
								singletile gettile;
								if (match_info.active_direction == MySeat)
								{
									gettile = next_gettile;//remaintiles.get_randomtile();
									next_gettile = remaintiles.get_randomtile();
									temptiles[2].reset_all();
									temptiles[2].change_tilenum(next_gettile, 1);
								}
								else
								{
									gettile = remaintiles.get_randomtile();
									while(gettile == next_gettile && remaintiles.get_tilenum(gettile) == 1)
										gettile = remaintiles.get_randomtile();
								}
								remaintiles.change_tilenum(gettile, -1);
								NewTile.reset_all();
								match_info.active_tile = gettile;
								match_info.chankan_possible = false;
								if (match_info.active_direction == MySeat)
								{
									tempmypai = mypai[0];
									tempmypai.change_tilenum(gettile, 1);
									robot[0].set_pai(tempmypai);
									tenpai_analysis = false;
									if ((robot[0].get_paistatus() & PAI_STATUS_RIICHI) == 0)
										robot[0].remove_paistatus(PAI_STATUS_FURITEN);
									mypai[0].remove_tile_status(TILE_RECOMMEND);
									NewTile.remove_tile_status(TILE_RECOMMEND);
								}
								tile_out = -1;
								mopai_completed = false;
								NewTile.set_tilenum(gettile, 1);
								analysis_begin = false;
								analysis_completed = false;
								mopai = false;
								MakeRefresh = true;
							}
						}
						//-----------------------------------宝牌标注及立直、杠、和牌的可能性判断--------------------------------------
						if (!mopai_completed)
						{
							for (int i = 0; i < match_info.kansum + 1; i++)
							{
								singletile truedora;
								truedora.type = match_info.dora[i].type;
								if (truedora.type <= 2)
									truedora.num = (match_info.dora[i].num + 1) % 9;
								else if (match_info.dora[i].num < 4)
									truedora.num = (match_info.dora[i].num + 1) % 4;
								else
									truedora.num = (match_info.dora[i].num - 4 + 1) % 3 + 4;
								mypai[0].add_tile_status(truedora, TILE_DORA);
								if (match_info.active_direction == MySeat)
									NewTile.add_tile_status(truedora, TILE_DORA);
							}
							mypai[0].remove_tile_status(TILE_DANGER);
							NewTile.remove_tile_status(TILE_DANGER);
							if (OneRoundForesee)
							{
								for (int seat = 1; seat < 4; seat ++)
								{
									if (mypai[seat].iftingpai())
									{
										BYTE* TingPaiByte = mypai[seat].get_tingpaitiles_byte();
										for (int type_num = 0; type_num < 37; type_num ++)
										{
											if (type_num % 10 == 9) continue;
											if (TingPaiByte[type_num] != 0)
											{
												mypai[0].add_tile_status(type_num / 10, type_num % 10, TILE_DANGER);
												if (match_info.active_direction == MySeat)
													NewTile.add_tile_status(type_num / 10, type_num % 10, TILE_DANGER);
											}
										}
									}
								}
							}
							if (match_info.active_direction == MySeat)
							{
								if(tempmypai.ifhu().ifhupai)
								{
									UINT TempHuFlags;
									TempHuFlags |= HU_TSUMO;
									TempHuFlags |= HU_RINSHAN * ((robot[0].get_paistatus() & PAI_STATUS_RINSHAN) != 0);
									TempHuFlags |= HU_HAITEI * (remaintiles.get_tilesum() == 4);
									TempHuFlags |= HU_RIICHI * ((robot[0].get_paistatus() & PAI_STATUS_RIICHI) != 0);
									hupaiinfo TempHuInfo = tempmypai.ifhu(match_info.active_tile, false, TempHuFlags, match_info, MySeat);
									if (strcmp(TempHuInfo.fanzhong, "") != 0)
									{
										Choosing = true;
										ChooseFlags |= CHOOSE_HU;
									}
								}
								if (((robot[0].get_paistatus() & PAI_STATUS_RIICHI) == 0) && !mypai[0].iffeimenqing() && tempmypai.iftingpai() && remaintiles.get_tilesum() > 8)
								{
									Choosing = true;
									ChooseFlags |= CHOOSE_RIICHI;
								}
								{
									bool KanFlag = false;
									if ((robot[0].get_paistatus() & PAI_STATUS_RIICHI) != 0)
									{
										bool KanValid = false;
										singletile newtile = NewTile.get_ordertile(0);
										if (mypai[0].get_tilenum(newtile) == 3)
										{
											KanValid = true;
											pai AfterKan_mypai(mypai[0]), BeforeKan_mypai(mypai[0]);
											AfterKan_mypai.change_tilenum(newtile, -3);
											AfterKan_mypai.add_fulu(YakupaiFulu);
											for (int type_num = 0; type_num < 37 && KanValid; type_num ++)
											{
												if (type_num % 10 == 9) continue;
												BeforeKan_mypai.change_tilenum(type_num / 10, type_num % 10, 1);
												if (BeforeKan_mypai.ifhu().ifhupai)
												{
													AfterKan_mypai.change_tilenum(type_num / 10, type_num % 10, 1);
													if (!AfterKan_mypai.ifhu().ifhupai)
														KanValid = false;
													AfterKan_mypai.change_tilenum(type_num / 10, type_num % 10, -1);
												}
												BeforeKan_mypai.change_tilenum(type_num / 10, type_num % 10, -1);
											}
										}
										if (KanValid)
											KanFlag = true;
									}
									else
									{
										for (int type_num = 0; type_num < 37 && !KanFlag; type_num ++)
										{
											if (type_num % 10 == 9) continue;
											if (tempmypai.get_tilenum(type_num / 10, type_num % 10) == 4)
												KanFlag = true;
										}
										for (int FuluIndex = 0; FuluIndex < tempmypai.get_fulusum() && !KanFlag; FuluIndex ++)
										{
											groupinfo tempGroup = tempmypai.get_fulu(FuluIndex);
											if (tempGroup.thistype == ke && tempmypai.get_tilenum(tempGroup.keytype, tempGroup.key) == 1)
												KanFlag = true;
										}
									}
									if (KanFlag)
									{
										Choosing = true;
										ChooseFlags |= CHOOSE_KAN;
									}
								}
							}
						}
						//----------------------------------牌局未结束-------------------------------------
						else
						{
							if (match_info.frame_status == FRAME_NORMAL && SpecialAction.ActionType == RESPONSE_PASS)
							{
								IfShowTenpaiHint = false;
								if (match_info.active_direction != MySeat)
								{
									if (!tileout && !tileout_completed)
									{
										robotresponse MyResponse;
										if (!analysis_begin)
										{
											for (int seat = 0; seat < 4; seat++)
											{
												//mypai[seat] = robot[seat].get_pai();
												all_fulu[seat] = mypai[seat].get_fuluinfo();
												all_paihe[seat] = mypai[seat].get_paihe();
											}
											pTempRobot = &robot[match_info.active_direction];
											AfxBeginThread(GetResponseThreadProc, (LPVOID)this);
											analysis_begin = true;
										}
										if (analysis_completed)
										{
											MyResponse = TempResponse;
										}
								
										if (MyResponse.response == RESPONSE_WIN)
										{
											SpecialAction.ActionType = RESPONSE_WIN;
											SpecialAction.ActionDirection = match_info.active_direction;
											//自摸
										}
										else if (MyResponse.response == RESPONSE_RIICHI && (robot[match_info.active_direction].get_paistatus() & PAI_STATUS_RIICHI) == 0)
										{
											SpecialAction.ActionType = RESPONSE_RIICHI;
											SpecialAction.ActionDirection = match_info.active_direction;
										}
										else if (MyResponse.response == RESPONSE_KAN)
										{
											SpecialAction.ActionType = RESPONSE_KAN;
											SpecialAction.ActionDirection = match_info.active_direction;
										}
										else if (MyResponse.response != RESPONSE_PASS)
										{
											tileout = true;
											MakeRefresh = true;
										}
									}
									else if (tileout)
									{
										tileout = false;
										robot[match_info.active_direction].act(match_info);
										mypai[match_info.active_direction] = robot[match_info.active_direction].get_pai();
										tileout_completed = true;
										analysis_begin = false;
										MakeRefresh = true;
									}
								}
								else
								{
									// 摸牌完成，分析建议打的牌
									if (!mopai && !tileout_completed)
									{
										if (!SetRiichi)
										{
											if (!analysis_begin)
											{
												for (int seat = 0; seat < 4; seat++)
												{
													pai TempPai = robot[seat].get_pai();
													all_fulu[seat] = TempPai.get_fuluinfo();
													all_paihe[seat] = TempPai.get_paihe();
												}
												tempmypai.RefreshISeeRemainTiles(isee_remaintiles, match_info, all_fulu, all_paihe);
												AfxBeginThread(AnalysisThreadProc, (LPVOID)this);
												analysis_begin = true;
											}
											if (analysis_completed && !(should_tile == defaulttile))
											{
												mypai[0].add_tile_status(should_tile, TILE_RECOMMEND);
												NewTile.add_tile_status(should_tile, TILE_RECOMMEND);
												if (MeAutoMode)
												{
													if (should_tile == NewTile.get_ordertile(0))
														tile_out = mypai[0].get_tilesum();
													else
														tile_out = mypai[0].get_tileorder(should_tile);
												}
												MakeRefresh = true;
												should_tile = defaulttile;
											}
										}
										else		// 立直后必须随摸随打
										{
											if (!analysis_begin)
											{
												NewTile.add_tile_status(NewTile.get_ordertile(0), TILE_RECOMMEND);
												if (MeAutoMode)
												{
													tile_out = mypai[0].get_tilesum();
												}
												analysis_begin = true;
												MakeRefresh = true;
											}
										}
									}		
									//听牌提示
									if (tile_selected >= 0 && tile_selected <= mypai[0].get_tilesum())
									{
										static bool TenPai = false;
										static tiles tingpai_tiles[14];
										static bool tingpai_afterselectedout[14];
										if (!tenpai_analysis)
										{
											TenPai = tempmypai.iftingpai();
											tenpai_analysis = true;
											for (int i = 0; i < 14; i++)
											{
												tingpai_tiles[i].reset_all();
												tingpai_afterselectedout[i] = false;
											}
											if (TenPai)
											{
												for (int i = 0; i < mypai[0].get_tilesum(); i++)
												{
													singletile SelectedTile = mypai[0].get_ordertile(i);
													if (i > 0 && SelectedTile == mypai[0].get_ordertile(i - 1))
													{
														tingpai_tiles[i] = tingpai_tiles[i - 1];
														tingpai_afterselectedout[i] = tingpai_afterselectedout[i - 1];
														continue;
													}
													pai AfterSelectedOutPai = tempmypai;
													AfterSelectedOutPai.change_tilenum(SelectedTile, -1);
													if (AfterSelectedOutPai.iftingpai())
													{
														tingpai_tiles[i] = AfterSelectedOutPai.get_tingpaitiles();
														tingpai_afterselectedout[i] = true;
													}
												}
												if (NewTile.get_tilesum() > 0)
												{
													pai AfterSelectedOutPai = tempmypai;
													AfterSelectedOutPai.change_tilenum(NewTile.get_ordertile(0), -1);
													if (AfterSelectedOutPai.iftingpai())
													{
														tingpai_tiles[mypai[0].get_tilesum()] = AfterSelectedOutPai.get_tingpaitiles();
														tingpai_afterselectedout[mypai[0].get_tilesum()] = true;
													}
												}
											}
										}
										if (tingpai_afterselectedout[tile_selected])
										{
											IfShowTenpaiHint = true;
											temptiles[1] = tingpai_tiles[tile_selected];
										}
									}
									// 吃牌
									if (Chi)
									{
										static bool chiflag[3] = {false, false, false};
										static int ChiWaySum = 0;
										static int FinalChiWay = -1;
										static bool GetChiInfo = false;
										static bool ChooseChiWay = false;
										singletile ChiTile = match_info.active_tile;
										if (MeAutoMode)
											FinalChiWay = MyResponse.response / RESPONSE_CHI_1 / 2;
										else
										{
											if (!GetChiInfo)
											{
												ChiWaySum = 0;
												for (int chi_way = 0; chi_way < 3; chi_way++)
												{
													if (ChiTile.num < 2 - chi_way || ChiTile.num > 8 - chi_way) continue;
													chiflag[chi_way] = true;
													for (int k = 0; k < 3; k++)
													{
														if (-2 + k + chi_way == 0) continue;
														if(mypai[0].get_tilenum(ChiTile.type, ChiTile.num - 2 + k + chi_way) == 0)
															chiflag[chi_way] = false;
													}
													if (chiflag[chi_way])
													{
														ChiWaySum ++;
														FinalChiWay = chi_way;
													}
												}
												GetChiInfo = true;
												if (ChiWaySum > 1)
												{
													FinalChiWay = -1;
													ChooseChiWay = true;
												}
											}
											if (ChooseChiWay)
											{
												if (tile_out != -1)
												{
													singletile GoalTile = mypai[0].get_ordertile(tile_out);
													if (GoalTile.type == ChiTile.type)
													{
														if (chiflag[0] && GoalTile.num == ChiTile.num - 2)
															FinalChiWay = 0;
														if (chiflag[1] && GoalTile.num > ChiTile.num - 2 && GoalTile.num < ChiTile.num + 2)
															FinalChiWay = 1;
														if (chiflag[2] && GoalTile.num == ChiTile.num + 2)
															FinalChiWay = 2;
													}
												}
												else if (tile_selected != -1)
												{
													int PossibleChiWay = -1;
													singletile GoalTile = mypai[0].get_ordertile(tile_selected);
													if (GoalTile.type == ChiTile.type)
													{
														if (chiflag[0] && GoalTile.num == ChiTile.num - 2)
															PossibleChiWay = 0;
														if (chiflag[1] && GoalTile.num > ChiTile.num - 2 && GoalTile.num < ChiTile.num + 2)
															PossibleChiWay = 1;
														if (chiflag[2] && GoalTile.num == ChiTile.num + 2)
															PossibleChiWay = 2;
													}
													if (PossibleChiWay != -1)
													{
														tiles ChiFulu_Tiles;
														for (int k = 0; k < 3; k++)
															ChiFulu_Tiles.change_tilenum(ChiTile.type, ChiTile.num - 2 + k + PossibleChiWay, 1);
														temptiles[0] = ChiFulu_Tiles;
														IfShowChiFuluHint = true;
												
													}
												}
												tile_out = -1;
											}
										}
								
										if (FinalChiWay != -1)
										{
											SpecialAction.ActionDirection = MySeat;
											SpecialAction.ActionType = RESPONSE_CHI_1 << FinalChiWay;
											ChooseChiWay = false;
											GetChiInfo = false;
											IfShowChiFuluHint = false;
											Chi = false;
										}
									}			
									// 开杠
									if (Kan)
									{
										static int PossibleKanSum = 0;
										static singletile PossibleKanTiles[3];
										static BYTE PlusKan = 0;
										static bool GetKanInfo = false;
										static bool ChooseKan = false;
										int FinalKanIndex = 0;
										int FinalPlusKanFuluIndex = -1;
										if (!GetKanInfo)
										{
											for (int type_num = 0; type_num < 37; type_num ++)
											{
												if (type_num % 10 == 9) continue;
												if (tempmypai.get_tilenum(type_num / 10, type_num % 10) == 4)
												{
													bool KanValid = true;
													if ((robot[0].get_paistatus() & PAI_STATUS_RIICHI) != 0)
													{
														pai AfterKan_mypai(mypai[0]), BeforeKan_mypai(mypai[0]);
														AfterKan_mypai.change_tilenum(type_num / 10, type_num % 10, -3);
														AfterKan_mypai.set_fulu(yakupaiexample);
														for (int type_num = 0; type_num < 37 && KanValid; type_num ++)
														{
															if (type_num % 10 == 9) continue;
															BeforeKan_mypai.change_tilenum(type_num / 10, type_num % 10, 1);
															if (BeforeKan_mypai.ifhu().ifhupai)
															{
																AfterKan_mypai.change_tilenum(type_num / 10, type_num % 10, 1);
																if (!AfterKan_mypai.ifhu().ifhupai)
																	KanValid = false;
																AfterKan_mypai.change_tilenum(type_num / 10, type_num % 10, -1);
															}
															BeforeKan_mypai.change_tilenum(type_num / 10, type_num % 10, -1);
														}
													}
													if (KanValid)
													{
														PossibleKanTiles[PossibleKanSum] = singletile(type_num / 10, type_num % 10);
														PossibleKanSum ++;
													}
												}
											}
											for (int FuluIndex = 0; FuluIndex < tempmypai.get_fulusum(); FuluIndex ++)
											{
												groupinfo tempGroup = tempmypai.get_fulu(FuluIndex);
												if (tempGroup.thistype == ke && tempmypai.get_tilenum(tempGroup.keytype, tempGroup.key) == 1)
												{
													PossibleKanTiles[PossibleKanSum] = singletile(tempGroup.keytype, tempGroup.key);
													PlusKan |= (1 << PossibleKanSum);
													PossibleKanSum ++;
												}
											}
											GetKanInfo = true;
											if (PossibleKanSum == 1)
											{
												if (PlusKan != 0)
													for (int FuluIndex = 0; FuluIndex < tempmypai.get_fulusum(); FuluIndex ++)
													{
														fulugroup tempGroup = tempmypai.get_fulu(FuluIndex);
														if (tempGroup.thistype == ke && tempmypai.get_tilenum(tempGroup.keytype, tempGroup.key) == 1)
															FinalPlusKanFuluIndex = FuluIndex;
													}
											}
											else
											{
												ChooseKan = true;
												for (int type_num = 0; type_num < 37; type_num ++)
												{
													if (type_num % 10 == 9) continue;
													bool IsKanTile = false;
													for (int i = 0; i < PossibleKanSum; i++)
														if (PossibleKanTiles[i] == singletile(type_num / 10, type_num % 10))
															IsKanTile = true;
													if (!IsKanTile)
														mypai[0].add_tile_status(type_num / 10, type_num % 10, TILE_INVALID);
												}
											}
										}
										if (ChooseKan)
										{
											if (MeAutoMode)
											{
												if (MyResponse.keytile == NewTile.get_ordertile(0))
													tile_out = mypai[0].get_tilesum();
												else
													tile_out = mypai[0].get_tileorder(MyResponse.keytile);
											}
											if (tile_out != -1)
											{
												if (tile_out == mypai[0].get_tilesum())
												{
													singletile kan_singletile = NewTile.get_ordertile(0);
													if (!(NewTile.get_tile_status(kan_singletile) & TILE_INVALID))
													{
														for (int i = 0; i < PossibleKanSum; i++)
															if (kan_singletile == PossibleKanTiles[i])
																FinalKanIndex = i;
														if (PlusKan & (1 << FinalKanIndex))
															for (int FuluIndex = 0; FuluIndex < tempmypai.get_fulusum(); FuluIndex ++)
															{
																fulugroup tempGroup = tempmypai.get_fulu(FuluIndex);
																if (tempGroup.thistype == ke && tempmypai.get_tilenum(tempGroup.keytype, tempGroup.key) == 1)
																	FinalPlusKanFuluIndex = FuluIndex;
															}
														ChooseKan = false;
													}
												}
												else	//从手牌中切出
												{
													singletile kan_singletile = mypai[0].get_ordertile(tile_out);
													if (kan_singletile.type != -1 && !(mypai[0].get_tile_status(kan_singletile) & TILE_INVALID))
													{
														for (int i = 0; i < PossibleKanSum; i++)
															if (kan_singletile == PossibleKanTiles[i])
																FinalKanIndex = i;
														if (PlusKan & (1 << FinalKanIndex))
															for (int FuluIndex = 0; FuluIndex < tempmypai.get_fulusum(); FuluIndex ++)
															{
																fulugroup tempGroup = tempmypai.get_fulu(FuluIndex);
																if (tempGroup.thistype == ke && tempmypai.get_tilenum(tempGroup.keytype, tempGroup.key) == 1)
																	FinalPlusKanFuluIndex = FuluIndex;
															}
														ChooseKan = false;
													}
												}
											}
											tile_out = -1;
										}
										if (!ChooseKan)
										{
											singletile FinalKanTile = PossibleKanTiles[FinalKanIndex];
											if (FinalPlusKanFuluIndex == -1)
											{
												groupinfo NewFulu(ankan, FinalKanTile.type, FinalKanTile.num);
												mypai[0].add_fulu(NewFulu);
											}
											else
											{
												fulugroup NewFulu(kan, FinalKanTile.type, FinalKanTile.num, -1, mypai[0].get_fulu(FinalPlusKanFuluIndex).seat_pos);
												mypai[0].delete_fulu(FinalPlusKanFuluIndex);
												mypai[0].add_fulu(NewFulu, FinalPlusKanFuluIndex);
											}
											mypai[0].change_tilenum(FinalKanTile, -4);
											NewTile.change_tilenum(FinalKanTile, -4);
											if (NewTile.get_tilesum() > 0)
												mypai[0].change_tilenum(NewTile.get_ordertile(0), 1);
											NewTile.reset_all();
											GetKanInfo = false;
											Kan = false;
											PossibleKanSum = 0;
											match_info.kansum ++;
											robot[0].add_paistatus(PAI_STATUS_RINSHAN);
											if (FinalPlusKanFuluIndex != -1 || FinalKanTile.type == 3)
											{
												match_info.chankan_possible = true;
												match_info.active_tile = FinalKanTile;
												analysis_begin = false;
												tileout_completed = true;
											}
											else
											{
												mopai = true;
												for (int seat = 0; seat < 4; seat ++)
													robot[seat].remove_paistatus(PAI_STATUS_IPPATSU);
											}
											tempmypai.post_stop_analysis();
											analysis_completed = false;
											mypai[0].remove_tile_status(TILE_RECOMMEND);
											MakeRefresh = true;
											if ((robot[0].get_paistatus() & PAI_STATUS_RIICHI) != 0)
											{
												for (int type_num = 0; type_num < 37; type_num ++)
												{
													if (type_num % 10 == 9) continue;
													mypai[0].add_tile_status(type_num / 10, type_num % 10, TILE_INVALID);
												}
												NewTile.remove_tile_status(TILE_INVALID);
											}
										}
									}			
									
									// 正常出牌	
									if (!Chi && !Kan && tile_out != -1)							
									{
										bool TileOutValid = false;
										// 随摸随打
										if (tile_out == mypai[0].get_tilesum())
										{
											out_singletile = NewTile.get_ordertile(0);
											if (!(NewTile.get_tile_status(out_singletile) & TILE_INVALID))
											{
												TileOutValid = true;
											}
										}
										else	//从手牌中切出
										{
											out_singletile = mypai[0].get_ordertile(tile_out);
											if (out_singletile.type != -1 && !(mypai[0].get_tile_status(out_singletile) & TILE_INVALID))
											{
												mypai[0].change_tilenum(out_singletile, -1);
												mypai[0].change_tilenum(NewTile.get_ordertile(0), 1);	
												TileOutValid = true;
											}
										}
										if (out_singletile == defaulttile)
											TileOutValid = false;
										// 判断出牌的有效性，无效则不能出牌
										if (TileOutValid)
										{
											tempmypai.post_stop_analysis();
											analysis_begin = false;
											analysis_completed = false;
											mypai[0].remove_tile_status(TILE_RECOMMEND);
											if(mypai[0].iftingpai() && ((robot[0].get_paistatus() & PAI_STATUS_FURITEN) == 0))
											{
												BYTE* TingPaiTiles = mypai[0].get_tingpaitiles_byte();
												for (int RiverNum = 0; RiverNum < mypai[0].get_paihesum(); RiverNum++)
												{
													singletile ThisTile = mypai[0].get_paihetile(RiverNum);
													if (TingPaiTiles[ThisTile.type * 10 + ThisTile.num] != 0)
														robot[0].add_paistatus(PAI_STATUS_FURITEN);
												}
											}
											robot[0].remove_paistatus(PAI_STATUS_RINSHAN | PAI_STATUS_IPPATSU);
										
											MakeRefresh = true;
											// 宣告立直
											if (((robot[0].get_paistatus() & PAI_STATUS_RIICHI) != 0) && !SetRiichi)
											{
												for (int type_num = 0; type_num < 37; type_num ++)
												{
													if (type_num % 10 == 9) continue;
													mypai[0].add_tile_status(type_num / 10, type_num % 10, TILE_INVALID);
												}
												NewTile.remove_tile_status(TILE_INVALID);
												mypai[0].set_paihetile_riichi();
												robot[0].add_paistatus(PAI_STATUS_RIICHI | PAI_STATUS_IPPATSU);
												match_info.RiichiBarSum ++;
												robot[0].ChangePoints(-1000);
												SetRiichi = true;
											}
											NewTile.reset_all();
											NewTile.remove_tile_status();
											mypai[0].add_paihetile(out_singletile);
											robot[0].set_pai(mypai[0]);
											tileout_completed = true;
											match_info.active_tile = out_singletile;
											tile_selected = -1;
											IfShowTenpaiHint = false;
										}
										tile_out = -1;
									}
								}
							}
						}
						arrange_completed = true;
						mopai_completed = true;
					}
					if (SpecialAction.ActionType != RESPONSE_PASS)
					{
						static bool HaveShownCutScenes = false;
						
						if (!HaveShownCutScenes) 
						{
							CutScenesDirection = SpecialAction.ActionDirection;
							CutScenesType = ShowAction;
							SetTimer(7, 10, NULL);
							HaveShownCutScenes = true;
							break;
						}
						else 
						{
							HaveShownCutScenes = false;
						}
						switch (SpecialAction.ActionType)
						{
						case RESPONSE_WIN:
							WinPaiStatus = robot[SpecialAction.ActionDirection].get_paistatus();
							PaiVisible[SpecialAction.ActionDirection] = true;
							tempmypai = robot[SpecialAction.ActionDirection].get_pai();
							if (match_info.active_direction != SpecialAction.ActionDirection) tempmypai.change_tilenum(match_info.active_tile, 1);
							if (SpecialAction.ActionDirection != MySeat)
							{
								robot[SpecialAction.ActionDirection].act(match_info);	
							}
							else
							{
								match_info.frame_status = FRAME_WIN;
								match_info.win_direction = MySeat;
								tileout_completed = false;
								if (match_info.chankan_possible) match_info.kansum --;
							}
							MakeRefresh = true;
							break;
						case RESPONSE_RIICHI:
							robot[SpecialAction.ActionDirection].add_paistatus(PAI_STATUS_RIICHI);
							if (SpecialAction.ActionDirection == MySeat)
							{
								// 即将打出立直宣告牌，判断出牌的有效性
								for (int type_num = 0; type_num < 37; type_num ++)
								{
									if (type_num % 10 == 9) continue;
									if (tempmypai.get_tilenum(type_num / 10, type_num % 10) == 0) continue;
									tempmypai.change_tilenum(type_num / 10, type_num % 10, -1);
									// 出牌后只有处于听牌状态，才是有效出牌
									if(!tempmypai.iftingpai())
									{
										mypai[0].add_tile_status(type_num / 10, type_num % 10, TILE_INVALID);
										NewTile.add_tile_status(type_num / 10, type_num % 10, TILE_INVALID);
									}
									tempmypai.change_tilenum(type_num / 10, type_num % 10, 1);
								}
								MakeRefresh = true;
							}
							break;
						case RESPONSE_PON:
						case RESPONSE_KAN:
						case RESPONSE_CHI:
							if (SpecialAction.ActionDirection != match_info.active_direction)
							{
								robot[match_info.active_direction].set_paihetile_invisible();
								mypai[match_info.active_direction] = robot[match_info.active_direction].get_pai();
								if (SpecialAction.ActionType == RESPONSE_KAN) mopai = true;
								if (SpecialAction.ActionDirection == MySeat)
								{
									if (SpecialAction.ActionType == RESPONSE_PON)
									{
										mypai[0].change_tilenum(match_info.active_tile, -2);
										mypai[0].add_fulu(fulugroup(ke, match_info.active_tile.type, match_info.active_tile.num, match_info.active_tile.num, (match_info.active_direction - MySeat + 4) % 4));
										Pon = false;
									}
									else if (SpecialAction.ActionType == RESPONSE_KAN)
									{
										mypai[0].change_tilenum(match_info.active_tile, -3);
										mypai[0].add_fulu(fulugroup(kan, match_info.active_tile.type, match_info.active_tile.num, match_info.active_tile.num, (match_info.active_direction - MySeat + 4) % 4));
										match_info.kansum ++;
										Kan = false;
									}
									match_info.active_direction = MySeat;
									NewTile.reset_all();
									tenpai_analysis = false;
									tileout_completed = false;
									tempmypai = mypai[0];
								}
								analysis_begin = false;
								analysis_completed = false;
								robot[SpecialAction.ActionDirection].act(match_info);
								for (int seat = 0; seat < 4; seat ++) robot[seat].remove_paistatus(PAI_STATUS_IPPATSU);
							}
							else
							{
								if (SpecialAction.ActionDirection != MySeat)
								{
									bool PlusKan = robot[match_info.active_direction].get_pai().get_tilenum(MyResponse.keytile) != 4;
									robot[match_info.active_direction].act(match_info);
									if (PlusKan || MyResponse.keytile.type == 3)
									{
										match_info.chankan_possible = true;
										tileout = true;
									}
									else
									{
										mopai = true;
										for (int seat = 0; seat < 4; seat ++)
											robot[seat].remove_paistatus(PAI_STATUS_IPPATSU);
										mypai[match_info.active_direction] = robot[match_info.active_direction].get_pai();
									}
									NewTile.reset_all();
								}
								else
								{
								}
							}
							break;
						case RESPONSE_CHI_1:
						case RESPONSE_CHI_2:
						case RESPONSE_CHI_3:
							{
								int FinalChiWay = SpecialAction.ActionType / RESPONSE_CHI_1 / 2;
								singletile ChiTile = match_info.active_tile;
								robot[(MySeat + 3) % 4].set_paihetile_invisible();
								mypai[(MySeat + 3) % 4] = robot[(MySeat + 3) % 4].get_pai();
								for (int k = 0; k < 3; k++)
								{
									if (-2 + k + FinalChiWay == 0) continue;
									mypai[0].change_tilenum(ChiTile.type, ChiTile.num - 2 + k + FinalChiWay, -1);
								}
								mypai[0].add_fulu(fulugroup(shun, ChiTile.type, ChiTile.num - 2 + FinalChiWay, ChiTile.num, 3));
								tempmypai = mypai[0];
								robot[0].set_pai(tempmypai);
								analysis_begin = false;
								analysis_completed = false;
								NewTile.reset_all();
								tenpai_analysis = false;
								for (int seat = 0; seat < 4; seat ++)
									robot[seat].remove_paistatus(PAI_STATUS_IPPATSU);
								MakeRefresh = true;
							}
							break;
						default:
							break;
						}
						SpecialAction.ActionType = RESPONSE_PASS;
					}
					if (MakeRefresh)
					{
						Invalidate(false);
					}
				}//这是!get_all_tiles的括号			
			}
			if (CutScenesType == ScenesEnd) CutScenesType = None;
		}
		break;
	case 7:
		{
			if (CutScenesDirection < 0 || CutScenesDirection > 4) CutScenesDirection = 4;
			if (CutRate == 0)
			{
				IfShowCutScenes = true;
				KillTimer(6);
			}
			CutRate ++;
			if (CutRate == 100)
			{
				KillTimer(7);
				if (CutScenesType == ShowMatchInfo) SetTimer(5, 250, NULL);
				if (CutScenesType != ShowPointsChange) SetTimer(6, 50, NULL);
				CutRate = 0;
				IfShowCutScenes = false;
				CutScenesType = ScenesEnd;
			}
			Invalidate(false);
		}
		break;
	}
	CDialogEx::OnTimer(nIDEvent);
}



BOOL CMahjongSpiritDlg::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	return true;
	//return CDialogEx::OnEraseBkgnd(pDC);
}



void CMahjongSpiritDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (DrawMode)
	{
		MousePoint = point;

		RECT tempRect = {450, 275, 730, 440};
		InvalidateRect(&tempRect, false);
	}
	else if (mahjong_start && get_all_tiles)
	{
		static int old_tile_selected = -1;
		static bool clear_selected = false;
		old_tile_selected = tile_selected;
		if (point.x > pai_pos[0].x && point.x < pai_pos[0].x + mypai[0].get_tilesum() * (TILE_STRAIGHT_WIDTH + TILE_SPACE)
			&& point.y > pai_pos[0].y && point.y < pai_pos[0].y + TILE_STRAIGHT_HEIGHT)
		{
			tile_selected = (point.x - pai_pos[0].x) / (TILE_STRAIGHT_WIDTH + TILE_SPACE);
		}
		else if(point.x > pai_pos[0].x + mypai[0].get_tilesum() * (TILE_STRAIGHT_WIDTH + TILE_SPACE) + TILE_NEWSPACE
			&& point.x < pai_pos[0].x + (mypai[0].get_tilesum() + 1) * (TILE_STRAIGHT_WIDTH + TILE_SPACE) + TILE_NEWSPACE
			&& point.y > pai_pos[0].y && point.y < pai_pos[0].y + TILE_STRAIGHT_HEIGHT)
			tile_selected = mypai[0].get_tilesum();
		else
			tile_selected = -1;
		if (old_tile_selected != tile_selected)
		{
			clear_selected = false;
			RECT tempRect;
			tempRect.left = pai_pos[0].x + old_tile_selected * (TILE_STRAIGHT_WIDTH + TILE_SPACE);
			tempRect.top = pai_pos[0].y - TILE_SELECTED_UP;
			tempRect.right = pai_pos[0].x + (old_tile_selected + 1) * (TILE_STRAIGHT_WIDTH + TILE_SPACE);
			tempRect.bottom = pai_pos[0].y + TILE_STRAIGHT_HEIGHT;
			if (match_info.active_direction == MySeat) OnTimer(6);
			InvalidateRect(&tempRect, false);
			if (tile_selected != -1)
			{
				tempRect.left = pai_pos[0].x + tile_selected * (TILE_STRAIGHT_WIDTH + TILE_SPACE) + TILE_NEWSPACE * (tile_selected == mypai[0].get_tilesum());
				tempRect.top = pai_pos[0].y - TILE_SELECTED_UP;
				tempRect.right = pai_pos[0].x + (tile_selected + 1) * (TILE_STRAIGHT_WIDTH + TILE_SPACE) + TILE_NEWSPACE * (tile_selected == mypai[0].get_tilesum());
				tempRect.bottom = pai_pos[0].y + TILE_STRAIGHT_HEIGHT;
				InvalidateRect(&tempRect, false);
			}
		}
		else if (!clear_selected)
		{
			clear_selected = true;
			RECT tempRect;
			tempRect.left = pai_pos[0].x;
			tempRect.top = pai_pos[0].y - TILE_SELECTED_UP;
			tempRect.right = pai_pos[0].x + (mypai[0].get_tilesum() + 1) * (TILE_STRAIGHT_WIDTH + TILE_SPACE) + TILE_NEWSPACE;
			tempRect.bottom = pai_pos[0].y;
			InvalidateRect(&tempRect, false);
		}
		//Invalidate();
	}
	
	CDialogEx::OnMouseMove(nFlags, point);
}


void CMahjongSpiritDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
		// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (DrawMode)
	{
		MousePoint = point;
		RECT tempRect = {450, 275, 730, 440};
		InvalidateRect(&tempRect, false);
	}
	else 
	{
		if (mahjong_start && match_info.frame_status == FRAME_NORMAL && get_all_tiles && CutScenesType == None)
		{
			if (point.x > pai_pos[0].x && point.x < pai_pos[0].x + mypai[0].get_tilesum() * (TILE_STRAIGHT_WIDTH + TILE_SPACE)
				&& point.y > pai_pos[0].y && point.y < pai_pos[0].y + TILE_STRAIGHT_HEIGHT)
			{
				tile_out = (point.x - pai_pos[0].x) / (TILE_STRAIGHT_WIDTH + TILE_SPACE);
				if (Choosing)
					FinalChoice = -1;
				RECT tempRect;
				tempRect.left = pai_pos[0].x + tile_out * (TILE_STRAIGHT_WIDTH + TILE_SPACE);
				tempRect.top = pai_pos[0].y - TILE_SELECTED_UP;
				tempRect.right = pai_pos[0].x + mypai[0].get_tilesum() * (TILE_STRAIGHT_WIDTH + TILE_SPACE);
				tempRect.bottom = pai_pos[0].y;
				InvalidateRect(&tempRect, false);
			}
			else if(point.x > pai_pos[0].x + mypai[0].get_tilesum() * (TILE_STRAIGHT_WIDTH + TILE_SPACE) + TILE_NEWSPACE
				&& point.x < pai_pos[0].x + (mypai[0].get_tilesum() + 1) * (TILE_STRAIGHT_WIDTH + TILE_SPACE) + TILE_NEWSPACE
				&& point.y > pai_pos[0].y && point.y < pai_pos[0].y + TILE_STRAIGHT_HEIGHT)
			{
				tile_out = mypai[0].get_tilesum();
				if (Choosing)
					FinalChoice = -1;
				RECT tempRect;
				tempRect.left = pai_pos[0].x + (tile_out - 1) * (TILE_STRAIGHT_WIDTH + TILE_SPACE) + TILE_NEWSPACE;
				tempRect.top = pai_pos[0].y - TILE_SELECTED_UP;
				tempRect.right = pai_pos[0].x + tile_out * (TILE_STRAIGHT_WIDTH + TILE_SPACE) + TILE_NEWSPACE;
				tempRect.bottom = pai_pos[0].y;
				InvalidateRect(&tempRect, false);
			}
			else
			{
				if (Choosing)
				{
					if (point.y > ChooseColumnPos.y && point.y < ChooseColumnPos.y + CHOOSE_COLUMN_HEIGHT)
					{
						CDC* pDC = GetDC();
						COLORREF ClickPosColor = pDC->GetPixel(point);
				
						if (ClickPosColor == CHOOSE_BUTTON_COLOR || ClickPosColor == CHOOSE_TEXT_COLOR)
						{
							if (point.x > dlg_width - 2.5 * CHOOSE_BUTTON_WIDTH)
								FinalChoice = -1;
							else
							{
								for (int i = 0; i < 5; i++)
									if (point.x < dlg_width - (3.5 + 1.5 * i) * CHOOSE_BUTTON_WIDTH
										&& point.x > dlg_width - (4.5 + 1.5 * i) * CHOOSE_BUTTON_WIDTH)
									FinalChoice = i + 1;
							}
						}
					}
					Invalidate();
				}
				tile_out = -1;
			}
		
		}
		if (match_info.frame_status != FRAME_NORMAL)
		{
			if (CutScenesType == None)
			{
				CutScenesType = ShowPointsChange;
				SetTimer(7, 5, NULL);
			}
			else if (CutScenesType == ScenesEnd)
			{
				//CutScenesType = None;
				frame_start = true;
				//match_info.frame_status = FRAME_NORMAL;
				SetTimer(6, 50, NULL);
			}
			Invalidate();
		}
	}
	CDialogEx::OnLButtonDown(nFlags, point);
}


void CMahjongSpiritDlg::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	OnLButtonDown(nFlags, point);
	CDialogEx::OnLButtonDblClk(nFlags, point);
}


BOOL CMahjongSpiritDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	for (int i = 0; i < 4; i++)
		if(TranslateAccelerator(m_hWnd, hAccKey[i], pMsg)){
			return true;
		}
	return CDialogEx::PreTranslateMessage(pMsg);
}


//void CMahjongSpiritDlg::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
//{
//	// TODO: 在此添加消息处理程序代码和/或调用默认值
//
//	CDialogEx::OnChar(nChar, nRepCnt, nFlags);
//}


void CMahjongSpiritDlg::OnBnClickedChangeVisible()
{
	// TODO: 在此添加控件通知处理程序代码
	static bool OriginalPaiVisible[4];
	AllVisible = !AllVisible;
	if (AllVisible)
	{
		for (int seat = 1; seat < 4; seat ++)
		{
			OriginalPaiVisible[seat] = PaiVisible[seat];
			PaiVisible[seat] = true;
		}
	}
	else
	{
		for (int seat = 1; seat < 4; seat ++)
		{
			PaiVisible[seat] = OriginalPaiVisible[seat];
		}
	}
	Invalidate(false);
}


void CMahjongSpiritDlg::OnBnClickedButtonForesee()
{
	OneRoundForesee = !OneRoundForesee;
	// TODO: 在此添加控件通知处理程序代码
}


void CMahjongSpiritDlg::OnBnClickedButtonManaged()
{
	MeAutoMode = !MeAutoMode;
	// TODO: 在此添加控件通知处理程序代码
}


void CMahjongSpiritDlg::OnBnClickedButtonDrawmode()
{
	DrawMode = !DrawMode;
	// TODO: 在此添加控件通知处理程序代码
}


void CMahjongSpiritDlg::OnGamesettingRules()
{
	// TODO: 在此添加命令处理程序代码
	if (!pRulesSettingsDlg) pRulesSettingsDlg = new CRulesSettingsDlg;
	INT_PTR nResponse = pRulesSettingsDlg->DoModal();
	if (nResponse == IDOK)
	{
		
	}
}


void CMahjongSpiritDlg::OnGamesettingMarks()
{
	// TODO: 在此添加命令处理程序代码
}


void CMahjongSpiritDlg::OnGamesettingReset()
{
	// TODO: 在此添加命令处理程序代码
}
