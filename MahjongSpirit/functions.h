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


// ��ȡһ���Ƶ���Ϣ��withgeneralΪ���ʾǿ�п����н��ƣ�get_ex��ʾ��ȡ������ϸ��Ϣ
tileinfo_ex get_typeinfo(char* tiles, bool withgeneral = false, bool get_ex = false);	
// һ�����ֵ������Ƿ��ϸ��С����
bool ifstob(int num);				
// ��һ����ת��Ϊ��ten�����Ƶ���
int jingzhi(int num, int ten = 9);	
// ��ȡ�ض��Ƶ�ͼ����Ҫ���룺���洢ͼ���CImage�������Ƶ���������֣���λ�������ҿɼ��ԣ���ǰ�Ƶ�״̬
BOOL TileToImage(CImage& DestImage, int type, int tile, int seat = 0, bool visible = true, BYTE status = TILE_NORMAL);
