#include "stdafx.h"
#include "structs.h"
#include "functions.h"
#include "DrawFunc.h"

paihe::paihe(paihe &other):paihesum(other.paihesum), paihe_visiblesum(other.paihe_visiblesum), riichitile_num(other.riichitile_num){
	for(int i = 0; i < paihesum; i++)
		paihe_tile[i] = other.paihe_tile[i];
}

paihe::paihe(const paihe &other):paihesum(other.paihesum), paihe_visiblesum(other.paihe_visiblesum), riichitile_num(other.riichitile_num){
	for(int i = 0; i < paihesum; i++)
		paihe_tile[i] = other.paihe_tile[i];
}

fuluinfo::fuluinfo(int groupsum, fulugroup allgroup[4]):groupsum(groupsum), feimenqing(false)
{
	for (int i = 0; i < groupsum; i++)
		this->allgroup[i] = allgroup[i];
}

fuluinfo::fuluinfo(fuluinfo &other):groupsum(other.groupsum), feimenqing(other.feimenqing)
{
	for (int i = 0; i < groupsum; i++)
		allgroup[i] = other.allgroup[i];
}

fuluinfo::fuluinfo(const fuluinfo &other):groupsum(other.groupsum), feimenqing(other.feimenqing)
{
	for (int i = 0; i < groupsum; i++)
		allgroup[i] = other.allgroup[i];
}

getpointinfo hupaiinfo::getpoint()
{
	getpointinfo result;
	if (fanshu <= 5)
	{
		result.normal_point = pow(2.0, 2 + fanshu) * fushu;
		result.dealer_point = 2 * result.normal_point;
		result.ron_point_normal = 4 * result.normal_point;
		result.ron_point_dealer = 6 * result.normal_point;
		result.normal_point = (result.normal_point / 100 + (result.normal_point % 100 != 0)) * 100;
		result.dealer_point = (result.dealer_point / 100 + (result.dealer_point % 100 != 0)) * 100;
		result.ron_point_normal = (result.ron_point_normal / 100 + (result.ron_point_normal % 100 != 0)) * 100;
		result.ron_point_dealer = (result.ron_point_dealer / 100 + (result.ron_point_dealer % 100 != 0)) * 100;
		if (result.dealer_point >= 4000)
		{
			result.dealer_point = 4000;
			result.normal_point = 2000;
		}
	}
	if (fanshu >= 6 && fanshu <= 7)
	{
		result.dealer_point = 6000;
		result.normal_point = 3000;
	}
	if (fanshu >= 8 && fanshu <= 10)
	{
		result.dealer_point = 8000;
		result.normal_point = 4000;
	}
	if (fanshu >= 11 && fanshu <= 12)
	{
		result.dealer_point = 12000;
		result.normal_point = 6000;
	}
	if (fanshu >= 13)
	{
		result.dealer_point = 16000;
		result.normal_point = 8000;
	}
	if (result.dealer_point >= 4000)
	{
		result.ron_point_normal = 4 * result.normal_point;
		result.ron_point_dealer = 6 * result.normal_point;
	}
	return result;
}

