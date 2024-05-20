#pragma once

#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#ifndef _CRT_NONSTDC_NO_DEPRECATE 
#define _CRT_NONSTDC_NO_DEPRECATE 
#endif

#include "structs.h"
#include "DrawFunc.h"

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

extern int jingzhi(int num, int ten);
extern tileinfo_ex get_typeinfo(char* tiles, bool withgeneral, bool get_ex);

class tiles{
private:
	char wan[10];
	char tong[10];
	char tiao[10];
	char zi[8];
protected:
	char* first(){return wan;}
	char* first(int typenum){return wan + typenum * 10;}
public:
	tiles();
	tiles(const tiles &other);
	bool operator==(tiles other);
	int get_tilenum(int typenum, int tilenum);		// 获取特定牌的张数
	int get_tilenum(singletile tile);				// 获取特定牌的张数
	int get_tilesum();								// 获取牌的总数
	void change_tilenum(int typenum, int tilenum, int change_num);	// 改变特定牌的张数，改变量为change_num
	void change_tilenum(singletile tile, int change_num);			// 改变特定牌的张数，改变量为change_num
	void set_tilenum(int typenum, int tilenum, int goal_num);		// 设定特定牌的张数，设定量为goal_num
	void set_tilenum(singletile tile, int goal_num);				// 设定特定牌的张数，设定量为goal_num
	void reset_all();					// 将牌数量全部置零
	singletile get_ordertile(int num);	// 按顺序获取第num张牌
	int get_tileorder(singletile tile);	// 获取某种牌在所有牌中的序数位置
	singletile get_randomtile();		// 随机获取一张牌

	void print_tiles(HDC hdc, POINT printpos, int seat = 0, bool visible = false, int selected = -1, BYTE* tile_status = NULL);
	void print_tiles(HDC hdc, int x, int y, int seat = 0, bool visible = false, int selected = -1, BYTE* tile_status = NULL);
	
};
