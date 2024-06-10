#pragma once

#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#ifndef _CRT_NONSTDC_NO_DEPRECATE 
#define _CRT_NONSTDC_NO_DEPRECATE 
#endif

#define RESPONSE_PASS 0
#define RESPONSE_CHI 0x38
#define RESPONSE_PON 2
#define RESPONSE_KAN 3
#define RESPONSE_RIICHI 4
#define RESPONSE_WIN 5
#define RESPONSE_OUT 6
#define RESPONSE_NINE_ORPHANS_DRAW 7
#define RESPONSE_CHI_1 0x8
#define RESPONSE_CHI_2 0x10
#define RESPONSE_CHI_3 0x20

#define FRAME_NORMAL 0
#define FRAME_DRAW_FOUR_KANS 0x1
#define FRAME_DRAW_SAME_WIND_TILES 0x2
#define FRAME_DRAW_NINE_ORPHANS 0x4
#define FRAME_DRAW_FOUR_RIICHIS 0x8
#define FRAME_DRAW_NO_REMAINING 0x10
#define FRAME_DRAW_MANGAN 0x20
#define FRAME_DRAW 0x3F
#define FRAME_WIN 0x40

enum grouptype {shun = 1, ke, kan, ankan};
enum direction {east, south, west, north, noneed, none};


struct getpointinfo;

struct singletile			//一张牌的数字、类型信息
{
	int type;
	int num;
	singletile():type(-1), num(0){};
	singletile(singletile& other):type(other.type), num(other.num){};
	singletile(const singletile& other):type(other.type), num(other.num){};
	singletile(int type, int num):type(type), num(num){};
	bool operator==(singletile other){
		return (type == other.type && num == other.num);
	};
};

struct paihetile: public singletile
{
	bool visible;
	paihetile():singletile(-1, 0), visible(true){};
	paihetile(singletile newtile, bool visible = true, bool riichi_tile = false):singletile(newtile), visible(visible){};
	paihetile(paihetile &other):singletile(other), visible(other.visible){};
	paihetile(const paihetile &other):singletile(other), visible(other.visible){};
};

struct paihe
{
	paihetile paihe_tile[25];
	int paihesum;
	int paihe_visiblesum;
	int riichitile_num;
	paihe(): paihesum(0), paihe_visiblesum(0), riichitile_num(-1){};
	paihe(paihe &other);
	paihe(const paihe &other);
};

struct groupinfo			//一组牌的类型（吃碰杠）、关键牌、关键牌类型
{
	grouptype thistype;		//一组牌的类型（吃碰杠）
	int keytype;			//关键牌类型
	int key;				//关键牌

	bool operator==(groupinfo other){
		return (key == other.key && keytype == other.keytype && thistype == other.thistype);
	};
	groupinfo(): thistype(shun), key(-1), keytype(-1){};
	groupinfo(grouptype thistype, int keytype, int key):thistype(thistype), key(key), keytype(keytype){};
	groupinfo(groupinfo &other):thistype(other.thistype), key(other.key), keytype(other.keytype){};
	groupinfo(const groupinfo &other):thistype(other.thistype), key(other.key), keytype(other.keytype){};
};

struct fulugroup: public groupinfo
{
	int other_num;
	int seat_pos;
	bool operator==(fulugroup other){
		return (groupinfo(*this) == groupinfo(other) && other_num == other.other_num && seat_pos == other.seat_pos);
	};
	fulugroup(): groupinfo(), other_num(-1), seat_pos(-1){};
	fulugroup(grouptype thistype, int keytype, int key, int other_num = -1, int seat_pos = -1): groupinfo(thistype, keytype, key), other_num(other_num), seat_pos(seat_pos){};
	fulugroup(groupinfo groupinfo, int other_num = -1, int seat_pos = -1): groupinfo(groupinfo), other_num(other_num), seat_pos(seat_pos){};
	fulugroup(fulugroup &other): groupinfo(other), other_num(other.other_num), seat_pos(other.seat_pos){};
	fulugroup(const fulugroup &other): groupinfo(other), other_num(other.other_num), seat_pos(other.seat_pos){};
};