void matchinfo::print_dora(const HDC &hdc, const POINT &printpos, const BYTE &ura_visible)
{
	CImage DoraImage;
	int doraimage_width = 5 * (TILE_STRAIGHT_WIDTH + TILE_SPACE) - TILE_SPACE;
	int doraimage_height;
	if (ura_visible > 0)
		doraimage_height = TILE_STRAIGHT_HEIGHT + TILE_STRAIGHT_SURFACE_HEIGHT;
	else
		doraimage_height = 2 * TILE_STRAIGHT_HEIGHT - TILE_STRAIGHT_SURFACE_HEIGHT;
	DoraImage.Create(doraimage_width, doraimage_height, 32);
	HDC hDoraDC = DoraImage.GetDC();
	BitBlt(hDoraDC, 0, 0, doraimage_width, doraimage_height, NULL, 0, 0, WHITENESS);
	if (ura_visible > 0)
	{
		const int DeltaHeight = (2 * TILE_STRAIGHT_SURFACE_HEIGHT - TILE_STRAIGHT_HEIGHT) / 2;
		for (int i = 0; i < kansum + 1; i++)
		{
			CImage ThisTileImage;
			TileToImage(ThisTileImage, dora[i].type, dora[i].num, 0, true);
			HDC hThisTileDC = ThisTileImage.GetDC();
			BitBlt(hDoraDC, (4 - i) * (TILE_STRAIGHT_WIDTH + TILE_SPACE), 0,
				TILE_STRAIGHT_WIDTH, TILE_STRAIGHT_HEIGHT, hThisTileDC, 0, 0, SRCCOPY);
			ThisTileImage.ReleaseDC();
			TileToImage(ThisTileImage, dora[i + 5].type, dora[i + 5].num, 0, ura_visible == 2);
			hThisTileDC = ThisTileImage.GetDC();
			BitBlt(hDoraDC, (4 - i) * (TILE_STRAIGHT_WIDTH + TILE_SPACE), TILE_STRAIGHT_SURFACE_HEIGHT,
				TILE_STRAIGHT_WIDTH, TILE_STRAIGHT_HEIGHT, hThisTileDC, 0, 0, SRCCOPY);
			ThisTileImage.ReleaseDC();
		}
		CImage HideTileImage;
		TileToImage(HideTileImage, 0, 0, -2, false);
		HDC hHideTileDC = HideTileImage.GetDC();
		for (int i = kansum + 1; i < 5; i++)
		{
			BitBlt(hDoraDC, (4 - i) * (TILE_STRAIGHT_WIDTH + TILE_SPACE), TILE_STRAIGHT_HEIGHT - TILE_STRAIGHT_SURFACE_HEIGHT + DeltaHeight,
				TILE_STRAIGHT_WIDTH, TILE_STRAIGHT_HEIGHT, hHideTileDC, 0, 0, SRCCOPY);
			BitBlt(hDoraDC, (4 - i) * (TILE_STRAIGHT_WIDTH + TILE_SPACE), DeltaHeight,
				TILE_STRAIGHT_WIDTH, TILE_STRAIGHT_HEIGHT, hHideTileDC, 0, 0, SRCCOPY);
		}
		HideTileImage.ReleaseDC();
	}
	else
	{
		CImage HideTileImage;
		TileToImage(HideTileImage, 0, 0, -2, false);
		HDC hHideTileDC = HideTileImage.GetDC();
		for (int i = 0; i < 5; i++)
		{
			BitBlt(hDoraDC, (4 - i) * (TILE_STRAIGHT_WIDTH + TILE_SPACE), TILE_STRAIGHT_HEIGHT - TILE_STRAIGHT_SURFACE_HEIGHT,
				TILE_STRAIGHT_WIDTH, TILE_STRAIGHT_HEIGHT, hHideTileDC, 0, 0, SRCCOPY);
		}
		for (int i = 0; i < kansum + 1; i++)
		{
			CImage ThisTileImage;
			TileToImage(ThisTileImage, dora[i].type, dora[i].num, 0, true);
			HDC hThisTileDC = ThisTileImage.GetDC();
			BitBlt(hDoraDC, (4 - i) * (TILE_STRAIGHT_WIDTH + TILE_SPACE), 0,
				TILE_STRAIGHT_WIDTH, TILE_STRAIGHT_HEIGHT, hThisTileDC, 0, 0, SRCCOPY);
			ThisTileImage.ReleaseDC();
		}
		for (int i = kansum + 1; i < 5; i++)
		{
			BitBlt(hDoraDC, (4 - i) * (TILE_STRAIGHT_WIDTH + TILE_SPACE), 0,
				TILE_STRAIGHT_WIDTH, TILE_STRAIGHT_HEIGHT, hHideTileDC, 0, 0, SRCCOPY);
		}
		HideTileImage.ReleaseDC();
		//ThisTileImage.Create(TILE_SURFACE_WIDTH, TILE_STRAIGHT_HEIGHT, 32);
	}
	TransparentBlt(hdc, printpos.x, printpos.y + (TILE_STRAIGHT_HEIGHT / 2 - TILE_STRAIGHT_SURFACE_HEIGHT) * (ura_visible > 0), 
		doraimage_width, doraimage_height, hDoraDC, 0, 0, doraimage_width, doraimage_height, RGB(255, 255, 255));
	DoraImage.ReleaseDC();
}

void matchinfo::print_dora(const HDC &hdc, const int &x, const int &y, const BYTE &ura_visible)
{
	POINT tempPoint = {x, y};
	print_dora(hdc, tempPoint, ura_visible);
}

tileinfo_ex::tileinfo_ex(tileinfo_ex &other):tileinfo(other), generalex(other.generalex)
{
	for (int i = 0; i < group; i++)
		groupex[i] = other.groupex[i];
}