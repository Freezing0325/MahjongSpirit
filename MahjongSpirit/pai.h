#pragma once
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#ifndef _CRT_NONSTDC_NO_DEPRECATE 
#define _CRT_NONSTDC_NO_DEPRECATE 
#endif

#include "stdafx.h"
#include "tiles.h"

#define TILE_NORMAL 0x0
#define TILE_DANGER 0x1
#define TILE_INVALID 0x2
#define TILE_RECOMMEND 0x4
#define TILE_DORA 0x8
#define TILE_ALLSTATUS 0xF

#define HU_TSUMO 0x1
#define HU_RIICHI 0x2
#define HU_WRIICHI 0x4
#define	HU_IPPATSU 0x8
#define HU_RINSHAN 0x10
#define HU_CHANKAN 0x20
#define HU_TENHOU 0x40
#define HU_HAITEI 0x80

#define TINGPAI_ALL 0x0
#define TINGPAI_SHIYANYAO 0x1
#define TINGPAI_QIDUIZI 0x2

#define POSSIBILITY_NULL 0x0
#define POSSIBILITY_CHI_1 0x1
#define POSSIBILITY_CHI_2 0x2
#define POSSIBILITY_CHI_3 0x4
#define POSSIBILITY_CHI 0x7
#define POSSIBILITY_PON	0x8
#define POSSIBILITY_KAN 0x10
#define POSSIBILITY_RON 0x20


extern const singletile defaulttile;
extern const matchinfo default_match;
extern const fuluinfo yakupaiexample;

extern char zipainame_simple[7][3];
extern char seatname[4][3];
extern char fanzhong_1[][9];
extern char fanzhong_2[][11];
extern char fanzhong_3[][11];
extern char fanzhong_6[][7];
extern char fanzhong_13[][9];
extern char fanzhong_sub1[][11];

class pai: public tiles{
private:
	fuluinfo fulu;
	paihe river;
	BYTE tile_status[37];
	BYTE tingpai_tiles[37];
	int nowminxiangting;
	bool stop_analysis;
public:
	pai();
	pai(const pai &other);

	// 打印牌面和副露
	void print_tiles(HDC hdc, POINT printpos, int seat = 0, bool visible = false, int selected = -1);
	void print_tiles(HDC hdc, int x, int y, int seat = 0, bool visible = false, int selected = -1);
	// 打印牌河
	void print_river(HDC hdc, POINT printpos, int seat = 0);
	void print_river(HDC hdc, int x, int y, int seat = 0);

	// 获取特定牌的当前状态
	BYTE get_tile_status(int type, int num);
	BYTE get_tile_status(singletile thistile);
	// 为特定牌增加状态
	BYTE add_tile_status(int type, int num, BYTE new_status);
	BYTE add_tile_status(singletile thistile, BYTE new_status);
	// 清空所有牌的特定状态
	void remove_tile_status(BYTE goal_status = TILE_ALLSTATUS);

	// 设定副露
	void set_fulu(fuluinfo goal_fulu){fulu = goal_fulu;}
	int get_fulusum(){return fulu.groupsum;}
	fulugroup get_fulu(int num);
	fuluinfo get_fuluinfo(){return fulu;}
	// 增加一套副露到指定位置，默认-1为最后
	void add_fulu(fulugroup newfulu, int num = -1);
	// 删除一套副露，默认-1为最后
	void delete_fulu(int num = -1);
	bool iffeimenqing(){return fulu.feimenqing;};

	// 设定牌河
	int get_paihesum(){return river.paihesum;}
	int get_paihe_visiblesum(){return river.paihe_visiblesum;}
	paihe get_paihe(){return river;}
	singletile get_paihetile(int num);
	void add_paihetile(int type, int num);
	void add_paihetile(singletile newpaihetile);
	void set_paihetile_invisible();
	void set_paihetile_riichi();
	void reset_paihe(){river.paihesum = river.paihe_visiblesum = 0;river.riichitile_num = -1;}

	//对当前形势获取可能的反应方式
	BYTE get_possible_response(matchinfo match_info, direction seat);
	//强制停止分析
	void post_stop_analysis();
	//判断当前是否听牌
	bool iftingpai();
	//判断当前的和牌情况
	BYTE* get_tingpaitiles_byte(){return tingpai_tiles;};
	//把当前听牌转化为牌面
	pai get_tingpaitiles();
	//获取当前的和牌信息
	hupaiinfo ifhu(singletile lasttile = defaulttile, bool test = true, UINT nFlags = 0x0,  matchinfo match_info = default_match, direction self_direction = noneed);
	//获取当前的听牌信息
	tingpaiinfo get_tingpaiinfo(tiles &remaintiles, const matchinfo &match_info, direction self_direction, BYTE specialform = TINGPAI_ALL);
	//分析应该打哪张牌
	singletile analysis_should(tiles &remaintiles, const matchinfo &match_info, direction self_direction = noneed);
	// 刷新某方所见的余牌总量，需要传入：需要被刷新的余牌变量、当前比赛信息、各家副露、各家牌河
	void RefreshISeeRemainTiles(tiles &ISeeRemainTiles, matchinfo match_info, const fuluinfo (&all_fulu)[4], const paihe (&all_paihe)[4]);
};