struct shoupaiinfo			//手牌的分组情况
{
	int groupsum;
	groupinfo allgroup[4];
	bool feimenqing;
	shoupaiinfo():groupsum(0), feimenqing(false){};
	shoupaiinfo(int groupsum, groupinfo allgroup[4]):groupsum(groupsum), feimenqing(false){
		for (int i = 0; i < groupsum; i++)
			(*this).allgroup[i] = allgroup[i];
	};
	shoupaiinfo(shoupaiinfo &other):groupsum(other.groupsum), feimenqing(other.feimenqing){
		for (int i = 0; i < 4; i++)
			allgroup[i] = other.allgroup[i];
	};
	shoupaiinfo(const shoupaiinfo &other):groupsum(other.groupsum), feimenqing(other.feimenqing){
		for (int i = 0; i < 4; i++)
			allgroup[i] = other.allgroup[i];
	};
};

struct fuluinfo			//手牌或副露的分组情况
{
	int groupsum;
	fulugroup allgroup[4];
	bool feimenqing;
	fuluinfo():groupsum(0), feimenqing(false){};
	fuluinfo(int groupsum, fulugroup allgroup[4]);
	fuluinfo(fuluinfo &other);
	fuluinfo(const fuluinfo &other);
};

struct tileinfo				//同一花色牌的信息，有几组，是否有将，有几对，分组方式，将牌是哪一对
{
	int group;
	bool general;
	int duizi;
	tileinfo(): group(0), general(false), duizi(0){};
	tileinfo(int group, bool general, int duizi): group(group), general(general), duizi(duizi){};
	tileinfo(tileinfo& other):group(other.group), general(other.general), duizi(other.duizi){};
	bool operator==(tileinfo other){
		return ((general == other.general) && (group == other.group) && (duizi == other.duizi));
	}
};

struct tileinfo_ex: public tileinfo
{
	groupinfo groupex[4];
	int generalex;
	tileinfo_ex():tileinfo(0, false, 0), generalex(-1){};
	tileinfo_ex(tileinfo_ex& other);
};

struct tingpaiinfo			//一套牌的听牌信息，几向听、几面听、和牌可能性、番数的期望值、是否向十三幺考虑、是否向七对子考虑
{
	int xiangting;
	int mianting;
	double hupossibility;
	double efan;
	bool shisanyao;
	bool qiduizi;
};

struct hupaiinfo			//一套牌的和牌信息，是否和牌、番种、番数、符数、和牌的种类（满贯、跳满等）
{
	bool ifhupai;				//是否和牌
	char fanzhong[100];			//番种
	int fanshu;					//番数
	int fushu;					//符数
	char hutype[30];			//和牌的种类（满贯、跳满等）
	getpointinfo getpoint();	//获取得点信息
};

struct matchinfo
{
	direction match_wind;		//场风
	direction this_dealer;		//当前庄家位置
	int game_num;				//局数
	int thisdealer_num;			//本场
	singletile dora[10];		//宝牌
	int kansum;					//杠数
	int round;					//巡数
	bool tenhou_possible;		//是否可以天和
	BYTE frame_status;			//牌局状态
	direction win_direction;	//获胜方
	direction active_direction;	//当前活跃者
	singletile active_tile;		//当前活跃牌
	bool chankan_possible;		//是否处于可以抢杠的状态
	int RiichiBarSum;			//当前场上立直棒的总数
	bool DoubleWindTile;		//连风对子是否记4符
	void print_dora(const HDC &hdc, const POINT &printpos, const BYTE &ura_visible = 0);
	void print_dora(const HDC &hdc, const int &x, const int &y, const BYTE &ura_visible = 0);
};


struct getpointinfo				//得点信息
{
	int normal_point;			//自摸时，闲家应付点数
	int dealer_point;			//自摸时，庄家应付点数
	int ron_point_normal;		//作为闲家，点和得点
	int ron_point_dealer;		//作为庄家，点和得点
};

struct robotresponse
{
	BYTE response;
	singletile keytile;
	robotresponse():response(RESPONSE_PASS), keytile(){};
	robotresponse(BYTE response):response(response), keytile(){};
	robotresponse(BYTE response, singletile keytile):response(response), keytile(keytile){};
	robotresponse(const robotresponse &other):response(other.response), keytile(other.keytile){};
};