#pragma once

#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#ifndef _CRT_NONSTDC_NO_DEPRECATE 
#define _CRT_NONSTDC_NO_DEPRECATE 
#endif

#include <cmath>
#include "structs.h"
#include "pai.h"


// 获取一种牌的信息，withgeneral为真表示强行考虑有将牌，get_ex表示获取牌组详细信息
tileinfo_ex get_typeinfo(char* tiles, bool withgeneral = false, bool get_ex = false);	
// 一个数字的排列是否严格从小到大
bool ifstob(int num);				
// 将一个数转化为“ten”进制的数
int jingzhi(int num, int ten = 9);	
// 获取特定牌的图像，需要传入：最后存储图像的CImage变量，牌的种类和数字，座位方向，他家可见性，当前牌的状态
BOOL TileToImage(CImage& DestImage, int type, int tile, int seat = 0, bool visible = true, BYTE status = TILE_NORMAL);
