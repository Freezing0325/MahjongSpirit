#include "stdafx.h"
#include "functions.h"
#include "Resource.h"

tileinfo_ex get_typeinfo(char* tiles, bool withgeneral, bool get_ex)
{
	tileinfo_ex info, temp1, temp2;
	
	int TypeLen = strlen(tiles);
	if (TypeLen != 9 && TypeLen != 7)
		return tileinfo_ex();
	bool ifhonors = (TypeLen == 7);
	if (ifhonors)
	{
		for (int i = 0; i < 7; i++)
		{
			if (tiles[i] >= '3' && info.group < 4)
			{
				if (get_ex)
				{
					groupinfo *temp = &info.groupex[info.group];
					temp->thistype = ke;
					temp->key = i;
					temp->keytype = 3;
				}
				info.group ++;
			}
			else if(tiles[i] == '2')
			{
				info.general = true;
				if (get_ex)
					info.generalex = i + 1; 
			}
			if (tiles[i] >= '2')
				info.duizi += (tiles[i] - '0') / 2;
		}
	}
	else
	{
		char tilenum[10];
		memset(tilenum, '0', 9);
		tilenum[9] = '\0';
		for (int i = 0; i < 9 && tiles[i] != '\0'; i++)
		{
			tilenum[i] = tiles[i];
			if (tiles[i] >= '2')
				info.duizi += (tiles[i] - '0') / 2;
		}
		if (withgeneral)
		{
			int maxgroup = -1;
			for (int i = 0; i < 9; i++)
			{
				if (tilenum[i] >= '2')
				{
					info.general = true;
					tilenum[i] -= 2;
					temp1 = get_typeinfo(tilenum, false, get_ex);
					tilenum[i] += 2;
					if (temp1.group > maxgroup)
					{
						maxgroup = temp1.group;
						if (get_ex)
						{
							info.generalex = i;
							for (int j = 0; j < maxgroup; j++)
								info.groupex[j] = temp1.groupex[j];
						}
					}
				}
			}
			info.group = maxgroup;
		}
		else
		{
			for (int i = 0; i < 9; i++)
			{
				if (tilenum[i] == '0')
					continue; 
				if (tilenum[i] >= '3' && i < 7 && (tilenum[i] - '0') * (tilenum[i + 1] - '0') * (tilenum[i + 2] - '0') >= 1)
				{
					tileinfo_ex *besttile = NULL;
					tilenum[i] -= 3;
					temp1 = get_typeinfo(tilenum, false, get_ex);
					tilenum[i] += 3;
					for (int j = i; j < i + 3; j++)
						tilenum[j] --;
					temp2 = get_typeinfo(tilenum, false, get_ex);
					if (temp1.group + 0.5 * temp1.general > temp2.group + 0.5 * temp2.general)
					{
						for (int j = i; j < i + 3; j++)
							tilenum[j] ++;
						tilenum[i] -= 3;
						for (int j = temp1.group; j > 0; j--)
							if (j < 4 && get_ex)
								temp1.groupex[j] = temp1.groupex[j - 1];
						if (!get_ex || temp1.group < 4)
							temp1.group ++;
						if (get_ex)
						{
							temp1.groupex[0].key = i;
							temp1.groupex[0].thistype = ke;
						}
						besttile = &temp1;
					}
					else
					{
						for (int j = temp2.group; j > 0; j--)
							if (j < 4 && get_ex)
								temp2.groupex[j] = temp2.groupex[j - 1];
						if (!get_ex || temp2.group < 4)
							temp2.group ++;
						if (get_ex)
						{
							temp2.groupex[0].key = i;
							temp2.groupex[0].thistype = shun;
						}
						besttile = &temp2;
					}
					if (get_ex)
					{
						for (int j = info.group; j < besttile->group + info.group && j < 4; j++)
							info.groupex[j] = besttile->groupex[j - info.group];
						if (info.generalex != -1)
							info.generalex = besttile->generalex;
					}
					info.group += besttile->group;
					return info;
				}
				while (i < 7 && (tilenum[i] - '0') * (tilenum[i + 1] - '0') * (tilenum[i + 2] - '0') >= 1 && (!get_ex ||info.group < 4))
				{
					if (get_ex)
					{
						groupinfo *temp = &info.groupex[info.group];
						temp->thistype = shun;
						temp->key = i;
					}
					info.group ++;
					for (int j = i; j < i + 3; j++)
						tilenum[j] --;
				}
				if (tilenum[i] >= '3' && (!get_ex ||info.group < 4))
				{
					if (get_ex)
					{
						groupinfo *temp = &info.groupex[info.group];
						temp->thistype = ke;
						temp->key = i;
					}
					info.group ++;
					tilenum[i] -= 3;
				}
				else if(tilenum[i] == '2')
				{
					tilenum[i] -= 2;
					if (!info.general)
					{
						info.general = true;
						if (get_ex)
							info.generalex = i;
					}
				}
			}
		}
	}
	return info;
}

