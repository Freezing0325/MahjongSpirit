#include "stdafx.h"
#include "tiles.h"
#include "functions.h"



tiles::tiles()
{
	for (int type = 0; type < 4; type++)
	{
		memset(first(type), '0', type == 3 ? 7 : 9);
		first(type)[type == 3 ? 7 : 9] = '\0';
	}
}

tiles::tiles(const tiles &other)
{
	for (int type = 0; type < 4; type ++)
		strcpy_s(first(type), type == 3 ? 8 : 10, other.wan + type * 10);
}

bool tiles::operator==(tiles other)
{
	return !(strcmp(wan, other.wan) || strcmp(tong, other.tong) || strcmp(tiao, other.tiao) || strcmp(zi, other.zi));
}

int tiles::get_tilenum(int typenum, int tilenum)
{
	return *(first(typenum) + tilenum) - '0';
}

int tiles::get_tilenum(singletile tile)
{
	return get_tilenum(tile.type, tile.num);
}

int tiles::get_tilesum()
{
	int result = 0;
	for (int type = 0; type < 4; type ++)
		for (int tile = 0; tile < 9 - 2 * (type == 3); tile ++)
			result += get_tilenum(type, tile);
	return result;
}

void tiles::change_tilenum(int typenum, int tilenum, int change_num)
{
	if (typenum * 10 + tilenum < 37 && typenum * 10 + tilenum >= 0)
	{
		*(first(typenum) + tilenum) += change_num;
		if (*(first(typenum) + tilenum) > '4')
			*(first(typenum) + tilenum) = '4';
		if (*(first(typenum) + tilenum) < '0')
			*(first(typenum) + tilenum) = '0';
	}
}

void tiles::change_tilenum(singletile tile, int change_num)
{
	change_tilenum(tile.type, tile.num, change_num);
}

void tiles::set_tilenum(int typenum, int tilenum, int goal_num)
{
	*(first(typenum) + tilenum) = goal_num + '0';
	if (*(first(typenum) + tilenum) > '4')
		*(first(typenum) + tilenum) = '4';
	if (*(first(typenum) + tilenum) < '0')
		*(first(typenum) + tilenum) = '0';
}

void tiles::set_tilenum(singletile tile, int goal_num)
{
	set_tilenum(tile.type, tile.num, goal_num);
}

singletile tiles::get_ordertile(int num)
{
	if (num >= get_tilesum() || num < 0)
		return defaulttile;
	int findnum = num;
	for (int type = 0; type < 4; type ++)
		for (int tile = 0; tile < 9 - 2 * (type == 3); tile ++)
		{
			findnum -= get_tilenum(type, tile);
			if (findnum < 0)
			{
				singletile findtile(type, tile);
				return findtile;
			}
		}
	return defaulttile;
}

int tiles::get_tileorder(singletile tile)
{
	int findindex = 0;
	for (int type_num = 0; type_num < 37; type_num ++)
	{
		if (type_num % 10 == 9) continue;
		if (tile.type == type_num / 10 && tile.num == type_num % 10) break;
		findindex += get_tilenum(type_num / 10, type_num % 10);
	}
	return findindex;
}

singletile tiles::get_randomtile()
{
	int TileSum = get_tilesum();
	if (TileSum <= 0) return defaulttile;
	singletile result;
	int getindex = rand() % TileSum;
	int findindex = getindex;
	for (int type_num = 0; type_num < 37; type_num ++)
	{
		if (type_num % 10 == 9)
			continue;
		findindex -= get_tilenum(type_num / 10, type_num % 10);
		if (findindex < 0)
		{
			result.type = type_num / 10;
			result.num = type_num % 10;
			break;
		}
	}
	return result;
}

void tiles::reset_all()
{
	for (int type = 0; type < 4; type++)
	{
		memset(first(type), '0', type == 3 ? 7 : 9);
		first(type)[type == 3 ? 7 : 9] = '\0';
	}
}

void tiles::print_tiles(HDC hdc, POINT printpos, int seat, bool visible, int selected, BYTE* tile_status)
{
	print_tiles(hdc, printpos.x, printpos.y, seat, visible, selected, tile_status);
}

void tiles::print_tiles(HDC hdc, int x, int y, int seat, bool visible, int selected, BYTE* tile_status)
{
	int tile_sum = get_tilesum();
	if (tile_sum <= 0) return;
	bool no_selected = (selected <= -1 || selected >= get_tilesum());
	bool no_status = (tile_status == NULL);
	POINT ThisPrintPos = {0, 0};
	
	int haveprintnum = 0;	//已经打印的牌的个数
	int type_num = (seat == 1 ? 36 : 0);
	int singletile_width = 0;
	int singletile_height = 0;
	if (seat == 0 || seat == 2)
	{
		singletile_width = TILE_STRAIGHT_WIDTH;
		singletile_height = TILE_STRAIGHT_HEIGHT;
	}
	else if (seat == 1 || seat == 3)
	{
		singletile_width = visible ? TILE_ACROSS_WIDTH : TILE_ACROSS_THICKNESS;
		singletile_height = visible ? TILE_ACROSS_HEIGHT : TILE_STRAIGHT_HEIGHT;
	}
	while (type_num >= 0 && type_num <= 36)
	{
		int type = type_num / 10;
		int tile = type_num % 10;
		if (seat == 1) type_num --; else type_num ++;
		if (tile == 9) continue;
		int thistilenum = get_tilenum(type, tile);
		if (thistilenum == 0) continue;
		//该张牌的图像
		CImage ThisTileImage;
		BYTE TileStatus = no_status ? TILE_NORMAL : tile_status[type * 10 + tile];
		TileToImage(ThisTileImage, type, tile, seat, visible, TileStatus);
		HDC hThisTileImageDC = ThisTileImage.GetDC();
		//将该张牌的图像打印在全体图像中
		for (int i = 0; i < thistilenum; i++)
		{
			switch (seat)
			{
			case 0:
				ThisPrintPos.x = haveprintnum * (TILE_STRAIGHT_WIDTH + TILE_SPACE);
				ThisPrintPos.y = TILE_SELECTED_UP * (!no_selected && selected != haveprintnum);
				break;
			case 2:
				ThisPrintPos.x = (tile_sum - haveprintnum - 1) * (TILE_STRAIGHT_WIDTH + TILE_SPACE);
				break;
			case 1:
			case 3:
				ThisPrintPos.y = haveprintnum * (TILE_ACROSS_SURFACE_HEIGHT + TILE_SPACE);
				break;
			}
			TransparentBlt(hdc, x + ThisPrintPos.x, y + ThisPrintPos.y, singletile_width, singletile_height, 
				hThisTileImageDC, 0, 0, singletile_width, singletile_height, ThisTileImage.GetPixel(0, 0));
			haveprintnum ++;
		}
		ThisTileImage.ReleaseDC();
		ThisTileImage.Destroy();
	}
}