bool ifstob(int num)
{
	int i, j;
	do
	{
		i = num % 10;
		num /= 10;
		j = num % 10;
		if (i < j)
			return false;
	}while(num >= 10);
	return true;
}

int jingzhi(int num, int ten)
{
	int i = 1;
	int result = 0;
	while(num >= ten)
	{
		result += (num - (num / ten) * ten) * i;
		i *= 10;
		num /= ten;
	}
	result += num * i;
	return result;
}

BOOL TileToImage(CImage& DestImage, int type, int tile, int seat, bool visible, BYTE status)
{
	if (seat == 4) seat = 0;
	static CImage AllTilesImage, AllTileFrameImage;
	if (AllTilesImage.IsNull())
		//AllTilesImage.Load(_T("ÂéÈ¸ÅÆ.bmp"));
		AllTilesImage.LoadFromResource(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BITMAP_MAHJONG_SURFACE));
	
	if (AllTileFrameImage.IsNull())
		//AllTileFrameImage.Load(_T("Âé½«ÅÆ.png"));
		LoadImageFromResource(AllTileFrameImage, IDB_PNG_MAHJONG_TILE, _T("PNG"));
	//AllTileFrameImage.LoadFromResource(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BITMAP_MAHJONG_TILE));
	if (!DestImage.IsNull())
		DestImage.Destroy();
	HDC hAllTilesDC = AllTilesImage.GetDC();
	HDC hFrameDC = AllTileFrameImage.GetDC();
	HDC hDestDC = NULL;
	if (visible)
	{
		static CImage TileSurfaceImageCrash_0[37], TileSurfaceImageCrash_1[37], TileSurfaceImageCrash_2[37], TileSurfaceImageCrash_3[37];
		static CImage* TileSurfaceImageCrash[4] = {TileSurfaceImageCrash_0, TileSurfaceImageCrash_1, TileSurfaceImageCrash_2, TileSurfaceImageCrash_3};
		
		int DestWidth, DestHeight, TileSurfaceHeight;
		if (seat == 0 || seat == 2)
		{
			DestWidth = TILE_STRAIGHT_WIDTH;
			DestHeight = TILE_STRAIGHT_HEIGHT;
			TileSurfaceHeight = TILE_STRAIGHT_SURFACE_HEIGHT;
		}
		else if (seat == 1 || seat == 3)
		{
			DestWidth = TILE_ACROSS_WIDTH;
			DestHeight = TILE_ACROSS_HEIGHT;
			TileSurfaceHeight = TILE_ACROSS_SURFACE_HEIGHT;
		}
		else
			return false;
		DestImage.Create(DestWidth, DestHeight, 32);
		hDestDC = DestImage.GetDC();
		if (!TileSurfaceImageCrash[seat][type * 10 + tile].IsNull())
		{
			HDC hTileSurfaceImageCrashDC = TileSurfaceImageCrash[seat][type * 10 + tile].GetDC();
			BitBlt(hDestDC, 0, 0, DestWidth, DestHeight, hTileSurfaceImageCrashDC, 0, 0, SRCCOPY);
			TileSurfaceImageCrash[seat][type * 10 + tile].ReleaseDC();
		}
		else
		{
			CImage TileSurfaceImage;
			BitBlt(hDestDC, 0, 0, DestWidth, DestHeight, NULL, 0, 0, WHITENESS);
			TileSurfaceImage.Create(DestWidth, TileSurfaceHeight, 32);
			HDC hTileSurfaceImageDC = TileSurfaceImage.GetDC();

			int FramePosX;
			if (seat == 0 || seat == 2)
				FramePosX = (TILE_STRAIGHT_WIDTH + 1) * 0;
			else if (seat == 1 || seat == 3)
				FramePosX = (TILE_STRAIGHT_WIDTH + 1) * 4;
			BitBlt(hDestDC, 0, 0, DestWidth, DestHeight, hFrameDC, FramePosX, 0, SRCCOPY);

			CImage TempSurfaceImage;
			TempSurfaceImage.Create(TILE_SURFACE_WIDTH, TILE_SURFACE_HEIGHT, 32);
			HDC hTempSurfaceDC = TempSurfaceImage.GetDC();
			BitBlt(hTempSurfaceDC, 0, 0, TILE_SURFACE_WIDTH, TILE_SURFACE_HEIGHT, hAllTilesDC, 58 * tile + 31, 85 * type + 21, SRCCOPY);
			TempSurfaceImage.ReleaseDC();
			SpinImage(TempSurfaceImage, seat);

			hTempSurfaceDC = TempSurfaceImage.GetDC();
			StretchBlt(hTileSurfaceImageDC, 0, 0, DestWidth, TileSurfaceHeight, 
				hTempSurfaceDC, 0, 0, TempSurfaceImage.GetWidth(), TempSurfaceImage.GetHeight(), SRCCOPY);
			TempSurfaceImage.ReleaseDC();
			TempSurfaceImage.Destroy();

			TransparentBlt(hDestDC, 0, 0, DestWidth, TileSurfaceHeight, 
				hTileSurfaceImageDC, 0, 0, DestWidth, TileSurfaceHeight, RGB(242, 242, 242));
		
			TileSurfaceImageCrash[seat][type * 10 + tile].Create(DestWidth, DestHeight, 32);
			HDC hTileSurfaceImageCrashDC = TileSurfaceImageCrash[seat][type * 10 + tile].GetDC();
			BitBlt(hTileSurfaceImageCrashDC, 0, 0, DestWidth, DestHeight, hDestDC, 0, 0, SRCCOPY);
			TileSurfaceImageCrash[seat][type * 10 + tile].ReleaseDC();
			
			TileSurfaceImage.ReleaseDC();
			TileSurfaceImage.Destroy();
		}
	}
	else
	{
		if (seat == 0)
		{
			CImage TileSurfaceImage;
			DestImage.Create(TILE_STRAIGHT_WIDTH, TILE_STRAIGHT_HEIGHT, 32);
			hDestDC = DestImage.GetDC();
			TileSurfaceImage.Create(TILE_STRAIGHT_WIDTH, TILE_STRAIGHT_SURFACE_HEIGHT, 32);
			HDC hTileSurfaceImageDC = TileSurfaceImage.GetDC();
			
			BitBlt(hDestDC, 0, 0, TILE_STRAIGHT_WIDTH, TILE_STRAIGHT_HEIGHT, hFrameDC, (TILE_STRAIGHT_WIDTH + 1) * 3, 0, SRCCOPY);
			StretchBlt(hTileSurfaceImageDC, 0, 0, TILE_STRAIGHT_WIDTH, TILE_STRAIGHT_SURFACE_HEIGHT, 
				hAllTilesDC, 58 * tile + 31, 85 * type + 21,  TILE_SURFACE_WIDTH, TILE_SURFACE_HEIGHT, SRCCOPY);
			TransparentBlt(hDestDC, 0, TILE_STRAIGHT_HEIGHT - TILE_STRAIGHT_SURFACE_HEIGHT, TILE_STRAIGHT_WIDTH, TILE_STRAIGHT_SURFACE_HEIGHT, 
				hTileSurfaceImageDC, 0, 0, TILE_STRAIGHT_WIDTH, TILE_STRAIGHT_SURFACE_HEIGHT, RGB(242, 242, 242));			
			TileSurfaceImage.ReleaseDC();
			TileSurfaceImage.Destroy();
		}
		if (seat == -2 || seat == 2)
		{
			DestImage.Create(TILE_STRAIGHT_WIDTH, TILE_STRAIGHT_HEIGHT, 32);
			hDestDC = DestImage.GetDC();
			int FramePosX = (seat == -2 ? (TILE_STRAIGHT_WIDTH + 1) * 1 : (TILE_STRAIGHT_WIDTH + 1) * 2);
			BitBlt(hDestDC, 0, 0, DestImage.GetWidth(), DestImage.GetHeight(), hFrameDC, FramePosX, 0, SRCCOPY);
		}
		if (seat == 1 || seat == 3)
		{
			DestImage.Create(TILE_ACROSS_THICKNESS, TILE_STRAIGHT_HEIGHT, 32);
			hDestDC = DestImage.GetDC();
			int FramePosX = (TILE_STRAIGHT_WIDTH + 1) * 4 + (TILE_ACROSS_WIDTH + 1) * 2 + (seat == 3) * (TILE_ACROSS_THICKNESS + 1);
			BitBlt(hDestDC, 0, 0, DestImage.GetWidth(), DestImage.GetHeight(), hFrameDC, FramePosX, 0, SRCCOPY);
		}
		if (seat == -1 || seat == -3)
		{
			DestImage.Create(TILE_ACROSS_WIDTH, TILE_ACROSS_HEIGHT, 32);
			hDestDC = DestImage.GetDC();
			int FramePosX = (TILE_STRAIGHT_WIDTH + 1) * 4 + (TILE_ACROSS_WIDTH + 1);
			BitBlt(hDestDC, 0, 0, DestImage.GetWidth(), DestImage.GetHeight(), hFrameDC, FramePosX, 0, SRCCOPY);
		}
	}
	if (status != TILE_NORMAL)
	{
		const BYTE StatusAlpha = 50;
		COLORREF StatusColor;
		if (status & TILE_DORA)
			StatusColor = RGB(255, 255, 0);
		if (status & TILE_RECOMMEND)
			StatusColor = RGB(0, 0, 255);
		if (status & TILE_DANGER)
			StatusColor = RGB(255, 0, 0);
		if (status & TILE_INVALID)
			StatusColor = RGB(25, 25, 25);
		CImage CoverColorImage;
		RECT CoverColorRect = {0, 0, DestImage.GetWidth(), DestImage.GetHeight()};
		CoverColorImage.Create(DestImage.GetWidth(), DestImage.GetHeight(), 32);
		HDC hCoverColorDC = CoverColorImage.GetDC();
		HBRUSH hBr = CreateSolidBrush(StatusColor);
		FillRect(hCoverColorDC, &CoverColorRect, hBr);
		DeleteObject(hBr);
		if (hDestDC != NULL)
			CoverColorImage.AlphaBlend(hDestDC, 0, 0, StatusAlpha);
		CoverColorImage.ReleaseDC();
		CoverColorImage.Destroy();
	}
	AllTilesImage.ReleaseDC();
	AllTileFrameImage.ReleaseDC();
	
	DestImage.ReleaseDC();
	//AllTilesImage.Destroy();
	//AllTileFrameImage.Destroy();
	//_CrtDumpMemoryLeaks();
	
	return true;
}