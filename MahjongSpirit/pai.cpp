#include "stdafx.h"
#include "pai.h"
#include "functions.h"

pai::pai(): fulu(), river(), nowminxiangting(6), stop_analysis(false)
{
	// ���캯����������Ͳ���ֵ�������ȥ���Ƶ�״̬��Ϣ��ȥ��������Ϣ
	for (int type = 0; type < 4; type++)
	{
		memset(first(type), '0', type == 3 ? 7 : 9);
		first(type)[type == 3 ? 7 : 9] = '\0';
	}
	memset(tile_status, TILE_NORMAL, 37);
	memset(tingpai_tiles, 0, 37);
}

pai::pai(const pai &other): tiles(other), fulu(other.fulu), river(other.river), nowminxiangting(other.nowminxiangting), stop_analysis(other.stop_analysis)
{
	for (int i = 0; i < 37; i++)
	{
		tile_status[i] = other.tile_status[i];
		tingpai_tiles[i] = other.tingpai_tiles[i];
	}
}

void pai::print_tiles(HDC hdc, POINT printpos, int seat, bool visible, int selected)
{
	if (get_tilesum() <= 0) return;
	bool no_selected = (selected <= -1 || selected >= get_tilesum());
	if (seat == 0)
	{
		//����ȫ��ͼ��
		CImage ShouPaiImage;
		int shoupaiwidth = get_tilesum() * (TILE_STRAIGHT_WIDTH + TILE_SPACE);
		ShouPaiImage.Create(shoupaiwidth, TILE_STRAIGHT_HEIGHT + TILE_SELECTED_UP * (!no_selected), 32);
		HDC hShouPaiImageDC = ShouPaiImage.GetDC();

		//������ȫ��ͼ��ȫ��Ϳ��
		BitBlt(hShouPaiImageDC, 0, 0, shoupaiwidth, TILE_STRAIGHT_HEIGHT + TILE_SELECTED_UP * (!no_selected), NULL, 0, 0, WHITENESS);
		
		//��ʼ��ӡ����
		tiles::print_tiles(hShouPaiImageDC, 0, 0, seat, visible, selected, tile_status);

		//�԰�ɫΪ͸��ɫ��������ȫ��ͼ��������豸
		TransparentBlt(hdc, printpos.x, printpos.y - TILE_SELECTED_UP * (!no_selected), shoupaiwidth, TILE_STRAIGHT_HEIGHT + TILE_SELECTED_UP * (!no_selected),
			hShouPaiImageDC, 0, 0, shoupaiwidth, TILE_STRAIGHT_HEIGHT + TILE_SELECTED_UP * (!no_selected), RGB(255, 255, 255));
		ShouPaiImage.ReleaseDC();
		ShouPaiImage.Destroy();

		if (fulu.groupsum > 0)
		{
			CImage FuluImage;
			int FuluWidth = 0, FuluHeight = TILE_STRAIGHT_HEIGHT;
			for (int i = 0; i < fulu.groupsum; i++)
			{
				switch(fulu.allgroup[i].thistype)
				{
					case shun:
					case ke:
						FuluWidth += 2 * (TILE_STRAIGHT_WIDTH + TILE_SPACE) + TILE_ACROSS_WIDTH;
						break;
					case kan:
						if (fulu.allgroup[i].other_num == -1)		// �Ӹ�
						{
							FuluWidth += 2 * (TILE_STRAIGHT_WIDTH + TILE_SPACE) + TILE_ACROSS_WIDTH;
							FuluHeight = TILE_ACROSS_HEIGHT + TILE_SPACE + TILE_ACROSS_SURFACE_HEIGHT;
						}
						else
							FuluWidth += 3 * (TILE_STRAIGHT_WIDTH + TILE_SPACE) + TILE_ACROSS_WIDTH;
						break;
					case ankan:
						FuluWidth += 3 * (TILE_STRAIGHT_WIDTH + TILE_SPACE) + TILE_STRAIGHT_WIDTH;
						break;
				}
				FuluWidth += TILE_NEWSPACE;
			}
			FuluImage.Create(FuluWidth, FuluHeight, 32);
			
			int FuluPrintX = 0;
			HDC hFuluDC = FuluImage.GetDC();
			BitBlt(hFuluDC, 0, 0, FuluWidth, FuluHeight, NULL, 0, 0, WHITENESS);
			
			for (int i = fulu.groupsum - 1; i >= 0; i--)
			{
				fulugroup ThisGroup = fulu.allgroup[i];
				CImage ThisFuluImage;
				int ThisFuluWidth;
				switch (ThisGroup.thistype)
				{
				case shun:
					{
						ThisFuluWidth = 2 * (TILE_STRAIGHT_WIDTH + TILE_SPACE) + TILE_ACROSS_WIDTH;
						ThisFuluImage.Create(ThisFuluWidth, TILE_STRAIGHT_HEIGHT, 32);
						HDC hThisFuluDC = ThisFuluImage.GetDC();
						BitBlt(hThisFuluDC, 0, 0, ThisFuluImage.GetWidth(), ThisFuluImage.GetHeight(), NULL, 0, 0, WHITENESS);
						int PrintPosX = 0;
						CImage ThisTileImage;
						TileToImage(ThisTileImage, ThisGroup.keytype, ThisGroup.other_num, seat + 1, true);
						HDC hThisTileImageDC = ThisTileImage.GetDC();
						BitBlt(hThisFuluDC, PrintPosX, TILE_STRAIGHT_HEIGHT - ThisTileImage.GetHeight(), ThisTileImage.GetWidth(), ThisTileImage.GetHeight(),
							hThisTileImageDC, 0, 0, SRCCOPY);
						ThisTileImage.ReleaseDC();
						PrintPosX += TILE_ACROSS_WIDTH + TILE_SPACE;
						for (int j = 0; j < 3; j++)
						{
							if (j == ThisGroup.other_num - ThisGroup.key) continue;
							TileToImage(ThisTileImage, ThisGroup.keytype, ThisGroup.key + j, seat, true);
							HDC hThisTileImageDC = ThisTileImage.GetDC();
							BitBlt(hThisFuluDC, PrintPosX, TILE_STRAIGHT_HEIGHT - ThisTileImage.GetHeight(), ThisTileImage.GetWidth(), ThisTileImage.GetHeight(),
								hThisTileImageDC, 0, 0, SRCCOPY);
							PrintPosX += (TILE_STRAIGHT_WIDTH + TILE_SPACE);
							ThisTileImage.ReleaseDC();
							
						}
						ThisFuluImage.ReleaseDC();
						ThisTileImage.Destroy();
					}
					break;
				case ke:
					{
						ThisFuluWidth = 2 * (TILE_STRAIGHT_WIDTH + TILE_SPACE) + TILE_ACROSS_WIDTH;
						ThisFuluImage.Create(ThisFuluWidth, TILE_STRAIGHT_HEIGHT, 32);
						HDC hThisFuluDC = ThisFuluImage.GetDC();
						BitBlt(hThisFuluDC, 0, 0, ThisFuluImage.GetWidth(), ThisFuluImage.GetHeight(), NULL, 0, 0, WHITENESS);
						int PrintPosX = 0;
						for (int j = 0; j < 3; j++)
						{
							CImage ThisTileImage;
							TileToImage(ThisTileImage, ThisGroup.keytype, ThisGroup.key, seat + (j == 3 - ThisGroup.seat_pos), true);
							HDC hThisTileImageDC = ThisTileImage.GetDC();
							BitBlt(hThisFuluDC, PrintPosX, TILE_STRAIGHT_HEIGHT - ThisTileImage.GetHeight(), ThisTileImage.GetWidth(), ThisTileImage.GetHeight(),
								hThisTileImageDC, 0, 0, SRCCOPY);
							PrintPosX += ThisTileImage.GetWidth() + TILE_SPACE;
							ThisTileImage.ReleaseDC();
							ThisTileImage.Destroy();
						}
						ThisFuluImage.ReleaseDC();
					}
					break;
				case kan:
					{
						if(ThisGroup.other_num == -1)		// �Ӹ�
						{
							ThisFuluWidth = 2 * (TILE_STRAIGHT_WIDTH + TILE_SPACE) + TILE_ACROSS_WIDTH;
							ThisFuluImage.Create(ThisFuluWidth, TILE_ACROSS_HEIGHT + TILE_SPACE + TILE_ACROSS_SURFACE_HEIGHT, 32);
							HDC hThisFuluDC = ThisFuluImage.GetDC();
							BitBlt(hThisFuluDC, 0, 0, ThisFuluImage.GetWidth(), ThisFuluImage.GetHeight(), NULL, 0, 0, WHITENESS);
							int PrintPosX = 0;
							for (int j = 0; j < 3; j++)
							{
								CImage ThisTileImage;
								TileToImage(ThisTileImage, ThisGroup.keytype, ThisGroup.key, seat + (j == 3 - ThisGroup.seat_pos), true);
								HDC hThisTileImageDC = ThisTileImage.GetDC();
								if (j == 3 - ThisGroup.seat_pos)
								{
									BitBlt(hThisFuluDC, PrintPosX, 0, ThisTileImage.GetWidth(), ThisTileImage.GetHeight(),
										hThisTileImageDC, 0, 0, SRCCOPY);
								}
								BitBlt(hThisFuluDC, PrintPosX, TILE_ACROSS_HEIGHT + TILE_SPACE + TILE_ACROSS_SURFACE_HEIGHT - TILE_STRAIGHT_HEIGHT + TILE_STRAIGHT_HEIGHT - ThisTileImage.GetHeight(), 
									ThisTileImage.GetWidth(), ThisTileImage.GetHeight(),hThisTileImageDC, 0, 0, SRCCOPY);
								PrintPosX += ThisTileImage.GetWidth() + TILE_SPACE;
								ThisTileImage.ReleaseDC();
								ThisTileImage.Destroy();
							}
							ThisFuluImage.ReleaseDC();
						}
						else			// ������
						{
							ThisFuluWidth = 3 * (TILE_STRAIGHT_WIDTH + TILE_SPACE) + TILE_ACROSS_WIDTH;
							ThisFuluImage.Create(ThisFuluWidth, TILE_STRAIGHT_HEIGHT, 32);
							HDC hThisFuluDC = ThisFuluImage.GetDC();
							BitBlt(hThisFuluDC, 0, 0, ThisFuluImage.GetWidth(), ThisFuluImage.GetHeight(), NULL, 0, 0, WHITENESS);
							int PrintPosX = 0;
							for (int j = 0; j < 4; j++)
							{
								CImage ThisTileImage;
								bool fromthisway = false;
								fromthisway = (j == 3 && ThisGroup.seat_pos == 1) || (j == 2 && ThisGroup.seat_pos == 2) || (j == 0 && ThisGroup.seat_pos == 3);
								TileToImage(ThisTileImage, ThisGroup.keytype, ThisGroup.key, seat + fromthisway, true);
								HDC hThisTileImageDC = ThisTileImage.GetDC();
								BitBlt(hThisFuluDC, PrintPosX, TILE_STRAIGHT_HEIGHT - ThisTileImage.GetHeight(), ThisTileImage.GetWidth(), ThisTileImage.GetHeight(),
									hThisTileImageDC, 0, 0, SRCCOPY);
								PrintPosX += ThisTileImage.GetWidth() + TILE_SPACE;
								ThisTileImage.ReleaseDC();
								ThisTileImage.Destroy();
							}
							ThisFuluImage.ReleaseDC();
							
						}
					}
					break;
				case ankan:
					{
						ThisFuluWidth = 3 * (TILE_STRAIGHT_WIDTH + TILE_SPACE) + TILE_STRAIGHT_WIDTH;
						ThisFuluImage.Create(ThisFuluWidth, TILE_STRAIGHT_HEIGHT, 32);
						HDC hThisFuluDC = ThisFuluImage.GetDC();
						BitBlt(hThisFuluDC, 0, 0, ThisFuluImage.GetWidth(), ThisFuluImage.GetHeight(), NULL, 0, 0, WHITENESS);
						int PrintPosX = 0;
						for (int j = 0; j < 4; j++)
						{
							CImage ThisTileImage;
							if (j == 0 || j == 3)
								TileToImage(ThisTileImage, ThisGroup.keytype, ThisGroup.key, -2, false);
							else
								TileToImage(ThisTileImage, ThisGroup.keytype, ThisGroup.key, seat, true);
							HDC hThisTileImageDC = ThisTileImage.GetDC();
							BitBlt(hThisFuluDC, PrintPosX, TILE_STRAIGHT_HEIGHT - ThisTileImage.GetHeight(), ThisTileImage.GetWidth(), ThisTileImage.GetHeight(),
								hThisTileImageDC, 0, 0, SRCCOPY);
							PrintPosX += ThisTileImage.GetWidth() + TILE_SPACE;
							ThisTileImage.ReleaseDC();
							ThisTileImage.Destroy();
						}
						ThisFuluImage.ReleaseDC();
					}
					break;
				}
				HDC hThisFuluDC = ThisFuluImage.GetDC();
				int DeltaHeight = (FuluHeight - TILE_STRAIGHT_HEIGHT) * !(ThisGroup.thistype == kan && ThisGroup.other_num == -1);
				BitBlt(hFuluDC, FuluPrintX, DeltaHeight, ThisFuluImage.GetWidth(), ThisFuluImage.GetHeight(), hThisFuluDC, 0, 0, SRCCOPY);
				FuluPrintX += ThisFuluWidth + TILE_NEWSPACE;
				ThisFuluImage.ReleaseDC();
				ThisFuluImage.Destroy();
			}
			TransparentBlt(hdc, printpos.x + 11 * TILE_STRAIGHT_WIDTH + 4 * TILE_ACROSS_WIDTH + 5 * TILE_NEWSPACE + 13 * TILE_SPACE - FuluWidth, printpos.y - FuluHeight + TILE_STRAIGHT_HEIGHT, 
				FuluWidth, FuluHeight, hFuluDC, 0, 0, FuluWidth, FuluHeight, RGB(255, 255, 255));
			FuluImage.ReleaseDC();
			FuluImage.Destroy();
		}
	}
	if (seat == 2)
	{
		//����ȫ��ͼ��
		CImage ShouPaiImage;
		int mypai_tilesum = get_tilesum();
		int shoupaiwidth = mypai_tilesum * (TILE_STRAIGHT_WIDTH + TILE_SPACE);
		ShouPaiImage.Create(shoupaiwidth, TILE_STRAIGHT_HEIGHT, 32);
		HDC hShouPaiImageDC = ShouPaiImage.GetDC();

		//������ȫ��ͼ��ȫ��Ϳ��
		BitBlt(hShouPaiImageDC, 0, 0, shoupaiwidth, TILE_STRAIGHT_HEIGHT, NULL, 0, 0, WHITENESS);
		
		//��ʼ��ӡ����
		tiles::print_tiles(hShouPaiImageDC, 0, 0, seat, visible, selected, tile_status);

		//�԰�ɫΪ͸��ɫ��������ȫ��ͼ��������豸
		TransparentBlt(hdc, printpos.x - shoupaiwidth, printpos.y - TILE_STRAIGHT_HEIGHT, shoupaiwidth, TILE_STRAIGHT_HEIGHT,
			hShouPaiImageDC, 0, 0, shoupaiwidth, TILE_STRAIGHT_HEIGHT, RGB(255, 255, 255));
		ShouPaiImage.ReleaseDC();
		ShouPaiImage.Destroy();

		if (fulu.groupsum > 0)
		{
			CImage FuluImage;
			int FuluWidth = 0, FuluHeight = TILE_STRAIGHT_HEIGHT;
			for (int i = 0; i < fulu.groupsum; i++)
			{
				switch(fulu.allgroup[i].thistype)
				{
					case shun:
					case ke:
						FuluWidth += 2 * (TILE_STRAIGHT_WIDTH + TILE_SPACE) + TILE_ACROSS_WIDTH;
						break;
					case kan:
						if (fulu.allgroup[i].other_num == -1)		// �Ӹ�
						{
							FuluWidth += 2 * (TILE_STRAIGHT_WIDTH + TILE_SPACE) + TILE_ACROSS_WIDTH;
							FuluHeight = TILE_ACROSS_HEIGHT + TILE_SPACE + TILE_ACROSS_SURFACE_HEIGHT;
						}
						else
							FuluWidth += 3 * (TILE_STRAIGHT_WIDTH + TILE_SPACE) + TILE_ACROSS_WIDTH;
						break;
					case ankan:
						FuluWidth += 3 * (TILE_STRAIGHT_WIDTH + TILE_SPACE) + TILE_STRAIGHT_WIDTH;
						break;
				}
				FuluWidth += TILE_NEWSPACE;
			}
			FuluImage.Create(FuluWidth, FuluHeight, 32);
			
			int FuluPrintX = FuluWidth;
			HDC hFuluDC = FuluImage.GetDC();
			BitBlt(hFuluDC, 0, 0, FuluWidth, FuluHeight, NULL, 0, 0, WHITENESS);
			
			for (int i = fulu.groupsum - 1; i >= 0; i--)
			{
				fulugroup ThisGroup = fulu.allgroup[i];
				CImage ThisFuluImage;
				int ThisFuluWidth;
				switch (ThisGroup.thistype)
				{
				case shun:
					{
						ThisFuluWidth = 2 * (TILE_STRAIGHT_WIDTH + TILE_SPACE) + TILE_ACROSS_WIDTH;
						ThisFuluImage.Create(ThisFuluWidth, TILE_STRAIGHT_HEIGHT, 32);
						HDC hThisFuluDC = ThisFuluImage.GetDC();
						BitBlt(hThisFuluDC, 0, 0, ThisFuluWidth, TILE_STRAIGHT_HEIGHT, NULL, 0, 0, WHITENESS);
						int PrintPosX = 2 * (TILE_STRAIGHT_WIDTH + TILE_SPACE);
						CImage ThisTileImage;
						TileToImage(ThisTileImage, ThisGroup.keytype, ThisGroup.other_num, seat + 1, true);
						HDC hThisTileImageDC = ThisTileImage.GetDC();
						BitBlt(hThisFuluDC, PrintPosX, 0, ThisTileImage.GetWidth(), ThisTileImage.GetHeight(),
							hThisTileImageDC, 0, 0, SRCCOPY);
						ThisTileImage.ReleaseDC();
						PrintPosX -= TILE_STRAIGHT_WIDTH + TILE_SPACE;
						for (int j = 0; j < 3; j++)
						{
							if (j == ThisGroup.other_num - ThisGroup.key) continue;
							TileToImage(ThisTileImage, ThisGroup.keytype, ThisGroup.key + j, seat, true);
							HDC hThisTileImageDC = ThisTileImage.GetDC();
							BitBlt(hThisFuluDC, PrintPosX, TILE_STRAIGHT_HEIGHT - ThisTileImage.GetHeight(), ThisTileImage.GetWidth(), ThisTileImage.GetHeight(),
								hThisTileImageDC, 0, 0, SRCCOPY);
							PrintPosX -= (TILE_STRAIGHT_WIDTH + TILE_SPACE);
							ThisTileImage.ReleaseDC();
						}
						ThisTileImage.Destroy();
						ThisFuluImage.ReleaseDC();
					}
					break;
				case ke:
					{
						ThisFuluWidth = 2 * (TILE_STRAIGHT_WIDTH + TILE_SPACE) + TILE_ACROSS_WIDTH;
						ThisFuluImage.Create(ThisFuluWidth, TILE_STRAIGHT_HEIGHT, 32);
						HDC hThisFuluDC = ThisFuluImage.GetDC();
						BitBlt(hThisFuluDC, 0, 0, ThisFuluWidth, TILE_STRAIGHT_HEIGHT, NULL, 0, 0, WHITENESS);
						int PrintPosX = ThisFuluWidth;
						for (int j = 0; j < 3; j++)
						{
							CImage ThisTileImage;
							TileToImage(ThisTileImage, ThisGroup.keytype, ThisGroup.key, seat + (j == 3 - ThisGroup.seat_pos), true);
							HDC hThisTileImageDC = ThisTileImage.GetDC();
							PrintPosX -= ThisTileImage.GetWidth() + TILE_SPACE;
							BitBlt(hThisFuluDC, PrintPosX, 0, ThisTileImage.GetWidth(), ThisTileImage.GetHeight(),
								hThisTileImageDC, 0, 0, SRCCOPY);
							ThisTileImage.ReleaseDC();
							ThisTileImage.Destroy();
						}
						ThisFuluImage.ReleaseDC();
					}
					break;
				case kan:
					{
						if(ThisGroup.other_num == -1)		// �Ӹ�
						{
							ThisFuluWidth = 2 * (TILE_STRAIGHT_WIDTH + TILE_SPACE) + TILE_ACROSS_WIDTH;
							ThisFuluImage.Create(ThisFuluWidth, TILE_ACROSS_HEIGHT + TILE_SPACE + TILE_ACROSS_SURFACE_HEIGHT, 32);
							HDC hThisFuluDC = ThisFuluImage.GetDC();
							BitBlt(hThisFuluDC, 0, 0, ThisFuluImage.GetWidth(), ThisFuluImage.GetHeight(), NULL, 0, 0, WHITENESS);
							int PrintPosX = ThisFuluWidth;
							for (int j = 0; j < 3; j++)
							{
								CImage ThisTileImage;
								TileToImage(ThisTileImage, ThisGroup.keytype, ThisGroup.key, seat + (j == 3 - ThisGroup.seat_pos), true);
								HDC hThisTileImageDC = ThisTileImage.GetDC();
								PrintPosX -= ThisTileImage.GetWidth() + TILE_SPACE;
								BitBlt(hThisFuluDC, PrintPosX, 0, 
									ThisTileImage.GetWidth(), ThisTileImage.GetHeight(),hThisTileImageDC, 0, 0, SRCCOPY);
								if (j == 3 - ThisGroup.seat_pos)
								{
									BitBlt(hThisFuluDC, PrintPosX, TILE_SPACE + TILE_ACROSS_SURFACE_HEIGHT, ThisTileImage.GetWidth(), ThisTileImage.GetHeight(),
										hThisTileImageDC, 0, 0, SRCCOPY);
								}
								ThisTileImage.ReleaseDC();
								ThisTileImage.Destroy();
							}
							ThisFuluImage.ReleaseDC();
						}
						else			// ������
						{
							ThisFuluWidth = 3 * (TILE_STRAIGHT_WIDTH + TILE_SPACE) + TILE_ACROSS_WIDTH;
							ThisFuluImage.Create(ThisFuluWidth, TILE_STRAIGHT_HEIGHT, 32);
							HDC hThisFuluDC = ThisFuluImage.GetDC();
							BitBlt(hThisFuluDC, 0, 0, ThisFuluImage.GetWidth(), ThisFuluImage.GetHeight(), NULL, 0, 0, WHITENESS);
							int PrintPosX = ThisFuluWidth;
							for (int j = 0; j < 4; j++)
							{
								CImage ThisTileImage;
								bool fromthisway = false;
								fromthisway = (j == 3 && ThisGroup.seat_pos == 1) || (j == 2 && ThisGroup.seat_pos == 2) || (j == 0 && ThisGroup.seat_pos == 3);
								TileToImage(ThisTileImage, ThisGroup.keytype, ThisGroup.key, seat + fromthisway, true);
								HDC hThisTileImageDC = ThisTileImage.GetDC();
								PrintPosX -= ThisTileImage.GetWidth() + TILE_SPACE;
								BitBlt(hThisFuluDC, PrintPosX, 0, ThisTileImage.GetWidth(), ThisTileImage.GetHeight(),
									hThisTileImageDC, 0, 0, SRCCOPY);
								ThisTileImage.ReleaseDC();
								ThisTileImage.Destroy();
							}
							ThisFuluImage.ReleaseDC();
						}
					}
					break;
				case ankan:
					{
						ThisFuluWidth = 3 * (TILE_STRAIGHT_WIDTH + TILE_SPACE) + TILE_STRAIGHT_WIDTH;
						ThisFuluImage.Create(ThisFuluWidth, TILE_STRAIGHT_HEIGHT, 32);
						HDC hThisFuluDC = ThisFuluImage.GetDC();
						BitBlt(hThisFuluDC, 0, 0, ThisFuluImage.GetWidth(), ThisFuluImage.GetHeight(), NULL, 0, 0, WHITENESS);
						int PrintPosX = 0;
						for (int j = 0; j < 4; j++)
						{
							CImage ThisTileImage;
							if (j == 0 || j == 3)
								TileToImage(ThisTileImage, ThisGroup.keytype, ThisGroup.key, -2, false);
							else
								TileToImage(ThisTileImage, ThisGroup.keytype, ThisGroup.key, seat, true);
							HDC hThisTileImageDC = ThisTileImage.GetDC();
							BitBlt(hThisFuluDC, PrintPosX, TILE_STRAIGHT_HEIGHT - ThisTileImage.GetHeight(), ThisTileImage.GetWidth(), ThisTileImage.GetHeight(),
								hThisTileImageDC, 0, 0, SRCCOPY);
							PrintPosX += ThisTileImage.GetWidth() + TILE_SPACE;
							ThisTileImage.ReleaseDC();
							ThisTileImage.Destroy();
						}
						ThisFuluImage.ReleaseDC();
					}
					break;
				}
				HDC hThisFuluDC = ThisFuluImage.GetDC();
				//int DeltaHeight = (FuluHeight - TILE_STRAIGHT_HEIGHT) * !(ThisGroup.thistype == kan && ThisGroup.other_num == -1);
				FuluPrintX -= ThisFuluWidth + TILE_NEWSPACE;
				BitBlt(hFuluDC, FuluPrintX, 0, ThisFuluImage.GetWidth(), ThisFuluImage.GetHeight(), hThisFuluDC, 0, 0, SRCCOPY);
				ThisFuluImage.ReleaseDC();
				ThisFuluImage.Destroy();
			}

			TransparentBlt(hdc, printpos.x - (11 * TILE_STRAIGHT_WIDTH + 4 * TILE_ACROSS_WIDTH + 5 * TILE_NEWSPACE + 13 * TILE_SPACE), printpos.y - TILE_STRAIGHT_HEIGHT, 
				FuluWidth, FuluHeight, hFuluDC, 0, 0, FuluWidth, FuluHeight, RGB(255, 255, 255));
			FuluImage.ReleaseDC();
			FuluImage.Destroy();
		}
	}
	if (seat == 1)
	{
		//����ȫ��ͼ��
		int singletile_width = visible ? TILE_ACROSS_WIDTH : TILE_ACROSS_THICKNESS;
		int singletile_height = visible ? TILE_ACROSS_HEIGHT : TILE_STRAIGHT_HEIGHT;
		CImage ShouPaiImage;
		int mypai_tilesum = get_tilesum();
		int shoupaiheight = (mypai_tilesum - 1) * (TILE_ACROSS_SURFACE_HEIGHT + TILE_SPACE) + singletile_height;
		ShouPaiImage.Create(singletile_width, shoupaiheight, 32);
		HDC hShouPaiImageDC = ShouPaiImage.GetDC();

		//������ȫ��ͼ��ȫ��Ϳ��
		BitBlt(hShouPaiImageDC, 0, 0, singletile_width, shoupaiheight, NULL, 0, 0, WHITENESS);
		
		//��ʼ��ӡ����
		tiles::print_tiles(hShouPaiImageDC, 0, 0, seat, visible, selected, tile_status);

		if (fulu.groupsum > 0)
		{
			CImage FuluImage;
			int FuluWidth = TILE_ACROSS_WIDTH, FuluHeight = 0;
			for (int i = 0; i < fulu.groupsum; i++)
			{
				switch(fulu.allgroup[i].thistype)
				{
					case shun:
					case ke:
						FuluHeight += 2 * (TILE_ACROSS_SURFACE_HEIGHT + TILE_SPACE) + TILE_STRAIGHT_SURFACE_HEIGHT;
						break;
					case kan:
						if (fulu.allgroup[i].other_num == -1)		// �Ӹ�
						{
							FuluWidth = 2 * TILE_STRAIGHT_WIDTH + TILE_SPACE;
							FuluHeight += 2 * (TILE_ACROSS_SURFACE_HEIGHT + TILE_SPACE) + TILE_STRAIGHT_SURFACE_HEIGHT;
						}
						else
							FuluHeight += 3 * (TILE_ACROSS_SURFACE_HEIGHT + TILE_SPACE) + TILE_STRAIGHT_SURFACE_HEIGHT;
						break;
					case ankan:
						FuluHeight += 3 * (TILE_ACROSS_SURFACE_HEIGHT + TILE_SPACE) + TILE_ACROSS_SURFACE_HEIGHT;
						break;
				}
				FuluHeight += TILE_NEWSPACE;
			}
			FuluHeight += (TILE_ACROSS_HEIGHT - TILE_ACROSS_SURFACE_HEIGHT);
			FuluImage.Create(FuluWidth, FuluHeight, 32);
			
			int FuluPrintY = 0;
			HDC hFuluDC = FuluImage.GetDC();
			BitBlt(hFuluDC, 0, 0, FuluWidth, FuluHeight, NULL, 0, 0, WHITENESS);
			
			for (int i = 0; i < fulu.groupsum; i++)
			{
				fulugroup ThisGroup = fulu.allgroup[i];
				CImage ThisFuluImage;
				int ThisFuluHeight;
				switch (ThisGroup.thistype)
				{
				case shun:
					{
						ThisFuluHeight = 2 * (TILE_ACROSS_SURFACE_HEIGHT + TILE_SPACE) + TILE_STRAIGHT_HEIGHT;
						ThisFuluImage.Create(TILE_ACROSS_WIDTH, ThisFuluHeight, 32);
						HDC hThisFuluDC = ThisFuluImage.GetDC();
						BitBlt(hThisFuluDC, 0, 0, ThisFuluImage.GetWidth(), ThisFuluImage.GetHeight(), NULL, 0, 0, WHITENESS);
						int PrintPosY = 0;
						CImage ThisTileImage;
						for (int j = 2; j >= 0; j--)
						{
							if (j == ThisGroup.other_num - ThisGroup.key) continue;
							TileToImage(ThisTileImage, ThisGroup.keytype, ThisGroup.key + j, seat, true);
							HDC hThisTileImageDC = ThisTileImage.GetDC();
							BitBlt(hThisFuluDC, 0, PrintPosY, ThisTileImage.GetWidth(), ThisTileImage.GetHeight(),
								hThisTileImageDC, 0, 0, SRCCOPY);
							PrintPosY += TILE_ACROSS_SURFACE_HEIGHT + TILE_SPACE;
							ThisTileImage.ReleaseDC();
						}
						TileToImage(ThisTileImage, ThisGroup.keytype, ThisGroup.other_num, seat + 1, true);
						HDC hThisTileImageDC = ThisTileImage.GetDC();
						BitBlt(hThisFuluDC, TILE_ACROSS_WIDTH - TILE_STRAIGHT_WIDTH, PrintPosY, ThisTileImage.GetWidth(), ThisTileImage.GetHeight(),
							hThisTileImageDC, 0, 0, SRCCOPY);
						ThisTileImage.ReleaseDC();
						ThisTileImage.Destroy();
						ThisFuluImage.ReleaseDC();
					}
					break;
				case ke:
					{
						ThisFuluHeight = 2 * (TILE_ACROSS_SURFACE_HEIGHT + TILE_SPACE) + TILE_STRAIGHT_HEIGHT;
						ThisFuluImage.Create(TILE_ACROSS_WIDTH, ThisFuluHeight, 32);
						HDC hThisFuluDC = ThisFuluImage.GetDC();
						BitBlt(hThisFuluDC, 0, 0, ThisFuluImage.GetWidth(), ThisFuluImage.GetHeight(), NULL, 0, 0, WHITENESS);
						int PrintPosY = 0;
						for (int j = 0; j < 3; j++)
						{
							CImage ThisTileImage;
							TileToImage(ThisTileImage, ThisGroup.keytype, ThisGroup.key, seat + (j == ThisGroup.seat_pos - 1), true);
							HDC hThisTileImageDC = ThisTileImage.GetDC();
							BitBlt(hThisFuluDC, TILE_ACROSS_WIDTH - ThisTileImage.GetWidth(), PrintPosY, ThisTileImage.GetWidth(), ThisTileImage.GetHeight(),
								hThisTileImageDC, 0, 0, SRCCOPY);
							PrintPosY += (j == ThisGroup.seat_pos - 1 ? TILE_STRAIGHT_SURFACE_HEIGHT : TILE_ACROSS_SURFACE_HEIGHT) + TILE_SPACE;
							ThisTileImage.ReleaseDC();
							ThisTileImage.Destroy();
						}
						ThisFuluImage.ReleaseDC();
					}
					break;
				case kan:
					{
						if(ThisGroup.other_num == -1)		// �Ӹ�
						{
							ThisFuluHeight = 2 * (TILE_ACROSS_SURFACE_HEIGHT + TILE_SPACE) + TILE_STRAIGHT_HEIGHT;
							ThisFuluImage.Create(TILE_STRAIGHT_WIDTH * 2 + TILE_SPACE, ThisFuluHeight, 32);
							HDC hThisFuluDC = ThisFuluImage.GetDC();
							BitBlt(hThisFuluDC, 0, 0, ThisFuluImage.GetWidth(), ThisFuluImage.GetHeight(), NULL, 0, 0, WHITENESS);
							int PrintPosY = 0;
							for (int j = 0; j < 3; j++)
							{
								CImage ThisTileImage;
								TileToImage(ThisTileImage, ThisGroup.keytype, ThisGroup.key, seat + (j == ThisGroup.seat_pos - 1), true);
								HDC hThisTileImageDC = ThisTileImage.GetDC();
								if (j == ThisGroup.seat_pos - 1)
								{
									BitBlt(hThisFuluDC, 0, PrintPosY, ThisTileImage.GetWidth(), ThisTileImage.GetHeight(),
										hThisTileImageDC, 0, 0, SRCCOPY);
								}
								BitBlt(hThisFuluDC, TILE_STRAIGHT_WIDTH * 2 + TILE_SPACE - ThisTileImage.GetWidth(), PrintPosY, 
									ThisTileImage.GetWidth(), ThisTileImage.GetHeight(),hThisTileImageDC, 0, 0, SRCCOPY);
								PrintPosY += (j == ThisGroup.seat_pos - 1 ? TILE_STRAIGHT_SURFACE_HEIGHT : TILE_ACROSS_SURFACE_HEIGHT) + TILE_SPACE;
								ThisTileImage.ReleaseDC();
								ThisTileImage.Destroy();
							}
							ThisFuluImage.ReleaseDC();
						}
						else			// ������
						{
							ThisFuluHeight = 3 * (TILE_ACROSS_SURFACE_HEIGHT + TILE_SPACE) + TILE_STRAIGHT_HEIGHT;
							ThisFuluImage.Create(TILE_ACROSS_WIDTH, ThisFuluHeight, 32);
							HDC hThisFuluDC = ThisFuluImage.GetDC();
							BitBlt(hThisFuluDC, 0, 0, ThisFuluImage.GetWidth(), ThisFuluImage.GetHeight(), NULL, 0, 0, WHITENESS);
							int PrintPosY = 0;
							for (int j = 0; j < 4; j++)
							{
								CImage ThisTileImage;
								bool fromthisway = false;
								fromthisway = (j == 3 && ThisGroup.seat_pos == 3) || (j == 2 && ThisGroup.seat_pos == 2) || (j == 0 && ThisGroup.seat_pos == 1);
								TileToImage(ThisTileImage, ThisGroup.keytype, ThisGroup.key, seat + fromthisway, true);
								HDC hThisTileImageDC = ThisTileImage.GetDC();
								BitBlt(hThisFuluDC, TILE_ACROSS_WIDTH - ThisTileImage.GetWidth(), PrintPosY, ThisTileImage.GetWidth(), ThisTileImage.GetHeight(),
									hThisTileImageDC, 0, 0, SRCCOPY);
								PrintPosY += (fromthisway ? TILE_STRAIGHT_SURFACE_HEIGHT : TILE_ACROSS_SURFACE_HEIGHT) + TILE_SPACE;
								ThisTileImage.ReleaseDC();
								ThisTileImage.Destroy();
							}
							ThisFuluImage.ReleaseDC();
						}
					}
					break;
				case ankan:
					{
						ThisFuluHeight = 3 * (TILE_ACROSS_SURFACE_HEIGHT + TILE_SPACE) + TILE_ACROSS_HEIGHT;
						ThisFuluImage.Create(TILE_ACROSS_WIDTH, ThisFuluHeight, 32);
						HDC hThisFuluDC = ThisFuluImage.GetDC();
						BitBlt(hThisFuluDC, 0, 0, ThisFuluImage.GetWidth(), ThisFuluImage.GetHeight(), NULL, 0, 0, WHITENESS);
						int PrintPosY = 0;
						for (int j = 0; j < 4; j++)
						{
							CImage ThisTileImage;
							if (j == 0 || j == 3)
								TileToImage(ThisTileImage, ThisGroup.keytype, ThisGroup.key, -1, false);
							else
								TileToImage(ThisTileImage, ThisGroup.keytype, ThisGroup.key, seat, true);
							HDC hThisTileImageDC = ThisTileImage.GetDC();
							BitBlt(hThisFuluDC, 0, PrintPosY, ThisTileImage.GetWidth(), ThisTileImage.GetHeight(),
								hThisTileImageDC, 0, 0, SRCCOPY);
							PrintPosY += TILE_ACROSS_SURFACE_HEIGHT + TILE_SPACE;
							ThisTileImage.ReleaseDC();
							ThisTileImage.Destroy();
						}
						ThisFuluImage.ReleaseDC();
					}
					break;
				}
				HDC hThisFuluDC = ThisFuluImage.GetDC();
				int DeltaWidth = (FuluWidth - TILE_ACROSS_WIDTH) * !(ThisGroup.thistype == kan && ThisGroup.other_num == -1);
				TransparentBlt(hFuluDC, DeltaWidth, FuluPrintY, ThisFuluImage.GetWidth(), ThisFuluImage.GetHeight(), hThisFuluDC, 0, 0, ThisFuluImage.GetWidth(), ThisFuluImage.GetHeight(), RGB(255, 255, 255));
				FuluPrintY += ThisFuluHeight - (TILE_ACROSS_HEIGHT - TILE_ACROSS_SURFACE_HEIGHT) + TILE_NEWSPACE;
				ThisFuluImage.ReleaseDC();
				ThisFuluImage.Destroy();
			}

			TransparentBlt(hdc, printpos.x + TILE_ACROSS_WIDTH - FuluWidth, printpos.y - (9 * TILE_ACROSS_SURFACE_HEIGHT + TILE_ACROSS_HEIGHT + 4 * TILE_STRAIGHT_SURFACE_HEIGHT + 5 * TILE_NEWSPACE + 13 * TILE_SPACE), 
				FuluWidth, FuluHeight, hFuluDC, 0, 0, FuluWidth, FuluHeight, RGB(255, 255, 255));
			FuluImage.ReleaseDC();
			FuluImage.Destroy();
			
		}
		//�԰�ɫΪ͸��ɫ��������ȫ��ͼ��������豸
		TransparentBlt(hdc, printpos.x, printpos.y - shoupaiheight, singletile_width, shoupaiheight,
			hShouPaiImageDC, 0, 0, singletile_width, shoupaiheight, RGB(255, 255, 255));
		ShouPaiImage.ReleaseDC();
		ShouPaiImage.Destroy();
	}
	if (seat == 3)
	{
		//����ȫ��ͼ��
		int singletile_width = visible ? TILE_ACROSS_WIDTH : TILE_ACROSS_THICKNESS;
		int singletile_height = visible ? TILE_ACROSS_HEIGHT : TILE_STRAIGHT_HEIGHT;
		CImage ShouPaiImage;
		int mypai_tilesum = get_tilesum();
		int shoupaiheight = (mypai_tilesum - 1) * (TILE_ACROSS_SURFACE_HEIGHT + TILE_SPACE) + singletile_height;
		ShouPaiImage.Create(singletile_width, shoupaiheight, 32);
		HDC hShouPaiImageDC = ShouPaiImage.GetDC();

		//������ȫ��ͼ��ȫ��Ϳ��
		BitBlt(hShouPaiImageDC, 0, 0, singletile_width, shoupaiheight, NULL, 0, 0, WHITENESS);
		
		//��ʼ��ӡ����
		tiles::print_tiles(hShouPaiImageDC, 0, 0, seat, visible, selected, tile_status);

		//�԰�ɫΪ͸��ɫ��������ȫ��ͼ��������豸
		TransparentBlt(hdc, printpos.x - singletile_width, printpos.y, singletile_width, shoupaiheight,
			hShouPaiImageDC, 0, 0, singletile_width, shoupaiheight, RGB(255, 255, 255));
		ShouPaiImage.ReleaseDC();
		ShouPaiImage.Destroy();

		if (fulu.groupsum > 0)
		{
			CImage FuluImage;
			int FuluWidth = TILE_ACROSS_WIDTH, FuluHeight = 0;
			for (int i = 0; i < fulu.groupsum; i++)
			{
				switch(fulu.allgroup[i].thistype)
				{
					case shun:
					case ke:
						FuluHeight += 2 * (TILE_ACROSS_SURFACE_HEIGHT + TILE_SPACE) + TILE_STRAIGHT_SURFACE_HEIGHT;
						break;
					case kan:
						if (fulu.allgroup[i].other_num == -1)		// �Ӹ�
						{
							FuluWidth = 2 * TILE_STRAIGHT_HEIGHT + TILE_SPACE;
							FuluHeight += 2 * (TILE_ACROSS_SURFACE_HEIGHT + TILE_SPACE) + TILE_STRAIGHT_SURFACE_HEIGHT;
						}
						else
							FuluHeight += 3 * (TILE_ACROSS_SURFACE_HEIGHT + TILE_SPACE) + TILE_STRAIGHT_SURFACE_HEIGHT;
						break;
					case ankan:
						FuluHeight += 3 * (TILE_ACROSS_SURFACE_HEIGHT + TILE_SPACE) + TILE_ACROSS_SURFACE_HEIGHT;
						break;
				}
				FuluHeight += TILE_NEWSPACE;
			}
			FuluHeight += (TILE_ACROSS_HEIGHT - TILE_ACROSS_SURFACE_HEIGHT);
			FuluImage.Create(FuluWidth, FuluHeight, 32);
			
			int FuluPrintY = 0;
			HDC hFuluDC = FuluImage.GetDC();
			BitBlt(hFuluDC, 0, 0, FuluWidth, FuluHeight, NULL, 0, 0, WHITENESS);
			
			for (int i = fulu.groupsum - 1; i >= 0; i--)
			{
				fulugroup ThisGroup = fulu.allgroup[i];
				CImage ThisFuluImage;
				int ThisFuluHeight;
				switch (ThisGroup.thistype)
				{
				case shun:
					{
						ThisFuluHeight = 2 * (TILE_ACROSS_SURFACE_HEIGHT + TILE_SPACE) + TILE_STRAIGHT_HEIGHT;
						ThisFuluImage.Create(TILE_ACROSS_WIDTH, ThisFuluHeight, 32);
						HDC hThisFuluDC = ThisFuluImage.GetDC();
						BitBlt(hThisFuluDC, 0, 0, ThisFuluImage.GetWidth(), ThisFuluImage.GetHeight(), NULL, 0, 0, WHITENESS);
						int PrintPosY = 0;
						CImage ThisTileImage;
						TileToImage(ThisTileImage, ThisGroup.keytype, ThisGroup.other_num, seat + 1, true);
						HDC hThisTileImageDC = ThisTileImage.GetDC();
						BitBlt(hThisFuluDC, 0, PrintPosY, ThisTileImage.GetWidth(), ThisTileImage.GetHeight(),
							hThisTileImageDC, 0, 0, SRCCOPY);
						PrintPosY += TILE_STRAIGHT_SURFACE_HEIGHT + TILE_SPACE;
						ThisTileImage.ReleaseDC();
						for (int j = 0; j < 3; j++)
						{
							if (j == ThisGroup.other_num - ThisGroup.key) continue;
							TileToImage(ThisTileImage, ThisGroup.keytype, ThisGroup.key + j, seat, true);
							HDC hThisTileImageDC = ThisTileImage.GetDC();
							BitBlt(hThisFuluDC, 0, PrintPosY, ThisTileImage.GetWidth(), ThisTileImage.GetHeight(),
								hThisTileImageDC, 0, 0, SRCCOPY);
							PrintPosY += TILE_ACROSS_SURFACE_HEIGHT + TILE_SPACE;
							ThisTileImage.ReleaseDC();
						}
						ThisTileImage.Destroy();
						ThisFuluImage.ReleaseDC();
					}
					break;
				case ke:
					{
						ThisFuluHeight = 2 * (TILE_ACROSS_SURFACE_HEIGHT + TILE_SPACE) + TILE_STRAIGHT_HEIGHT;
						ThisFuluImage.Create(TILE_ACROSS_WIDTH, ThisFuluHeight, 32);
						HDC hThisFuluDC = ThisFuluImage.GetDC();
						BitBlt(hThisFuluDC, 0, 0, ThisFuluImage.GetWidth(), ThisFuluImage.GetHeight(), NULL, 0, 0, WHITENESS);
						int PrintPosY = 0;
						for (int j = 0; j < 3; j++)
						{
							CImage ThisTileImage;
							TileToImage(ThisTileImage, ThisGroup.keytype, ThisGroup.key, seat + (j == 3 - ThisGroup.seat_pos), true);
							HDC hThisTileImageDC = ThisTileImage.GetDC();
							BitBlt(hThisFuluDC, 0, PrintPosY, ThisTileImage.GetWidth(), ThisTileImage.GetHeight(),
								hThisTileImageDC, 0, 0, SRCCOPY);
							PrintPosY += (j == 3 - ThisGroup.seat_pos ? TILE_STRAIGHT_SURFACE_HEIGHT : TILE_ACROSS_SURFACE_HEIGHT) + TILE_SPACE;
							ThisTileImage.ReleaseDC();
							ThisTileImage.Destroy();
						}
						
						ThisFuluImage.ReleaseDC();
					}
					break;
				case kan:
					{
						if(ThisGroup.other_num == -1)		// �Ӹ�
						{
							ThisFuluHeight = 2 * (TILE_ACROSS_SURFACE_HEIGHT + TILE_SPACE) + TILE_STRAIGHT_HEIGHT;
							ThisFuluImage.Create(TILE_STRAIGHT_WIDTH * 2 + TILE_SPACE, ThisFuluHeight, 32);
							HDC hThisFuluDC = ThisFuluImage.GetDC();
							BitBlt(hThisFuluDC, 0, 0, ThisFuluImage.GetWidth(), ThisFuluImage.GetHeight(), NULL, 0, 0, WHITENESS);
							int PrintPosY = 0;
							for (int j = 0; j < 3; j++)
							{
								CImage ThisTileImage;
								TileToImage(ThisTileImage, ThisGroup.keytype, ThisGroup.key, seat + (j == 3 - ThisGroup.seat_pos), true);
								HDC hThisTileImageDC = ThisTileImage.GetDC();
								if (j == 3 - ThisGroup.seat_pos)
								{
									BitBlt(hThisFuluDC, TILE_STRAIGHT_WIDTH + TILE_SPACE, PrintPosY, ThisTileImage.GetWidth(), ThisTileImage.GetHeight(),
										hThisTileImageDC, 0, 0, SRCCOPY);
								}
								BitBlt(hThisFuluDC, 0, PrintPosY, 
									ThisTileImage.GetWidth(), ThisTileImage.GetHeight(), hThisTileImageDC, 0, 0, SRCCOPY);
								PrintPosY += (j == 3 - ThisGroup.seat_pos ? TILE_STRAIGHT_SURFACE_HEIGHT : TILE_ACROSS_SURFACE_HEIGHT) + TILE_SPACE;
								ThisTileImage.ReleaseDC();
								ThisTileImage.Destroy();
							}
							ThisFuluImage.ReleaseDC();
						}
						else			// ������
						{
							ThisFuluHeight = 2 * (TILE_ACROSS_SURFACE_HEIGHT + TILE_SPACE) + TILE_STRAIGHT_HEIGHT;
							ThisFuluImage.Create(TILE_ACROSS_WIDTH, ThisFuluHeight, 32);
							HDC hThisFuluDC = ThisFuluImage.GetDC();
							BitBlt(hThisFuluDC, 0, 0, ThisFuluImage.GetWidth(), ThisFuluImage.GetHeight(), NULL, 0, 0, WHITENESS);
							int PrintPosY = 0;
							for (int j = 0; j < 4; j++)
							{
								CImage ThisTileImage;
								bool fromthisway = false;
								fromthisway = (j == 3 && ThisGroup.seat_pos == 1) || (j == 2 && ThisGroup.seat_pos == 2) || (j == 0 && ThisGroup.seat_pos == 3);
								TileToImage(ThisTileImage, ThisGroup.keytype, ThisGroup.key, seat + fromthisway, true);
								HDC hThisTileImageDC = ThisTileImage.GetDC();
								BitBlt(hThisFuluDC, TILE_ACROSS_WIDTH - ThisTileImage.GetWidth(), PrintPosY, ThisTileImage.GetWidth(), ThisTileImage.GetHeight(),
									hThisTileImageDC, 0, 0, SRCCOPY);
								PrintPosY += (fromthisway ? TILE_STRAIGHT_SURFACE_HEIGHT : TILE_ACROSS_SURFACE_HEIGHT)  + TILE_SPACE;
								ThisTileImage.ReleaseDC();
								ThisTileImage.Destroy();
							}
							ThisFuluImage.ReleaseDC();
						}
					}
					break;
				case ankan:
					{
						ThisFuluHeight = 3 * (TILE_ACROSS_SURFACE_HEIGHT + TILE_SPACE) + TILE_ACROSS_HEIGHT;
						ThisFuluImage.Create(TILE_ACROSS_WIDTH, ThisFuluHeight, 32);
						HDC hThisFuluDC = ThisFuluImage.GetDC();
						BitBlt(hThisFuluDC, 0, 0, ThisFuluImage.GetWidth(), ThisFuluImage.GetHeight(), NULL, 0, 0, WHITENESS);
						int PrintPosY = 0;
						for (int j = 0; j < 4; j++)
						{
							CImage ThisTileImage;
							if (j == 0 || j == 3)
								TileToImage(ThisTileImage, ThisGroup.keytype, ThisGroup.key, -3, false);
							else
								TileToImage(ThisTileImage, ThisGroup.keytype, ThisGroup.key, seat, true);
							HDC hThisTileImageDC = ThisTileImage.GetDC();
							BitBlt(hThisFuluDC, 0, PrintPosY, ThisTileImage.GetWidth(), ThisTileImage.GetHeight(),
								hThisTileImageDC, 0, 0, SRCCOPY);
							PrintPosY += TILE_ACROSS_SURFACE_HEIGHT + TILE_SPACE;
							ThisTileImage.ReleaseDC();
							ThisTileImage.Destroy();
						}
						ThisFuluImage.ReleaseDC();
					}
					break;
				}
				HDC hThisFuluDC = ThisFuluImage.GetDC();
				//int DeltaWidth = (FuluWidth - TILE_ACROSS_WIDTH) * !(ThisGroup.thistype == kan && ThisGroup.other_num == -1);
				TransparentBlt(hFuluDC, 0, FuluPrintY, ThisFuluImage.GetWidth(), ThisFuluImage.GetHeight(), hThisFuluDC, 0, 0, ThisFuluImage.GetWidth(), ThisFuluImage.GetHeight(), RGB(255, 255, 255));
				FuluPrintY += ThisFuluHeight - (TILE_ACROSS_HEIGHT - TILE_ACROSS_SURFACE_HEIGHT) + TILE_NEWSPACE;
				ThisFuluImage.ReleaseDC();
				ThisFuluImage.Destroy();
			}
			
			TransparentBlt(hdc, printpos.x - TILE_ACROSS_WIDTH, printpos.y + 9 * TILE_ACROSS_SURFACE_HEIGHT + TILE_ACROSS_HEIGHT + 4 * TILE_STRAIGHT_SURFACE_HEIGHT + 5 * TILE_NEWSPACE + 13 * TILE_SPACE - FuluHeight, 
				FuluWidth, FuluHeight, hFuluDC, 0, 0, FuluWidth, FuluHeight, RGB(255, 255, 255));
			FuluImage.ReleaseDC();
			FuluImage.Destroy();
			
		}
	}
}

void pai::print_tiles(HDC hdc, int x, int y, int seat, bool visible, int selected)
{
	POINT temppoint = {x, y};
	print_tiles(hdc, temppoint, seat, visible, selected);
}

void pai::print_river(HDC hdc, POINT printpos, int seat)
{
	if (river.paihe_visiblesum == 0)
		return;
	if (seat == 0)
	{
		static CImage RiverImageCrash;
		static int RecordTileSum;
		static int BeginTileNum;
		static POINT BeginPos;
		if (river.paihe_visiblesum == 1 && !RiverImageCrash.IsNull())
		{
			RiverImageCrash.Destroy();
			RecordTileSum = 0;
			BeginTileNum = 0;
			BeginPos.x = 0;
			BeginPos.y = 0;
		}
		CImage RiverImage;
		if (river.paihe_visiblesum <= 18)
			RiverImage.Create((TILE_STRAIGHT_WIDTH + TILE_SPACE) * 5 + (TILE_ACROSS_WIDTH + TILE_SPACE),
				((river.paihe_visiblesum - 1) / 6) * (TILE_STRAIGHT_SURFACE_HEIGHT + TILE_SPACE) + TILE_STRAIGHT_HEIGHT, 32);
		else
			RiverImage.Create((TILE_STRAIGHT_WIDTH + TILE_SPACE) * (river.paihe_visiblesum - 18 + 5) + (TILE_ACROSS_WIDTH + TILE_SPACE),
				2 * (TILE_STRAIGHT_SURFACE_HEIGHT + TILE_SPACE) + TILE_STRAIGHT_HEIGHT, 32);
		HDC hRiverDC = RiverImage.GetDC();
		BitBlt(hRiverDC, 0, 0, RiverImage.GetWidth(), RiverImage.GetHeight(), NULL, 0, 0, WHITENESS);
		int haveprintnum = 0;
		POINT thispos = {0, 0};

		if (!RiverImageCrash.IsNull())
		{
			HDC hRiverImageCrashDC = RiverImageCrash.GetDC();
			BitBlt(hRiverDC, 0, 0, RiverImageCrash.GetWidth(), RiverImageCrash.GetHeight(), hRiverImageCrashDC, 0, 0, SRCCOPY);
			RiverImageCrash.ReleaseDC();
			haveprintnum = RecordTileSum;
			thispos.x = BeginPos.x;
			thispos.y = BeginPos.y;
		}
		for (int i = BeginTileNum; i < river.paihesum; i++)
		{
			paihetile thistile = river.paihe_tile[i];
			if (thistile.visible == false) continue;
			CImage ThisTileImage;
			if (haveprintnum == river.riichitile_num)
			{
				TileToImage(ThisTileImage, thistile.type, thistile.num, 1, true);
			}
			else
			{
				TileToImage(ThisTileImage, thistile.type, thistile.num, 0, true);
			}
			HDC hThisTileDC = ThisTileImage.GetDC();
			BitBlt(hRiverDC, thispos.x, thispos.y + (haveprintnum == river.riichitile_num) * (TILE_STRAIGHT_HEIGHT - TILE_ACROSS_HEIGHT) / 2, 
				ThisTileImage.GetWidth(), ThisTileImage.GetHeight(), hThisTileDC, 0, 0, SRCCOPY);
			haveprintnum ++;
			if (haveprintnum < 18 && haveprintnum % 6 == 0)
			{
				thispos.x = 0;
				thispos.y += TILE_STRAIGHT_SURFACE_HEIGHT + TILE_SPACE;
			}
			else
			{
				thispos.x += ThisTileImage.GetWidth() + TILE_SPACE;
			}
			ThisTileImage.ReleaseDC();
			ThisTileImage.Destroy();
			if (haveprintnum == river.paihe_visiblesum - 2)
			{
				if (!RiverImageCrash.IsNull())
					RiverImageCrash.Destroy();
				RiverImageCrash.Create(RiverImage.GetWidth(), RiverImage.GetHeight(), 32);
				HDC hRiverImageCrashDC = RiverImageCrash.GetDC();
				BitBlt(hRiverImageCrashDC, 0, 0, RiverImageCrash.GetWidth(), RiverImageCrash.GetHeight(), NULL, 0, 0, WHITENESS);
				BitBlt(hRiverImageCrashDC, 0, 0, RiverImageCrash.GetWidth(), RiverImageCrash.GetHeight(), hRiverDC, 0, 0, SRCCOPY);
				RiverImageCrash.ReleaseDC();
				RecordTileSum = river.paihe_visiblesum - 2;
				BeginTileNum = i + 1;
				BeginPos.x = thispos.x;
				BeginPos.y = thispos.y;
			}
		}
		TransparentBlt(hdc, printpos.x, printpos.y, RiverImage.GetWidth(), RiverImage.GetHeight(),
			hRiverDC, 0, 0, RiverImage.GetWidth(), RiverImage.GetHeight(), RGB(255, 255, 255));
		RiverImage.ReleaseDC();
		RiverImage.Destroy();
	}
	if (seat == 2)
	{
		static CImage RiverImageCrash;
		static int EndTileNum;
		if (river.paihe_visiblesum == 1 && !RiverImageCrash.IsNull())
		{
			RiverImageCrash.Destroy();
			EndTileNum = 0;
		}
		CImage RiverImage;
		if (river.paihe_visiblesum <= 18)
			RiverImage.Create((TILE_STRAIGHT_WIDTH + TILE_SPACE) * 5 + (TILE_ACROSS_WIDTH + TILE_SPACE),
				((river.paihe_visiblesum - 1) / 6) * (TILE_STRAIGHT_SURFACE_HEIGHT + TILE_SPACE) + TILE_STRAIGHT_HEIGHT, 32);
		else
			RiverImage.Create((TILE_STRAIGHT_WIDTH + TILE_SPACE) * (river.paihe_visiblesum - 18 + 5) + (TILE_ACROSS_WIDTH + TILE_SPACE),
				2 * (TILE_STRAIGHT_SURFACE_HEIGHT + TILE_SPACE) + TILE_STRAIGHT_HEIGHT, 32);
		HDC hRiverDC = RiverImage.GetDC();
		BitBlt(hRiverDC, 0, 0, RiverImage.GetWidth(), RiverImage.GetHeight(), NULL, 0, 0, WHITENESS);
		int haveprintnum = 0;
		POINT thispos = {0, 0};
		if (river.paihe_visiblesum <= 18)
			thispos.x = RiverImage.GetWidth() - ((river.paihe_visiblesum - 1) % 6) * (TILE_STRAIGHT_WIDTH + TILE_SPACE)
				- ((river.paihe_visiblesum - 1) / 6 == river.riichitile_num / 6 && river.riichitile_num != -1 && river.paihe_visiblesum > river.riichitile_num ? TILE_ACROSS_WIDTH : TILE_STRAIGHT_WIDTH);
		else
			thispos.x = river.riichitile_num >= 12 ? 0 : TILE_ACROSS_WIDTH - TILE_STRAIGHT_WIDTH;
		for (int i = river.paihesum - 1; i >= EndTileNum; i--)
		{
			paihetile thistile = river.paihe_tile[i];
			if (thistile.visible == false) continue;
			if (haveprintnum == 2)
			{
				HDC hRiverImageCrashDC;
				CImage CopyTempImage;
				if (!RiverImageCrash.IsNull())
				{
					CopyTempImage.Create(RiverImageCrash.GetWidth(), RiverImageCrash.GetHeight(), 32);
					HDC hCopyTempDC = CopyTempImage.GetDC();
					hRiverImageCrashDC = RiverImageCrash.GetDC();
					BitBlt(hCopyTempDC, 0, 0, CopyTempImage.GetWidth(), CopyTempImage.GetHeight(), NULL, 0, 0, WHITENESS);
					BitBlt(hCopyTempDC, 0, 0, RiverImageCrash.GetWidth(), RiverImageCrash.GetHeight(), hRiverImageCrashDC, 0, 0, SRCCOPY);
					CopyTempImage.ReleaseDC();
					RiverImageCrash.ReleaseDC();
					RiverImageCrash.Destroy();
				}
				RiverImageCrash.Create(RiverImage.GetWidth(), RiverImage.GetHeight(), 32);
				hRiverImageCrashDC = RiverImageCrash.GetDC();
				BitBlt(hRiverImageCrashDC, 0, 0, RiverImageCrash.GetWidth(), RiverImageCrash.GetHeight(), NULL, 0, 0, WHITENESS);
				EndTileNum = i + 1;
				CImage ThisTileImage;
				if (river.paihe_visiblesum - 1 - haveprintnum == river.riichitile_num)
				{
					TileToImage(ThisTileImage, thistile.type, thistile.num, 3, true);
					HDC hThisTileDC = ThisTileImage.GetDC();
					BitBlt(hRiverImageCrashDC, thispos.x, thispos.y + (TILE_STRAIGHT_HEIGHT - TILE_ACROSS_HEIGHT) / 2, 
					ThisTileImage.GetWidth(), ThisTileImage.GetHeight(), hThisTileDC, 0, 0, SRCCOPY);
					ThisTileImage.ReleaseDC();
					ThisTileImage.Destroy();
				}
				else
				{
					TileToImage(ThisTileImage, thistile.type, thistile.num, 2, true);
					HDC hThisTileDC = ThisTileImage.GetDC();
					BitBlt(hRiverImageCrashDC, thispos.x, thispos.y, 
					ThisTileImage.GetWidth(), ThisTileImage.GetHeight(), hThisTileDC, 0, 0, SRCCOPY);
					ThisTileImage.ReleaseDC();
					ThisTileImage.Destroy();
				}
				if (!CopyTempImage.IsNull())
				{
					HDC hCopyTempDC = CopyTempImage.GetDC();
					TransparentBlt(hRiverImageCrashDC, RiverImageCrash.GetWidth() - CopyTempImage.GetWidth(), RiverImageCrash.GetHeight() - CopyTempImage.GetHeight(), 
						CopyTempImage.GetWidth(), CopyTempImage.GetHeight(), hCopyTempDC, 0, 0, CopyTempImage.GetWidth(), CopyTempImage.GetHeight(), RGB(255, 255, 255));
					CopyTempImage.ReleaseDC();
				}
				RiverImageCrash.ReleaseDC();
				break;
			}
			CImage ThisTileImage;
			if (river.paihe_visiblesum - 1 - haveprintnum == river.riichitile_num)
			{
				TileToImage(ThisTileImage, thistile.type, thistile.num, 3, true);
				HDC hThisTileDC = ThisTileImage.GetDC();
				BitBlt(hRiverDC, thispos.x, thispos.y + (TILE_STRAIGHT_HEIGHT - TILE_ACROSS_HEIGHT) / 2, 
				ThisTileImage.GetWidth(), ThisTileImage.GetHeight(), hThisTileDC, 0, 0, SRCCOPY);
				ThisTileImage.ReleaseDC();
			}
			else
			{
				TileToImage(ThisTileImage, thistile.type, thistile.num, 2, true);
				HDC hThisTileDC = ThisTileImage.GetDC();
				BitBlt(hRiverDC, thispos.x, thispos.y, 
				ThisTileImage.GetWidth(), ThisTileImage.GetHeight(), hThisTileDC, 0, 0, SRCCOPY);
				ThisTileImage.ReleaseDC();
			}
			haveprintnum ++;
			int remainprintnum = river.paihe_visiblesum - haveprintnum;
			if (remainprintnum < 18 && remainprintnum % 6 == 0)
			{
				thispos.x = RiverImage.GetWidth() - 5 * (TILE_STRAIGHT_WIDTH + TILE_SPACE)
					- ((remainprintnum - 1) / 6 == river.riichitile_num / 6 && river.riichitile_num != -1 ? TILE_ACROSS_WIDTH : TILE_STRAIGHT_WIDTH);
				thispos.y += TILE_STRAIGHT_SURFACE_HEIGHT + TILE_SPACE;
			}
			else
			{
				thispos.x += ThisTileImage.GetWidth() + TILE_SPACE;
			}
			ThisTileImage.Destroy();
		}
		if (!RiverImageCrash.IsNull())
		{
			HDC hRiverImageCrashDC = RiverImageCrash.GetDC();
			TransparentBlt(hRiverDC, RiverImage.GetWidth() - RiverImageCrash.GetWidth(), RiverImage.GetHeight() - RiverImageCrash.GetHeight(), RiverImageCrash.GetWidth(), RiverImageCrash.GetHeight(), 
					hRiverImageCrashDC, 0, 0, RiverImageCrash.GetWidth(), RiverImageCrash.GetHeight(), RGB(255, 255, 255));
			RiverImageCrash.ReleaseDC();
		}
		TransparentBlt(hdc, printpos.x - RiverImage.GetWidth(), printpos.y - RiverImage.GetHeight(), RiverImage.GetWidth(), RiverImage.GetHeight(),
			hRiverDC, 0, 0, RiverImage.GetWidth(), RiverImage.GetHeight(), RGB(255, 255, 255));
		RiverImage.ReleaseDC();
		RiverImage.Destroy();
	}
	if (seat == 1)
	{
		static CImage RiverImageCrash;
		static int EndTileNum;
		if (river.paihe_visiblesum == 1 && !RiverImageCrash.IsNull())
		{
			RiverImageCrash.Destroy();
			EndTileNum = 0;
		}
		CImage RiverImage;
		if (river.paihe_visiblesum <= 18)
			RiverImage.Create(((river.paihe_visiblesum - 1) / 6) * (TILE_ACROSS_WIDTH + TILE_SPACE) + TILE_ACROSS_WIDTH,
			(TILE_ACROSS_SURFACE_HEIGHT + TILE_SPACE) * 5 + TILE_STRAIGHT_HEIGHT, 32);
		else
			RiverImage.Create(2 * (TILE_ACROSS_WIDTH + TILE_SPACE) + TILE_ACROSS_WIDTH, 
			(TILE_ACROSS_SURFACE_HEIGHT + TILE_SPACE) * (river.paihe_visiblesum - 18 + 5) + TILE_STRAIGHT_HEIGHT, 32);
		HDC hRiverDC = RiverImage.GetDC();
		BitBlt(hRiverDC, 0, 0, RiverImage.GetWidth(), RiverImage.GetHeight(), NULL, 0, 0, WHITENESS);
		int haveprintnum = 0;
		POINT thispos = {RiverImage.GetWidth() - TILE_ACROSS_WIDTH, 0};
		if (river.paihe_visiblesum <= 18)
			thispos.y = RiverImage.GetHeight() - ((river.paihe_visiblesum - 1) % 6) * (TILE_ACROSS_SURFACE_HEIGHT + TILE_SPACE)
				- ((river.paihe_visiblesum - 1) / 6 == river.riichitile_num / 6 && river.riichitile_num != -1 && river.paihe_visiblesum > river.riichitile_num ? TILE_STRAIGHT_HEIGHT : TILE_ACROSS_HEIGHT);
		else
			thispos.y = river.riichitile_num >= 12 ? 0 : TILE_STRAIGHT_HEIGHT - TILE_ACROSS_HEIGHT;
		for (int i = river.paihesum - 1; i >= EndTileNum; i--)
		{
			paihetile thistile = river.paihe_tile[i];
			if (thistile.visible == false) continue;
			if (haveprintnum == 2)
			{
				HDC hRiverImageCrashDC;
				CImage CopyTempImage;
				if (!RiverImageCrash.IsNull())
				{
					CopyTempImage.Create(RiverImageCrash.GetWidth(), RiverImageCrash.GetHeight(), 32);
					HDC hCopyTempDC = CopyTempImage.GetDC();
					hRiverImageCrashDC = RiverImageCrash.GetDC();
					BitBlt(hCopyTempDC, 0, 0, CopyTempImage.GetWidth(), CopyTempImage.GetHeight(), NULL, 0, 0, WHITENESS);
					BitBlt(hCopyTempDC, 0, 0, RiverImageCrash.GetWidth(), RiverImageCrash.GetHeight(), hRiverImageCrashDC, 0, 0, SRCCOPY);
					CopyTempImage.ReleaseDC();
					RiverImageCrash.ReleaseDC();
					RiverImageCrash.Destroy();
				}
				RiverImageCrash.Create(RiverImage.GetWidth(), RiverImage.GetHeight(), 32);
				hRiverImageCrashDC = RiverImageCrash.GetDC();
				BitBlt(hRiverImageCrashDC, 0, 0, RiverImageCrash.GetWidth(), RiverImageCrash.GetHeight(), NULL, 0, 0, WHITENESS);
				EndTileNum = i + 1;
				CImage ThisTileImage;
				if (river.paihe_visiblesum - 1 - haveprintnum == river.riichitile_num)
				{
					TileToImage(ThisTileImage, thistile.type, thistile.num, seat + 1, true);
					HDC hThisTileDC = ThisTileImage.GetDC();
					BitBlt(hRiverImageCrashDC, thispos.x + (TILE_ACROSS_WIDTH - TILE_STRAIGHT_WIDTH) / 2, thispos.y, 
					ThisTileImage.GetWidth(), ThisTileImage.GetHeight(), hThisTileDC, 0, 0, SRCCOPY);
					ThisTileImage.ReleaseDC();
					ThisTileImage.Destroy();
				}
				else
				{
					TileToImage(ThisTileImage, thistile.type, thistile.num, seat, true);
					HDC hThisTileDC = ThisTileImage.GetDC();
					BitBlt(hRiverImageCrashDC, thispos.x, thispos.y, 
					ThisTileImage.GetWidth(), ThisTileImage.GetHeight(), hThisTileDC, 0, 0, SRCCOPY);
					ThisTileImage.ReleaseDC();
					ThisTileImage.Destroy();
				}
				if (!CopyTempImage.IsNull())
				{
					HDC hCopyTempDC = CopyTempImage.GetDC();
					TransparentBlt(hRiverImageCrashDC, 0, RiverImageCrash.GetHeight() - CopyTempImage.GetHeight(), 
						CopyTempImage.GetWidth(), CopyTempImage.GetHeight(), hCopyTempDC, 0, 0, CopyTempImage.GetWidth(), CopyTempImage.GetHeight(), RGB(255, 255, 255));
					CopyTempImage.ReleaseDC();
					CopyTempImage.Destroy();
				}
				RiverImageCrash.ReleaseDC();

				break;
			}
			CImage ThisTileImage;
			if (river.paihe_visiblesum - 1 - haveprintnum == river.riichitile_num)
			{
				TileToImage(ThisTileImage, thistile.type, thistile.num, seat + 1, true);
				HDC hThisTileDC = ThisTileImage.GetDC();
				BitBlt(hRiverDC, thispos.x + (TILE_ACROSS_WIDTH - TILE_STRAIGHT_WIDTH) / 2, thispos.y, 
				ThisTileImage.GetWidth(), ThisTileImage.GetHeight(), hThisTileDC, 0, 0, SRCCOPY);
				ThisTileImage.ReleaseDC();
				ThisTileImage.Destroy();
			}
			else
			{
				TileToImage(ThisTileImage, thistile.type, thistile.num, seat, true);
				HDC hThisTileDC = ThisTileImage.GetDC();
				BitBlt(hRiverDC, thispos.x, thispos.y, 
				ThisTileImage.GetWidth(), ThisTileImage.GetHeight(), hThisTileDC, 0, 0, SRCCOPY);
				ThisTileImage.ReleaseDC();
				ThisTileImage.Destroy();
			}
			haveprintnum ++;
			int remainprintnum = river.paihe_visiblesum - haveprintnum;
			if (remainprintnum < 18 && remainprintnum % 6 == 0)
			{
				thispos.y = RiverImage.GetHeight() - 5 * (TILE_ACROSS_SURFACE_HEIGHT + TILE_SPACE)
					- ((remainprintnum - 1) / 6 == river.riichitile_num / 6 && river.riichitile_num != -1 ? TILE_STRAIGHT_HEIGHT : TILE_ACROSS_HEIGHT);
				thispos.x -= TILE_ACROSS_WIDTH + TILE_SPACE;
			}
			else
			{
				thispos.y += (remainprintnum == river.riichitile_num ? TILE_STRAIGHT_SURFACE_HEIGHT : TILE_ACROSS_SURFACE_HEIGHT) + TILE_SPACE;
			}
			
		}
		if (!RiverImageCrash.IsNull())
		{
			HDC hRiverImageCrashDC = RiverImageCrash.GetDC();
			bool flag = TransparentBlt(hRiverDC, 0, RiverImage.GetHeight() - RiverImageCrash.GetHeight(), RiverImageCrash.GetWidth(), RiverImageCrash.GetHeight(), 
					hRiverImageCrashDC, 0, 0, RiverImageCrash.GetWidth(), RiverImageCrash.GetHeight(), RGB(255, 255, 255));
			RiverImageCrash.ReleaseDC();
		}
		TransparentBlt(hdc, printpos.x, printpos.y - RiverImage.GetHeight(), RiverImage.GetWidth(), RiverImage.GetHeight(),
			hRiverDC, 0, 0, RiverImage.GetWidth(), RiverImage.GetHeight(), RGB(255, 255, 255));
		RiverImage.ReleaseDC();
		RiverImage.Destroy();
	}
	if (seat == 3)
	{
		static CImage RiverImageCrash;
		static int RecordTileSum;
		static int BeginTileNum;
		static POINT BeginPos;
		if (river.paihe_visiblesum == 1 && !RiverImageCrash.IsNull())
		{
			RiverImageCrash.Destroy();
			RecordTileSum = 0;
			BeginTileNum = 0;
			BeginPos.x = 0;
			BeginPos.y = 0;
		}
		CImage RiverImage;
		if (river.paihe_visiblesum <= 18)
			RiverImage.Create(((river.paihe_visiblesum - 1) / 6) * (TILE_ACROSS_WIDTH + TILE_SPACE) + TILE_ACROSS_WIDTH,
			(TILE_ACROSS_SURFACE_HEIGHT + TILE_SPACE) * 5 + TILE_STRAIGHT_HEIGHT, 32);
		else
			RiverImage.Create(2 * (TILE_ACROSS_WIDTH + TILE_SPACE) + TILE_ACROSS_WIDTH, 
			(TILE_ACROSS_SURFACE_HEIGHT + TILE_SPACE) * (river.paihe_visiblesum - 18 + 5) + TILE_STRAIGHT_HEIGHT, 32);
		HDC hRiverDC = RiverImage.GetDC();
		BitBlt(hRiverDC, 0, 0, RiverImage.GetWidth(), RiverImage.GetHeight(), NULL, 0, 0, WHITENESS);
		int haveprintnum = 0;
		POINT thispos = {RiverImage.GetWidth() - TILE_ACROSS_WIDTH, 0};

		if (!RiverImageCrash.IsNull())
		{
			HDC hRiverImageCrashDC = RiverImageCrash.GetDC();
			BitBlt(hRiverDC, RiverImage.GetWidth() - RiverImageCrash.GetWidth(), 0, RiverImageCrash.GetWidth(), RiverImageCrash.GetHeight(), hRiverImageCrashDC, 0, 0, SRCCOPY);
			RiverImageCrash.ReleaseDC();
			haveprintnum = RecordTileSum;
			thispos.x = BeginPos.x + RiverImage.GetWidth() - RiverImageCrash.GetWidth();
			thispos.y = BeginPos.y;
		}
		for (int i = BeginTileNum; i < river.paihesum; i++)
		{
			paihetile thistile = river.paihe_tile[i];
			if (thistile.visible == false)
				continue;
			CImage ThisTileImage;
			if (haveprintnum == river.riichitile_num)
			{
				TileToImage(ThisTileImage, thistile.type, thistile.num, seat + 1, true);
				HDC hThisTileDC = ThisTileImage.GetDC();
				BitBlt(hRiverDC, thispos.x + (TILE_ACROSS_WIDTH - TILE_STRAIGHT_WIDTH) / 2, thispos.y, 
				ThisTileImage.GetWidth(), ThisTileImage.GetHeight(), hThisTileDC, 0, 0, SRCCOPY);
				ThisTileImage.ReleaseDC();
				ThisTileImage.Destroy();
			}
			else
			{
				TileToImage(ThisTileImage, thistile.type, thistile.num, seat, true);
				HDC hThisTileDC = ThisTileImage.GetDC();
				BitBlt(hRiverDC, thispos.x, thispos.y, 
				ThisTileImage.GetWidth(), ThisTileImage.GetHeight(), hThisTileDC, 0, 0, SRCCOPY);
				ThisTileImage.ReleaseDC();
				ThisTileImage.Destroy();
			}
			haveprintnum ++;
			
			if (haveprintnum < 18 && haveprintnum % 6 == 0)
			{
				thispos.y = 0;
				thispos.x -= TILE_ACROSS_WIDTH + TILE_SPACE;
			}
			else
			{
				thispos.y += (haveprintnum - 1 == river.riichitile_num ? TILE_STRAIGHT_SURFACE_HEIGHT : TILE_ACROSS_SURFACE_HEIGHT) + TILE_SPACE;
			}
			if (haveprintnum == river.paihe_visiblesum - 2)
			{
				if (!RiverImageCrash.IsNull())
					RiverImageCrash.Destroy();
				RiverImageCrash.Create(RiverImage.GetWidth(), RiverImage.GetHeight(), 32);
				HDC hRiverImageCrashDC = RiverImageCrash.GetDC();
				BitBlt(hRiverImageCrashDC, 0, 0, RiverImageCrash.GetWidth(), RiverImageCrash.GetHeight(), NULL, 0, 0, WHITENESS);
				BitBlt(hRiverImageCrashDC, 0, 0, RiverImageCrash.GetWidth(), RiverImageCrash.GetHeight(), hRiverDC, 0, 0, SRCCOPY);
				RiverImageCrash.ReleaseDC();
				RecordTileSum = river.paihe_visiblesum - 2;
				BeginTileNum = i + 1;
				BeginPos.x = thispos.x;
				BeginPos.y = thispos.y;
			}
		}
		TransparentBlt(hdc, printpos.x - RiverImage.GetWidth(), printpos.y, RiverImage.GetWidth(), RiverImage.GetHeight(),
			hRiverDC, 0, 0, RiverImage.GetWidth(), RiverImage.GetHeight(), RGB(255, 255, 255));
		RiverImage.ReleaseDC();
		RiverImage.Destroy();
	}
}

void pai::print_river(HDC hdc, int x, int y, int seat)
{
	POINT temppoint = {x, y};
	print_river(hdc, temppoint, seat);
}

BYTE pai::get_possible_response(matchinfo match_info, direction seat)
{
	BYTE result = 0;
	if (seat == direction((match_info.active_direction + 1) % 4))	// ����������¼ң���ô���Կ��ǳ���
	{
		bool ChiFlag = false;
		for (int chi_way = 0; chi_way < 3 && !ChiFlag; chi_way++)
		{
			bool ThisWayChiFlag = true;
			if (match_info.active_tile.num < 2 - chi_way || match_info.active_tile.num > 8 - chi_way) continue;	// ���ŵĳ��Ʒ�ʽ����
			for (int k = 0; k < 3; k++)
			{
				if (-2 + k + chi_way == 0) continue;	// ������Ҫ�Ե��ƣ�����Ҫ�ж������Ƿ����
				if(get_tilenum(match_info.active_tile.type, match_info.active_tile.num - 2 + k + chi_way) == 0)
					ThisWayChiFlag = false;				// ������в��������ֳԷ�����Ҫ���ƣ���ô���ֳԷ���Ч
			}
			result |= ThisWayChiFlag * (POSSIBILITY_CHI_1 << chi_way);	
		}
	}
	if (get_tilenum(match_info.active_tile) >= 2)	//���г������������ƣ�������
	{
		result |= POSSIBILITY_PON;
		if (get_tilenum(match_info.active_tile) >= 3)	//���г������������ƣ�������
		{
			result |= POSSIBILITY_KAN;
		}
	}
	if (iftingpai() && tingpai_tiles[match_info.active_tile.type * 10 + match_info.active_tile.num] != 0)	// ���ƣ���״�ϣ�
	{
		result |= POSSIBILITY_RON;
	}
	return result;
}

BYTE pai::get_tile_status(int type, int num)
{
	if (type * 10 + num > 36)
		return TILE_NORMAL;
	return tile_status[type * 10 + num];
}

BYTE pai::get_tile_status(singletile thistile)
{
	return get_tile_status(thistile.type, thistile.num);
}

BYTE pai::add_tile_status(int type, int num, BYTE new_status)
{
	if (type * 10 + num > 36)
		return TILE_NORMAL;
	tile_status[type * 10 + num] |= (new_status & TILE_ALLSTATUS);
	return tile_status[type * 10 + num];
}

BYTE pai::add_tile_status(singletile thistile, BYTE new_status)
{
	return add_tile_status(thistile.type, thistile.num, new_status);
}

void pai::remove_tile_status(BYTE goal_status)
{
	for (int i = 0; i < 37; i++)
		tile_status[i] &= ~goal_status;
}

fulugroup pai::get_fulu(int num)
{
	if (num + 1 > fulu.groupsum)
		return fulugroup();
	else
		return fulu.allgroup[num];
}

void pai::add_fulu(fulugroup newfulu, int num)
{
	if (num == -1)
		fulu.allgroup[fulu.groupsum] = newfulu;
	else
	{
		for (int i = fulu.groupsum - 1; i >= num; i--)
			fulu.allgroup[i + 1] = fulu.allgroup[i];
		fulu.allgroup[num] = newfulu;
	}
	fulu.groupsum ++;
	if (newfulu.thistype != ankan)
		fulu.feimenqing = true;
}

void pai::delete_fulu(int num)
{
	if (num == -1)
	{
		if (fulu.groupsum > 0)
			fulu.groupsum --;
	}
	else
	{
		if (fulu.groupsum > num)
		{
			for (int i = num; i < fulu.groupsum - 1; i++)
				fulu.allgroup[i] = fulu.allgroup[i + 1];
			fulu.groupsum --;
		}
	}
	fulu.feimenqing = false;
	for (int i = 0; i < fulu.groupsum; i++)
		if (fulu.allgroup[i].thistype != ankan)
			fulu.feimenqing = true;
}

singletile pai::get_paihetile(int num)
{
	if (num + 1 > river.paihesum)
		return defaulttile;
	else
		return river.paihe_tile[num];
}

void pai::add_paihetile(int type, int num)
{
	singletile temptile(type, num);
	add_paihetile(temptile);
}

void pai::add_paihetile(singletile newpaihetile)
{
	if (river.paihesum == 25)
	{
		river.paihesum = 0;
		river.paihe_visiblesum = 0;
		river.riichitile_num = -1;
	}
	river.paihe_tile[river.paihesum] = newpaihetile;
	river.paihesum ++;
	river.paihe_visiblesum ++;
	
}

void pai::set_paihetile_invisible()
{
	river.paihe_tile[river.paihesum - 1].visible = false;
	river.paihe_visiblesum --;
}

void pai::set_paihetile_riichi()
{
	river.riichitile_num = river.paihe_visiblesum;
}

void pai::post_stop_analysis()
{
	stop_analysis = true;
}

bool pai::iftingpai()
{
	bool tingpai = false;
	pai temppai = *this;
	memset(tingpai_tiles, 0, 37);
	for (int type_num = 0; type_num < 37; type_num ++)
	{
		if (type_num % 10 == 9) continue;
		if (temppai.get_tilenum(type_num / 10, type_num % 10) >= 4) continue;
		temppai.change_tilenum(type_num / 10, type_num % 10, 1);
		if (temppai.ifhu().ifhupai)
		{
			tingpai = true;
			tingpai_tiles[type_num] = 1;
		}
		temppai.change_tilenum(type_num / 10, type_num % 10, -1);
	}
	return tingpai;
}

pai pai::get_tingpaitiles()
{
	pai result;
	if (iftingpai())
		for (int type_num = 0; type_num < 37; type_num ++)
			if (tingpai_tiles[type_num] == 1)
				result.change_tilenum(type_num / 10, type_num % 10, 1);
	return result;
}

hupaiinfo pai::ifhu(singletile lasttile, bool test, UINT nFlags, matchinfo match_info, direction self_direction)
{
	bool tsumo = nFlags & HU_TSUMO;
	bool riichi = nFlags & HU_RIICHI;
	bool w_riichi = nFlags & HU_WRIICHI;
	bool ippatsu = nFlags & HU_IPPATSU;
	bool rinshan = nFlags & HU_RINSHAN;
	bool chankan = nFlags & HU_CHANKAN;
	bool tenhou = nFlags & HU_TENHOU;
	bool haitei = nFlags & HU_HAITEI;

	int groupsum = 0, DuiZiSum = 0, YaoJiuSum = 0;	// ���������������������۾�������
	bool ShiSanYao = true, QiDuiZi = false, normalhu = false, PingHu = true;		// ���������Ƿ�Ϊʮ���ۡ��߶��ӡ��������ơ�ƽ��
	tileinfo_ex typeinfo[2][4];						//typeinfo[0]�洢��������ʱ����Ϣ��typeinfo[1]�洢������ʱ����Ϣ��
	hupaiinfo result = {false, ""};
	tiles alltile = *this;		// ���ϸ�¶���������
	singletile FinalGeneral;						// ����ʱ�Ľ���

	//���ϸ�¶������������
	if (fulu.groupsum > 0)
	{
		groupinfo *pThisGroup = NULL;
		for (int i = 0; i < fulu.groupsum; i++)
		{
			pThisGroup = &fulu.allgroup[i];
			switch(pThisGroup->thistype)
			{
				case shun:
					for (int j = 0; j < 3; j ++)
						alltile.change_tilenum(pThisGroup->keytype, pThisGroup->key + j, 1);
					break;
				case ke:
					alltile.change_tilenum(pThisGroup->keytype, pThisGroup->key, 3);
					break;
				case kan:
				case ankan:
					alltile.change_tilenum(pThisGroup->keytype, pThisGroup->key, 4);
					break;
			}
		}
	}
	//--------------------------�����۾��Ƶ��������ж��Ƿ�Ϊʮ����---------------------------
	for (int i = 0; i < 3; i++)
	{
		YaoJiuSum += alltile.get_tilenum(i, 0);
		YaoJiuSum += alltile.get_tilenum(i, 8);
		if (alltile.get_tilenum(i, 0) == 0 || alltile.get_tilenum(i, 8) == 0)
			ShiSanYao = false;
	}
	for (int i = 0; i < 7; i++)
	{
		YaoJiuSum += alltile.get_tilenum(3, i);
		if (alltile.get_tilenum(3, i) == 0)
			ShiSanYao = false;
	}
	if (fulu.groupsum == 0 && ShiSanYao && YaoJiuSum >= 14)
	{
		result.ifhupai = true;
		strcat_s(result.fanzhong, strlen(result.fanzhong) + 8, "ʮ���� ");
		if (!test)
		{
			memset(result.hutype, '\0', 30);
			strcpy_s(result.hutype, 6, "���� ");
			result.fanshu = 13;
			result.fushu = 30;
			goto outputinfo;
		}
		else
			return result;
	}
	else
	{
		for (int i = 0; i < 2; i++)
			for (int j = 0; j < 4; j++)
			{
				typeinfo[i][j] = get_typeinfo(first(j), (i == 1), true);
				for (int k = 0; k < typeinfo[i][j].group; k++)
					typeinfo[i][j].groupex[k].keytype = j;
			}
		// ͳ�ƶ�������
		for (int i = 0; i < 4; i++)
			DuiZiSum += typeinfo[0][i].duizi;
		if (DuiZiSum >= 7)
		{
			result.ifhupai = true;
			QiDuiZi = true;
		}
		
		for (int k = 0; k < 4 && !normalhu; k ++)				//�н��Ƶ���һ�ֻ�ɫ
		{
			if (typeinfo[1][k].general == false) continue;
			groupsum = 0;
			for (int i = 0; i < 4 && !normalhu; i++)
				groupsum += typeinfo[(i == k)][i].group;
			if (groupsum == 4 - fulu.groupsum)		// ���������һ�ԣ����Ժ���
			{
				result.ifhupai = true;
				normalhu = true;
				FinalGeneral.type = k;
				FinalGeneral.num = typeinfo[1][k].generalex;
			}
		}
	}
	// �����Ҫ��ȡ�����Ϣ
	if (result.ifhupai && !test)
	{
		// �ж�����
		{
			int SingleTypeSum[4] = {0, 0, 0, 0};						// һ���Ƶ�����
			int shunnum = 0, kenum = 0, ankenum = 0, kannum = 0;		// ˳���̡����̡��ܵ�����
			bool QianZhang = false, BianZhang = false, PingHuZhang = false;	

			// ��ȡ����ʱ�ġ������һ�ԡ�
			shoupaiinfo hutile;
			hutile.groupsum = 0;
			for (int i = 0; i < 4 && hutile.groupsum < 4; i++)
				for (int j = 0; j < typeinfo[(i == FinalGeneral.type)][i].group; j++, hutile.groupsum++)
					hutile.allgroup[hutile.groupsum] = typeinfo[(i == FinalGeneral.type)][i].groupex[j];
			for (int i = 0; i < fulu.groupsum; i++, hutile.groupsum++)
				hutile.allgroup[hutile.groupsum] = fulu.allgroup[i];
		
			tiles FinalTile; // ����ʱ��ɡ������һ�ԡ���ȫ����
			if (normalhu)
			{
				for (int i = 0; i < hutile.groupsum; i++)
				{
					switch(hutile.allgroup[i].thistype){

					case shun:
						shunnum ++;
						SingleTypeSum[hutile.allgroup[i].keytype] += 3;
						for (int j = 0; j < 3; j++)
							FinalTile.change_tilenum(hutile.allgroup[i].keytype, hutile.allgroup[i].key + j, 1);
						break;

					case ke:
						kenum ++;
						ankenum ++;
						SingleTypeSum[hutile.allgroup[i].keytype] += 3;
						FinalTile.change_tilenum(hutile.allgroup[i].keytype, hutile.allgroup[i].key, 3);
						break;

					case kan:
						kannum ++;
						kenum ++;
						SingleTypeSum[hutile.allgroup[i].keytype] += 4;
						FinalTile.change_tilenum(hutile.allgroup[i].keytype, hutile.allgroup[i].key, 4);
						break;

					case ankan:
						kannum ++;
						ankenum ++;
						SingleTypeSum[hutile.allgroup[i].keytype] += 4;
						FinalTile.change_tilenum(hutile.allgroup[i].keytype, hutile.allgroup[i].key, 4);
						break;
					}
				}
				for (int i = 0; i < fulu.groupsum; i++)
					if (fulu.allgroup[i].thistype == ke)
						ankenum --;
				SingleTypeSum[FinalGeneral.type] += 2;
				FinalTile.change_tilenum(FinalGeneral.type, FinalGeneral.num, 2);
				
				// �ж��������Ƿ�Ϊ���š�Ƕ��
				if (lasttile.type != 3)
				{
					for (int i = 0; i < 4; i++)
					{
						groupinfo* ThisGroup = &hutile.allgroup[i];
						if (ThisGroup->thistype == shun && ThisGroup->keytype == lasttile.type)
						{
							if (ThisGroup->key == lasttile.num)
							{
								if (ThisGroup->key < 6) PingHuZhang = true;
								else BianZhang = true;
							}
							else if (ThisGroup->key + 1 == lasttile.num)
							{
								QianZhang = true;
							}
							else if (ThisGroup->key + 2 == lasttile.num)
							{
								if (ThisGroup->key > 0) PingHuZhang = true;
								else BianZhang = true;
							}
						}
					}
					if (PingHuZhang)
					{
						BianZhang = false;
						QianZhang = false;
					}
				}

				for (int i = 0; i < 4; i++)
					if (hutile.allgroup[i].thistype == ke && hutile.allgroup[i].keytype == lasttile.type && hutile.allgroup[i].key == lasttile.num && !tsumo && !QianZhang && !BianZhang && !PingHuZhang)
						ankenum --;
			}
			else if(QiDuiZi)
			{
				// ������
				int counted_duizi = 0;
				for (int type_num = 0; type_num < 37 && counted_duizi < 7; type_num ++)
				{
					if (type_num % 10 == 9) continue;
					int thisduizi = get_tilenum(type_num / 10, type_num % 10) / 2;
					if (thisduizi > 0)
					{
						FinalTile.change_tilenum(type_num / 10, type_num % 10, 2 * thisduizi);
						SingleTypeSum[type_num / 10] += 2 * thisduizi;
						counted_duizi += thisduizi;
					}
					if (counted_duizi > 7)
					{
						FinalTile.change_tilenum(type_num / 10, type_num % 10, -2);
						SingleTypeSum[type_num / 10] -= 2;
						counted_duizi --;
					}
				}
			}
			int FinalTileSum = FinalTile.get_tilesum();
			
			// �жϷ���
			if (riichi)
			{
				if (w_riichi)
					strcat_s(result.fanzhong, strlen(result.fanzhong) + 8, "˫��ֱ ");
				else
					strcat_s(result.fanzhong, strlen(result.fanzhong) + 6, "��ֱ ");
				if (ippatsu)
					strcat_s(result.fanzhong, strlen(result.fanzhong) + 6, "һ�� ");
			}
			if (chankan)
				strcat_s(result.fanzhong, strlen(result.fanzhong) + 6, "���� ");
			if (rinshan)
				strcat_s(result.fanzhong, strlen(result.fanzhong) + 10, "���Ͽ��� ");
			if (haitei)
			{
				if (tsumo)
					strcat_s(result.fanzhong, strlen(result.fanzhong) + 10, "�������� ");
				else
					strcat_s(result.fanzhong, strlen(result.fanzhong) + 10, "�ӵ����� ");
			}
			if (tenhou)
			{
				if (self_direction == match_info.this_dealer)
					strcat_s(result.fanzhong, strlen(result.fanzhong) + 6, "��� ");
				else
					strcat_s(result.fanzhong, strlen(result.fanzhong) + 6, "�غ� ");
			}
			// ���硢�Է�������
			for (int i = 0; i < 4; i++)
			{
				if (hutile.allgroup[i].keytype == 3)
				{
					if (hutile.allgroup[i].key < 4 && self_direction <= north && match_info.match_wind <= north)
					{
						if (match_info.match_wind == hutile.allgroup[i].key)
						{
							strcat_s(result.fanzhong, strlen(result.fanzhong) + 5, "����");
							strcat_s(result.fanzhong, strlen(result.fanzhong) + 3, seatname[match_info.match_wind]);
							strcat_s(result.fanzhong, strlen(result.fanzhong) + 2, " ");
						}
						if ((self_direction - match_info.this_dealer + 4) % 4 == hutile.allgroup[i].key)
						{
							strcat_s(result.fanzhong, strlen(result.fanzhong) + 5, "�Է�");
							strcat_s(result.fanzhong, strlen(result.fanzhong) + 3, seatname[(self_direction - match_info.this_dealer + 4) % 4]);
							strcat_s(result.fanzhong, strlen(result.fanzhong) + 2, " ");
						}
					}
					if (hutile.allgroup[i].key >= 4)
					{
						strcat_s(result.fanzhong, strlen(result.fanzhong) + 5, "����");
						strcat_s(result.fanzhong, strlen(result.fanzhong) + 3, zipainame_simple[hutile.allgroup[i].key]);
						strcat_s(result.fanzhong, strlen(result.fanzhong) + 2, " ");
					}
				}
			}
			// �����߶�����������
			if (QiDuiZi)
			{
				if (normalhu)	// ͬʱ������Ϊ�������ƴ�����߶�����ʵ��������
					strcat_s(result.fanzhong, strlen(result.fanzhong) + 8, "������ ");
				else
					strcat_s(result.fanzhong, strlen(result.fanzhong) + 8, "�߶��� ");
			}
			if (SingleTypeSum[3] == FinalTileSum)
				strcat_s(result.fanzhong, strlen(result.fanzhong) + 8, "��һɫ ");
			else
			{
				// ��һɫ
				if(SingleTypeSum[2] + SingleTypeSum[3] == FinalTileSum)	
				{
					int greenflag = true;
					for (int i = 0; i < 6 && greenflag; i++)
						if (alltile.get_tilenum(3, i) != 0)
							greenflag = false;
					if (greenflag && alltile.get_tilenum(2, 0) == 0 && alltile.get_tilenum(2, 4) == 0 && alltile.get_tilenum(2, 6) == 0 && alltile.get_tilenum(2, 8) == 0)
						strcat_s(result.fanzhong, strlen(result.fanzhong) + 8, "��һɫ ");
				}
				// ��һɫ���һɫ
				for (int i = 0; i < 3; i++)
				{
					if (SingleTypeSum[i] + SingleTypeSum[3] == FinalTileSum)
					{
						if (SingleTypeSum[i] == FinalTileSum)
						{
							bool nine_gates_flag = fulu.groupsum == 0 && get_tilenum(i, 0) >= 3 && get_tilenum(i, 8) >= 3;
							for (int j = 0; j < 9 && nine_gates_flag; j++)
								if (get_tilenum(i, j) == 0)
									nine_gates_flag = false;
							if (nine_gates_flag)
								strcat_s(result.fanzhong, strlen(result.fanzhong) + 10, "�������� ");
							else 
								strcat_s(result.fanzhong, strlen(result.fanzhong) + 8, "��һɫ ");
						}
						else
							strcat_s(result.fanzhong, strlen(result.fanzhong) + 8, "��һɫ ");
					}
				}
			}
			if (YaoJiuSum == FinalTileSum)
			{
				if (SingleTypeSum[3] == 0)
					strcat_s(result.fanzhong, strlen(result.fanzhong) + 8, "���۾� ");
				else 
					strcat_s(result.fanzhong, strlen(result.fanzhong) + 8, "���۾� ");
			}
			// ��ȫ�����ȫ��
			else if (FinalGeneral.type == 3 || FinalGeneral.num == 0 || FinalGeneral.num == 8)
			{
				int yaojiugroup = 0;
				for (int i = 0; i < hutile.groupsum; i++)
				{
					if (hutile.allgroup[i].thistype >= ke && (hutile.allgroup[i].keytype == 3 || hutile.allgroup[i].key == 0 || hutile.allgroup[i].key == 8))
						yaojiugroup ++;
					if (hutile.allgroup[i].thistype == shun && (hutile.allgroup[i].key == 0 || hutile.allgroup[i].key == 6))
						yaojiugroup ++;
				}
				if (yaojiugroup == 4)
				{
					if (SingleTypeSum[3] == 0)
						strcat_s(result.fanzhong, strlen(result.fanzhong) + 12, "��ȫ���۾� ");
					else
						strcat_s(result.fanzhong, strlen(result.fanzhong) + 12, "��ȫ���۾� ");
				}
			}
			if (YaoJiuSum == 0)
				strcat_s(result.fanzhong, strlen(result.fanzhong) + 8, "���۾� ");
			// �жϴ���Ԫ��С��Ԫ
			{
				int SanYuanKeSum = 0;
				for (int i = 0; i < 4; i++)
					SanYuanKeSum += (hutile.allgroup[i].keytype == 3 && hutile.allgroup[i].key >= 4);
				if (SanYuanKeSum == 2 && FinalGeneral.type == 3 && FinalGeneral.num >= 4)
					strcat_s(result.fanzhong, strlen(result.fanzhong) + 8, "С��Ԫ ");
				else if (SanYuanKeSum == 3)
					strcat_s(result.fanzhong, strlen(result.fanzhong) + 8, "����Ԫ ");
			}
			// �жϴ���ϲ��С��ϲ
			{
				int SiXiKeSum = 0;
				for (int i = 0; i < 4; i++)
					SiXiKeSum += (hutile.allgroup[i].keytype == 3 && hutile.allgroup[i].key < 4);
				if (SiXiKeSum == 3)
					strcat_s(result.fanzhong, strlen(result.fanzhong) + 8, "С��ϲ ");
				else if (SiXiKeSum == 4)
					strcat_s(result.fanzhong, strlen(result.fanzhong) + 8, "����ϲ ");
			}
			if (shunnum < 4 || (FinalGeneral.type == 3 && (FinalGeneral.num >= 4 || FinalGeneral.num == match_info.match_wind || FinalGeneral.num == (self_direction - match_info.this_dealer + 4) % 4)))
				PingHu = false;
			if (PingHu && PingHuZhang && fulu.groupsum == 0)
				strcat_s(result.fanzhong, strlen(result.fanzhong) + 6, "ƽ�� ");
			if (fulu.feimenqing == false && tsumo)
				strcat_s(result.fanzhong, strlen(result.fanzhong) + 10, "�������� ");
			if (normalhu)
			{
				// ����ϵ������
				if (!QiDuiZi)
				{
					for (int i = 0; i < hutile.groupsum - fulu.groupsum - 1; i++)
						if (hutile.allgroup[i] == hutile.allgroup[i + 1])
						{
							if (i < 2 && hutile.allgroup[i + 1] == hutile.allgroup[i + 2] && (lasttile.type != hutile.allgroup[i].keytype || lasttile.num != hutile.allgroup[i].key && lasttile.num != hutile.allgroup[i].key + 1 && lasttile.num != hutile.allgroup[i].key + 2))
								strcat_s(result.fanzhong, strlen(result.fanzhong) + 8, "������ ");
							else if (strstr(result.fanzhong, "������") == NULL && fulu.feimenqing == false)
								strcat_s(result.fanzhong, strlen(result.fanzhong) + 8, "һ���� ");
						}
					if (kenum + ankenum >= 3)
					{
						if (ankenum == 3 && strstr(result.fanzhong, "������") == NULL)
							strcat_s(result.fanzhong, strlen(result.fanzhong) + 8, "������ ");
						if (ankenum == 4)
							strcat_s(result.fanzhong, strlen(result.fanzhong) + 8, "�İ��� ");
						if (kannum == 3)
							strcat_s(result.fanzhong, strlen(result.fanzhong) + 8, "������ ");
						if (kannum == 4)
							strcat_s(result.fanzhong, strlen(result.fanzhong) + 8, "�ĸ��� ");
						if (kenum == 4)
							strcat_s(result.fanzhong, strlen(result.fanzhong) + 8, "�ԶԺ� ");
						{
							char *TongKe = (char*)malloc(sizeof(char) * 10);
							memset(TongKe, '0', 9);
							TongKe[9] = '\0';
							for (int j = 0; j < hutile.groupsum; j++)
							{
								if (hutile.allgroup[j].keytype < 3 && hutile.allgroup[j].thistype == ke && (j == 0 || hutile.allgroup[j].keytype != hutile.allgroup[j - 1].keytype || hutile.allgroup[j].key != hutile.allgroup[j - 1].key))
									TongKe[hutile.allgroup[j].key] += 1;
							}
							if (strstr(TongKe, "3") != NULL)
								strcat_s(result.fanzhong, strlen(result.fanzhong) + 10, "��ɫͬ�� ");
							free(TongKe);
						}
					}
				}
				if (shunnum >= 3)
				{
					// һ��ͨ��
					{
						bool DragonShun[3][3] = {false};
						for (int j = 0; j < hutile.groupsum; j++)
						{
							if (hutile.allgroup[j].thistype == shun && hutile.allgroup[j].key % 3 == 0)
								DragonShun[hutile.allgroup[j].keytype][hutile.allgroup[j].key / 3] = true;
						}
						for (int j = 0; j < 3; j++)
							if (DragonShun[j][0] && DragonShun[j][1] && DragonShun[j][2])
								strcat_s(result.fanzhong, strlen(result.fanzhong) + 10, "һ��ͨ�� ");
					}
					// ��ɫͨ˳
					{
						char *TongShun = new char[8];
						memset(TongShun, 0, 8);
						for (int j = 0; j < hutile.groupsum; j++)
						{
							groupinfo *ThisGroup = &hutile.allgroup[j];
							if (ThisGroup->thistype == shun)
								TongShun[ThisGroup->key] |= 1 << ThisGroup->keytype;
						}
						bool TongShunFlag = false;
						for (int j = 0; j < 7; j++)
							if (TongShun[j] == char(7))
								TongShunFlag = true;
						if (TongShunFlag)
							strcat_s(result.fanzhong, strlen(result.fanzhong) + 10, "��ɫͬ˳ ");
						delete[] TongShun;
					}
				}
			}
			result.fanshu = 0;
			// ���������жϣ����ȱ�������
			if (result.fanzhong[0] != '\0' && match_info.dora[0].num != -1)
			{
				int dorasum = 0;
				// ��ȡ��������
				for (int i = 0; i < match_info.kansum + 1; i++)
				{
					singletile truedora;
					while(i < 5 + riichi * 5)
					{
						// �ɱ���ָʾ�ƻ�ȡ��������
						truedora.type = match_info.dora[i].type;
						if (truedora.type <= 2)
							truedora.num = (match_info.dora[i].num + 1) % 9;
						else if (match_info.dora[i].num < 4)
							truedora.num = (match_info.dora[i].num + 1) % 4;
						else
							truedora.num = (match_info.dora[i].num - 4 + 1) % 3 + 4;
						dorasum += alltile.get_tilenum(truedora);
						i += 5;
					}
					i = i % 5;
				}
				if (dorasum > 0)
				{
					char tempchar[3];
					_itoa_s(dorasum, tempchar, 10);
					strcat_s(result.fanzhong, strlen(result.fanzhong) + 5, "����");
					strcat_s(result.fanzhong, strlen(result.fanzhong) + strlen(tempchar) + 1, tempchar);
					strcat_s(result.fanzhong, strlen(result.fanzhong) + 2, " ");
					result.fanshu += dorasum;
				}
			}
			// �жϷ��ֵķ���
			{
				memset(result.hutype, '\0', 30);
				char *fanzhong = NULL;
				char *fanzhong_fanshu[6] = {fanzhong_sub1[0], fanzhong_1[0], fanzhong_2[0], fanzhong_3[0], fanzhong_6[0], fanzhong_13[0]}; 
				int fanshu[6] = {-1, 1, 2, 3, 6, 13};		//���ַ��ֵķ���
				int zishusize[6] = {11, 9, 11, 11, 7, 9};		//���ַ��������Ĵ�С
				char newfanzhong[50] = {'\0'};					//�Է��ֿ�����Ҫ���£�����ֱ������ʱ����Ҫ����С�Ʒ���
				// ����������
				for (fanzhong = fanzhong_fanshu[5]; fanzhong[0] != '\0'; fanzhong += zishusize[5])
				{
					if(strstr(result.fanzhong, fanzhong) != NULL)
					{
						result.fanshu += fanshu[5];
						strcat_s(newfanzhong, strlen(newfanzhong) + strlen(fanzhong) + 1, fanzhong);
						strcat_s(newfanzhong, strlen(newfanzhong) + 2, " ");
						strcpy_s(result.hutype, 6, "���� ");
					}
				}
				// û����������
				if (newfanzhong[0] == '\0')
				{
					for (int i = 0; i < 5; i++)
						for (fanzhong = fanzhong_fanshu[i]; fanzhong[0] != '\0'; fanzhong += zishusize[i])
							if(strstr(result.fanzhong, fanzhong) != NULL && (i > 0 || (fulu.groupsum != 0 && fulu.feimenqing)))
								result.fanshu += fanshu[i];
					// ˫��ֱ����ֱ�����ظ��ж�
					if (strstr(result.fanzhong, "˫��ֱ") != NULL)
						result.fanshu -= 1;

					// ���ƿ����ж��֣����������֣���Ϊ����Ϊ����Ԫ������Ҫ�ظ��ж�
					char *temp = strstr(result.fanzhong, "����");
					if (temp != NULL)
					{
						temp += 4;
						temp = strstr(temp, "����");
						if (temp != NULL)
							result.fanshu += 1;
					}
				}
				else 
					strcpy_s(result.fanzhong, strlen(newfanzhong) + 1, newfanzhong);
			}
			// �жϷ���
			{
				result.fushu = 0;
				// �������
				if (strstr(result.fanzhong, "ƽ��") != NULL && strstr(result.fanzhong, "��������") != NULL)
					result.fushu = 20;
				if (strstr(result.fanzhong, "�߶���") != NULL)
					result.fushu = 25;
				if (PingHu && PingHuZhang && !tsumo && strstr(result.fanzhong, "ƽ��") == NULL)
					result.fushu = 30;
				// һ�����
				if (result.fushu == 0)
				{
					result.fushu = 20;
					if (BianZhang || QianZhang)
						result.fushu += 2;
					else if (!PingHuZhang && FinalGeneral.type == lasttile.type && FinalGeneral.num == lasttile.num)
						result.fushu += 2;
					if (tsumo)
						result.fushu += 2;
					else if (fulu.groupsum == 0 || fulu.feimenqing == false)
						result.fushu += 10;
					if (FinalGeneral.type == 3)
					{
						if (FinalGeneral.num >= 4)
							result.fushu += 2;
						bool SelfWindTile = (FinalGeneral.num == (self_direction - match_info.this_dealer + 4) % 4);
						bool MatchWindTile = (FinalGeneral.num == match_info.match_wind);
						if (match_info.DoubleWindTile && SelfWindTile && MatchWindTile)
							result.fushu += 4;
						else if (SelfWindTile || MatchWindTile)
							result.fushu += 2;
					}
					for (int i = 0; i < hutile.groupsum; i++)
					{
						if (hutile.allgroup[i].thistype >= ke)
						{
							bool anke = true;
							if (hutile.allgroup[i].keytype == lasttile.type && hutile.allgroup[i].key == lasttile.num && !tsumo && !QianZhang && !BianZhang && !PingHuZhang)
								anke = false;
							for (int j = 0; j < fulu.groupsum && anke; j++)
								if (fulu.allgroup[j].thistype == ke && hutile.allgroup[i] == fulu.allgroup[j])
									anke = false;
							result.fushu += pow(2.0, hutile.allgroup[i].thistype) / (anke ? 1 : 2) * (hutile.allgroup[i].keytype == 3 || hutile.allgroup[i].key == 0 || hutile.allgroup[i].key == 8 ? 2 : 1);
						}
					}
					result.fushu = (result.fushu / 10 + (result.fushu % 10 != 0)) * 10;
					if (result.fushu == 20)
						result.fushu = 30;
				}
			}
		}
		// �����Ϣ
		outputinfo:
		char get_point_info[30];
		memset(get_point_info, 0, 30);
		char tempnum[6];
		memset(tempnum, 0, 6);
		// ����Ѿ��������������ʮ���ۣ�������Ҫͳ�Ʒ����ͷ���
		if (result.hutype[0] == '\0')
		{
			_itoa_s(result.fushu, tempnum, 10);
			strcpy_s(get_point_info, strlen(tempnum) + 1, tempnum);
			strcat_s(get_point_info, strlen(get_point_info) + 3, "��");
			_itoa_s(result.fanshu, tempnum, 10);
			strcat_s(get_point_info, strlen(get_point_info) + strlen(tempnum) + 1, tempnum);
			strcat_s(get_point_info, strlen(get_point_info) + 4, "�� ");
		}
		getpointinfo get_point = result.getpoint();
		if (get_point.dealer_point == 4000)
			strcat_s(get_point_info, strlen(get_point_info) + 6, "���� ");
		else if(get_point.dealer_point == 6000)
			strcat_s(get_point_info, strlen(get_point_info) + 6, "���� ");
		else if(get_point.dealer_point == 8000)
			strcat_s(get_point_info, strlen(get_point_info) + 6, "���� ");
		else if(get_point.dealer_point == 12000)
			strcat_s(get_point_info, strlen(get_point_info) + 8, "������ ");
		else if(get_point.dealer_point == 16000 && result.hutype[0] == '\0')
			strcat_s(get_point_info, strlen(get_point_info) + 10, "�ۼ����� ");
		// �Լ���ׯ��
		if ((self_direction - match_info.this_dealer + 4) % 4 == east)
		{
			if(tsumo)
			{
				_itoa_s(get_point.dealer_point, tempnum, 10);
				strcat_s(get_point_info, strlen(get_point_info) + strlen(tempnum) + 1, tempnum);
				strcat_s(get_point_info, strlen(get_point_info) + 5, " all");
			}
			else
			{
				if (get_point.dealer_point >= 4000)
					_itoa_s(get_point.dealer_point * 3, tempnum, 10);
				else
					_itoa_s(get_point.ron_point_dealer, tempnum, 10);
				strcat_s(get_point_info, strlen(get_point_info) + strlen(tempnum) + 1, tempnum);
			}
		}
		// �Լ����м�
		else
		{
			if(tsumo)
			{
				_itoa_s(get_point.normal_point, tempnum, 10);
				strcat_s(get_point_info, strlen(get_point_info) + strlen(tempnum) + 1, tempnum);
				strcat_s(get_point_info, strlen(get_point_info) + 3, "��");
				_itoa_s(get_point.dealer_point, tempnum, 10);
				strcat_s(get_point_info, strlen(get_point_info) + strlen(tempnum) + 1, tempnum);
			}
			else
			{
				if (get_point.dealer_point >= 4000)
					_itoa_s(get_point.dealer_point * 2, tempnum, 10);
				else
					_itoa_s(get_point.ron_point_normal, tempnum, 10);
				strcat_s(get_point_info, strlen(get_point_info) + strlen(tempnum) + 1, tempnum);
			}
		}
		strcat_s(result.hutype, strlen(result.hutype) + strlen(get_point_info) + 1, get_point_info);
	}
	return result;
}

tingpaiinfo pai::get_tingpaiinfo(tiles &remaintiles, const matchinfo &match_info, direction self_direction, BYTE specialform)
{
	pai ifpai = *this;
	tileinfo paiinfo[2][4], ifpaiinfo[2][4];
	tileinfo tempinfo[2];
	int needtilenum = 0, addtile = 0, tempaddtile, addtype, tempaddtype;
	int* actualaddtile = NULL, *actualaddtype = NULL;
	bool actualaddvalid;
	char* paddtile = NULL;
	bool hu = false;
	tingpaiinfo result = {0, 0, 0.0, 0.0, false, false};
	bool shisanyaogeneral = false;
	int shisanyaoneedtilenum = 0;
	double shisanyaopossibility = 1.0;
	int qiduizi_duizi = 0,qiduizi_danzhang = 0;
	double qiduizi_possibility = 0.0, qiduizi_thispossibility = 1.0;
	int qiduizi_needtilenum = 7;
	int remaintiles_num = remaintiles.get_tilesum();
	//----------------------------��ʼ����---------------------------------
	if (fulu.groupsum == 0)
	{
		//----------------------------�ȼ���ʮ���۵�������-----------------------------
		tiles shisanyaotempremain = remaintiles;
		actualaddtile = (int *)malloc(sizeof(int) * 14);
		actualaddtype = (int *)malloc(sizeof(int) * 14);
		for (int i = 0; i < 3; i++)
		{
			if (get_tilenum(i, 0) == 0)
			{
				actualaddtile[shisanyaoneedtilenum] = 0;
				actualaddtype[shisanyaoneedtilenum] = i;
				shisanyaoneedtilenum ++;
			}
			if (get_tilenum(i, 8) == 0)
			{
				actualaddtile[shisanyaoneedtilenum] = 8;
				actualaddtype[shisanyaoneedtilenum] = i;
				shisanyaoneedtilenum ++;
			}
			if (get_tilenum(i, 0) >= 2 || get_tilenum(i, 8) >= 2)
				shisanyaogeneral = true;
		}
		for (int i = 0; i < 7; i++)
		{
			if (get_tilenum(3, i) == 0)
			{
				actualaddtile[shisanyaoneedtilenum] = i;
				actualaddtype[shisanyaoneedtilenum] = 3;
				shisanyaoneedtilenum ++;
			}
			if (get_tilenum(3, i) >= 2)
				shisanyaogeneral = true;
		}
		for (int k = 0; k < shisanyaoneedtilenum; k++)
		{
			shisanyaopossibility *= shisanyaotempremain.get_tilenum(actualaddtype[k], actualaddtile[k]) / double(remaintiles_num);
			if (shisanyaotempremain.get_tilenum(actualaddtype[k], actualaddtile[k]) > 0)
				shisanyaotempremain.change_tilenum(actualaddtype[k], actualaddtile[k], -1);
		}
		if (!shisanyaogeneral)
		{
			int yaojiuremain = 0;
			for (int i = 0; i < 3; i++)
				yaojiuremain += shisanyaotempremain.get_tilenum(i, 0) + shisanyaotempremain.get_tilenum(i, 8);
			shisanyaopossibility *= yaojiuremain / double(remaintiles_num);
			shisanyaoneedtilenum ++;
		}
		free(actualaddtile);
		free(actualaddtype);
		if (specialform & TINGPAI_SHIYANYAO)
		{
			result.hupossibility = shisanyaopossibility;
			result.shisanyao = true;
			result.qiduizi = false;
			result.xiangting = shisanyaoneedtilenum - 1;
			return result;
		}
		//------------------------------��Ҫ�����߶��ӵļ��㷨----------------------------
		
		tiles qiduizi_tempremain = remaintiles;
		int* qiduizi_danzhangtile = NULL, *qiduizi_danzhangtype = NULL;
		qiduizi_danzhangtile = (int *)malloc(sizeof(int) * 14);
		qiduizi_danzhangtype = (int *)malloc(sizeof(int) * 14);
		for(int i = 0; i < 37; i++)
		{
			if (i % 10 == 9) continue;
			if(get_tilenum(i / 10, i % 10) >= 2)
				qiduizi_duizi ++;
			if (get_tilenum(i / 10, i % 10) == 1 || get_tilenum(i / 10, i % 10) == 3)
			{
				qiduizi_danzhangtile[qiduizi_danzhang] = i % 10;
				qiduizi_danzhangtype[qiduizi_danzhang] = i / 10;
				qiduizi_danzhang ++;
			}
		}
		qiduizi_needtilenum -= qiduizi_duizi;
		if (qiduizi_needtilenum > 0)
		{
			if (qiduizi_danzhang < 10)
			{
				int maxzuhenum = 0;
				for (int i = 0; i < qiduizi_needtilenum; i++)
					maxzuhenum += (qiduizi_danzhang - i) * pow(10.0, i);
				for (int i = (pow(double(qiduizi_danzhang + 1), qiduizi_needtilenum) - 1) / qiduizi_danzhang, zuhenum = jingzhi(i, qiduizi_danzhang + 1); zuhenum <= maxzuhenum; i++, zuhenum = jingzhi(i, qiduizi_danzhang + 1))
				{
					int lastnum = zuhenum, nextnum = zuhenum;
					bool flag = false;
					for (int j = 0; j < qiduizi_needtilenum - 1; j++)
					{
						lastnum = zuhenum, nextnum = zuhenum;
						for (int k = 0; k < qiduizi_needtilenum - j - 2; k++)
						{
							lastnum /= 10;
							nextnum /= 10;
						}
						lastnum /= 10;
						nextnum -= lastnum * 10;
						lastnum = lastnum % 10;
						if (lastnum >= nextnum)
						{
							i += (lastnum - nextnum + 1) * pow((double)(qiduizi_danzhang + 1), qiduizi_needtilenum - j - 2) - 1;
							flag = true;
						}
					}
					if (flag)
						continue;
					qiduizi_thispossibility = 1.0;
					for (int j = 0, thistilenum = zuhenum; j < qiduizi_needtilenum; j++)
					{
						qiduizi_thispossibility *= qiduizi_tempremain.get_tilenum(qiduizi_danzhangtype[thistilenum % 10 - 1], qiduizi_danzhangtile[thistilenum % 10 - 1]) / double(remaintiles_num);
						thistilenum /= 10;
					}
					qiduizi_possibility += qiduizi_thispossibility;
				}
			}
		}
		free(qiduizi_danzhangtile);
		free(qiduizi_danzhangtype);
		if (specialform & TINGPAI_QIDUIZI)
		{
			result.hupossibility = qiduizi_possibility;
			result.shisanyao = false;
			result.qiduizi = true;
			result.xiangting = qiduizi_needtilenum - 1;
			return result;
		}
	}
	else
	{
		qiduizi_possibility = 0.0;
		qiduizi_needtilenum = 8;
		shisanyaopossibility = 0.0;
		shisanyaoneedtilenum = 15;
	}
	//------------------------------��ʼ����һ����Ʒ�ʽ--------------------------------
	for (int i = 0; i < 2; i++)
		for (int j = 0; j < 4; j++)
			paiinfo[i][j] = get_typeinfo(first(j), (i == 1));
	while (((!hu && needtilenum < nowminxiangting + 2) || (result.hupossibility <= pow(10.0, -12) && needtilenum < nowminxiangting + 3)) 
		&& (needtilenum < shisanyaoneedtilenum || shisanyaopossibility <= pow(10.0, -12)) 
		&& (needtilenum <= qiduizi_needtilenum || qiduizi_possibility <= pow(10.0, -12)) && needtilenum < 7)
	{
		actualaddtile = (int*)malloc(sizeof(int) * needtilenum);
		actualaddtype = (int*)malloc(sizeof(int) * needtilenum);
		for (int i = 0; needtilenum > 0 && i < pow(9.0, needtilenum); i++)
		{
			//-----------------------��ȷ�������Ƶ�����-----------------------------
			addtile = jingzhi(i);			
			if (!ifstob(addtile)) continue; 
			tempaddtile = addtile;
			
			for (int j = 0; j < needtilenum; j++)
			{
				actualaddtile[j] = tempaddtile % 10;
				tempaddtile /= 10;
			}
			//-----------------------�پ��嵽�����Ƶ�����---------------------------
			{
				int addtypenum[4] = {0};
				for (int j = 0; j < pow(4.0, needtilenum); j++, ifpai = *this)
				{
					for (int k = 0; k < 4; k++)
						addtypenum[k] = 0;
					addtype = jingzhi(j, 4);
					tempaddtype = addtype;
					for (int k = 0; k < needtilenum; k++)
					{
						actualaddtype[k] = tempaddtype % 10;
						addtypenum[actualaddtype[k]] ++;
						tempaddtype /= 10;
					}
					actualaddvalid = true;
					//------------------------�ϸ����ƣ�ͬһ�ִ���ֻ�ܳ���һ��---------------------------
					for (int k = 0, cmpchar = 0, charnum = 1; k < needtilenum; cmpchar = 0, charnum = 1)
					{
						while (actualaddtile[k + charnum - 1] == actualaddtile[k + charnum])
							charnum ++;
						for (int l = 0; l < charnum; l++)
							cmpchar = actualaddtype[k + charnum - l - 1] + cmpchar * 10;
						if (!ifstob(cmpchar))
						{
							actualaddvalid = false;
							break;
						}
						k += charnum;
					}
					//---------------------�������ֲ��ܳ���6�����������Ч-------------------------------
					for (int k = 0; k < needtilenum; k++)				
						if (actualaddtype[k] == 3 && actualaddtile[k] > 6)
							actualaddvalid = false;
					if (!actualaddvalid) continue;
					//-----------------------��ʼ����--------------------------------
					{
						int haveaddtilenum[4] = {0};
						for (int k = 0; k < needtilenum; k++)
						{
							haveaddtilenum[actualaddtype[k]] ++;
							ifpai.change_tilenum(actualaddtype[k], actualaddtile[k], 1);
							if (ifpai.get_tilenum(actualaddtype[k], actualaddtile[k]) > 4)
							{
								actualaddvalid = false;
								break;
							}
							//------------------------------------����һ�ֻ�ɫ������ʹ���ֻ�ɫ������һ������----------------------------------
							
							if (haveaddtilenum[actualaddtype[k]] == addtypenum[actualaddtype[k]])
							{
								tempinfo[0] = get_typeinfo(ifpai.first(actualaddtype[k]), false);
								tempinfo[1] = get_typeinfo(ifpai.first(actualaddtype[k]), true);
								if (tempinfo[0] == paiinfo[0][actualaddtype[k]] && tempinfo[1] == paiinfo[1][actualaddtype[k]])
								{
									actualaddvalid = false;
									break;
								}
							}
						}
					}
					if (!actualaddvalid) continue;
					if (ifpai.ifhu().ifhupai)
					{
						hu = true;
						result.mianting++;
						double thispossibility = 1.0;
						tiles tempremain = remaintiles;
						int thisfan = 0; 
						double thisefan = 0.0;
						int actualaddtile_remainsum = 0;
						for (int k = 0; k < needtilenum; k++)
							actualaddtile_remainsum += tempremain.get_tilenum(actualaddtype[k], actualaddtile[k]);
						if (actualaddtile_remainsum == 0)
							thispossibility = 0;
						else
							for (int k = 0; k < needtilenum; k++)
							{
								if (needtilenum < 3)
								{
									singletile lasttile(actualaddtype[k], actualaddtile[k]);
									thisfan = ifpai.ifhu(lasttile, false, HU_TSUMO | (HU_RIICHI * (fulu.groupsum == 0 || fulu.feimenqing == false)), match_info, self_direction).fanshu;
									thisefan += thisfan * (tempremain.get_tilenum(actualaddtype[k], actualaddtile[k])) / double(actualaddtile_remainsum);
								}
								thispossibility *= (tempremain.get_tilenum(actualaddtype[k], actualaddtile[k])) / double(remaintiles_num);
								if (tempremain.get_tilenum(actualaddtype[k], actualaddtile[k]) > 0)
									tempremain.change_tilenum(actualaddtype[k], actualaddtile[k], -1);
							}
						result.hupossibility += thispossibility;
						result.efan += thispossibility * thisefan;
					}
				}
			}
		}
		if (needtilenum == 0)
		{
			if(ifpai.ifhu().ifhupai)
			{
				hu = true;
				result.xiangting = needtilenum - 1;
				nowminxiangting = needtilenum - 1;
				result.hupossibility = 1.0;
			}
			else
				needtilenum ++;
		}
		else if (hu && result.hupossibility > pow(10.0, -12))
		{
			result.xiangting = needtilenum - 1;
			nowminxiangting = needtilenum - 1;
			if (needtilenum == qiduizi_needtilenum && abs(qiduizi_possibility - result.hupossibility) < pow(10.0, -12))
			{
				hu = false;
				break;
			}
		}
		else 
		{
			hu = false;
			result.hupossibility = 0;
			result.mianting = 0;
			needtilenum ++;
		}
		free(actualaddtile);
		free(actualaddtype);
	}
	if (!hu)
	{
		if (shisanyaoneedtilenum == needtilenum && shisanyaopossibility > pow(10.0, -12))
		{
			if (needtilenum < 3) 
				result.efan = 13 * shisanyaopossibility;
			result.mianting = 13 - 12 * shisanyaogeneral;
			result.shisanyao = true;
			result.xiangting = shisanyaoneedtilenum - 1;
			nowminxiangting = shisanyaoneedtilenum - 1;
			result.hupossibility = shisanyaopossibility;
		}
		else
		{
			if (qiduizi_needtilenum == needtilenum && qiduizi_possibility > pow(10.0, -12))
			{
				result.mianting = 1;
				result.qiduizi = true;
				result.xiangting = qiduizi_needtilenum - 1;
				nowminxiangting = qiduizi_needtilenum - 1;
				result.hupossibility = qiduizi_possibility;
			}
			else
			{
				result.mianting = 1716;
				result.hupossibility = 0;
				result.xiangting = 6;
				nowminxiangting = 6;
			}
		}
	}
	return result;
}

singletile pai::analysis_should(tiles &remaintiles, const matchinfo &match_info, direction self_direction)
{
	//char* ppai = wan, *premain = remainpai.wan;
	if (ifhu().ifhupai)
		return defaulttile;
	bool findshouldtile;
	bool qiduizi = false, shisanyao = false;
	bool have_getinfo = false;
	int minxiangting = 0;
	double maxpossibility = 0.0;
	double maxefan = 0.0;
	int shouldtile, shouldtype;
	int remaintiles_num = remaintiles.get_tilesum();
	tingpaiinfo info, ifinfo;
	tingpaiinfo shisanyao_info, qiduizi_info;
	if (iffeimenqing())
	{
		shisanyao = false;
		shisanyao_info.xiangting = 10;
		qiduizi = false;
		qiduizi_info.xiangting = 10;
	}
	else
	{
		shisanyao_info = get_tingpaiinfo(remaintiles, match_info, self_direction, TINGPAI_SHIYANYAO);
		qiduizi_info = get_tingpaiinfo(remaintiles, match_info, self_direction, TINGPAI_QIDUIZI);
		if (shisanyao_info.xiangting > qiduizi_info.xiangting)
		{
			shisanyao = false;
		}
		else
		{
			info = get_tingpaiinfo(remaintiles, match_info, self_direction);
			shisanyao = info.shisanyao;											
			qiduizi = info.qiduizi;
			have_getinfo = true;
		}
	}
	findshouldtile = false;
	//���ȴ�������
	if (qiduizi_info.xiangting >= 1 && !shisanyao)
	{
		int min_honor_mark = 20;
		for (int i = 0; i < 7; i++)
		{
			if (get_tilenum(3, i) == 1)			//���ȴ������������
			{
				int this_honor_mark = 0;
				if (i == (self_direction + 4 - match_info.this_dealer) % 4)
					this_honor_mark += 3;
				if (i >= 4)
					this_honor_mark += 2;
				if (i == match_info.match_wind)
					this_honor_mark += 1;
				this_honor_mark *= remaintiles.get_tilenum(3, i);
				if (this_honor_mark < min_honor_mark)
				{
					min_honor_mark = this_honor_mark;
					shouldtile = i;
					shouldtype = 3;
					findshouldtile = true;
				}
			}
		}
		if (!findshouldtile)					//���ƴ�����ϣ���δ����������
		{
			int tilenum = 0;
			bool ifisolate = false;
			for (int type_tile = 0; type_tile < 30; type_tile++)
			{
				if (type_tile % 10 == 9 || get_tilenum(type_tile / 10, type_tile % 10) != 1) continue;
				tilenum = type_tile % 10;		//ֻ��һ�ŵ��Ƶĵ���
				ifisolate = false;
				switch(tilenum)					//��ͬ�����ġ�������������ͬ
				{
					case 0:
						if (get_tilenum(type_tile / 10, type_tile % 10 + 1) == 0 && get_tilenum(type_tile / 10, type_tile % 10 + 2) == 0)
							ifisolate = true;
						break;
					case 1:
						if (get_tilenum(type_tile / 10, type_tile % 10 - 1) == 0 && get_tilenum(type_tile / 10, type_tile % 10 + 1) == 0 && get_tilenum(type_tile / 10, type_tile % 10 + 2) == 0)
							ifisolate = true;
						break;
					case 7:
						if (get_tilenum(type_tile / 10, type_tile % 10 + 1) == 0 && get_tilenum(type_tile / 10, type_tile % 10 - 1) == 0 && get_tilenum(type_tile / 10, type_tile % 10 - 2) == 0)
							ifisolate = true;
						break;
					case 8:
						if (get_tilenum(type_tile / 10, type_tile % 10 - 1) == 0 && get_tilenum(type_tile / 10, type_tile % 10 - 2) == 0)
							ifisolate = true;
						break;
					default:
						if (get_tilenum(type_tile / 10, type_tile % 10 - 1) == 0 && get_tilenum(type_tile / 10, type_tile % 10 - 2) == 0 && get_tilenum(type_tile / 10, type_tile % 10 + 1) == 0 && get_tilenum(type_tile / 10, type_tile % 10 + 2) == 0)
							ifisolate = true;
						break;
				}
				if (ifisolate && (!findshouldtile || abs(tilenum - 4) > abs(shouldtile - 4)))		//��������������ԽԶԽ���ȴ���
				{
					shouldtile = tilenum;
					shouldtype = type_tile / 10;
					findshouldtile = true;
				}
			}
		}
	}
	if (shisanyao)
	{
		int zhongzhangmarks, minmarks = 20;
		for (int i = 0; i < 3; i++)
			for (int j = 1; j < 8; j++)
				if (get_tilenum(i, j) > 0)
				{
					zhongzhangmarks = (j != 1) * (get_tilenum(i, j - 2))
									+ 2 * (get_tilenum(i, j - 1))
									+ 3 * (get_tilenum(i, j))
									+ 2 * (get_tilenum(i, j + 1))
									+ (j != 7) * (get_tilenum(i, j + 2));
					if (zhongzhangmarks < minmarks)
					{
						minmarks = zhongzhangmarks;
						shouldtile = j;
						shouldtype = i;
						findshouldtile = true;
					}
				}
	}
	if (!findshouldtile && !have_getinfo)
	{
		info = get_tingpaiinfo(remaintiles, match_info, self_direction);
		shisanyao = info.shisanyao;											
		qiduizi = info.qiduizi;
		have_getinfo = true;
	}
	if (qiduizi)
	{
		int qiduizi_danzhang = 0;
		int zhongzhangmarks, minmarks = 20;
		int minremainnum = 4;
		tiles qiduizi_tempremain = remaintiles;
		//char* tempp = qiduizi_tempremain.wan;
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 9 - (i == 3) * 2; j++)
				if (get_tilenum(i, j) == 1 || get_tilenum(i, j) == 3)
				{
					if (qiduizi_tempremain.get_tilenum(i, j) < minremainnum)
					{
						minremainnum = qiduizi_tempremain.get_tilenum(i, j);
						shouldtile = j;
						shouldtype = i;
						if (i != 3)
							minmarks = (j > 1) * (get_tilenum(i, j - 2))
									+ (j > 0) * 2 * (get_tilenum(i, j - 1))
									+ 3 * (get_tilenum(i, j))
									+ (j < 8) * 2 * (get_tilenum(i, j + 1))
									+ (j < 7) * (get_tilenum(i, j + 2));
						else 
							minmarks = 3 * (get_tilenum(i, j));
					}
					else if (get_tilenum(i, j) == minremainnum)
					{
						if (i != 3)
							zhongzhangmarks = (j > 1) * (get_tilenum(i, j - 2))
											+ (j > 0) * 2 * (get_tilenum(i, j - 1))
											+ 3 * (get_tilenum(i, j))
											+ (j < 8) * 2 * (get_tilenum(i, j + 1))
											+ (j < 7) * (get_tilenum(i, j + 2));
						else
							zhongzhangmarks = 3 * (get_tilenum(i, j));
						if (zhongzhangmarks < minmarks)
						{
							minmarks = zhongzhangmarks;
							shouldtile = j;
							shouldtype = i;
						}
					}
				}
		findshouldtile = true;
	}
	if (!findshouldtile)			//����������������������Ȼ����ȷ�������ţ������ͳ�Ƽ���
	{
		maxpossibility = 0.0;
		maxefan = 0.0;
		minxiangting = 8;
		pai ifpai = *this;
		bool firstway = (info.xiangting > 1), secondway = !firstway;
		for (int type_tile = 0; type_tile < 37; type_tile++)
		{
			if (stop_analysis) 
			{
				stop_analysis = false;
				return defaulttile;
			}
			if (type_tile % 10 == 9 || get_tilenum(type_tile / 10, type_tile % 10) == 0) continue;
			ifpai.change_tilenum(type_tile / 10, type_tile % 10, -1);
			ifinfo = ifpai.get_tingpaiinfo(remaintiles, match_info, self_direction);
			ifpai.change_tilenum(type_tile / 10, type_tile % 10, 1);
			if (ifinfo.hupossibility < pow(10.0, -12)) continue;
			if (firstway)
			{
				if(ifinfo.hupossibility > maxpossibility + pow(10.0, -12))
				{
					//maxmianting = ifinfo.mianting;
					maxpossibility = ifinfo.hupossibility;
					shouldtile = type_tile % 10;
					shouldtype = type_tile / 10;
					findshouldtile = true;
				}
				else if(abs(ifinfo.hupossibility - maxpossibility) < pow(10.0, -12) && (!findshouldtile || abs(type_tile % 10 - 4) > abs(shouldtile - 4))) 
				{
					shouldtile = type_tile % 10;
					shouldtype = type_tile / 10;
					findshouldtile = true;
				}
			}
			else
			{
				if(ifinfo.efan > maxefan + pow(10.0, -12))
				{
					maxefan = ifinfo.efan;
					shouldtile = type_tile % 10;
					shouldtype = type_tile / 10;
					findshouldtile = true;
				}
				else if(abs(ifinfo.efan - maxefan) < pow(10.0, -12) && (!findshouldtile || abs(type_tile % 10 - 4) > abs(shouldtile - 4))) 
				{
					shouldtile = type_tile % 10;
					shouldtype = type_tile / 10;
					findshouldtile = true;
				}
			}
		}
	}
	singletile shouldsingle;
	shouldsingle.num = shouldtile;
	shouldsingle.type = shouldtype;
	return shouldsingle;
}

void pai::RefreshISeeRemainTiles(tiles &ISeeRemainTiles, matchinfo match_info, const fuluinfo (&all_fulu)[4], const paihe (&all_paihe)[4])
{
	for (int type_num = 0; type_num < 37; type_num ++)
		if (type_num % 10 != 9)
			ISeeRemainTiles.set_tilenum(type_num / 10, type_num % 10, 4);
	for (int i = 0; i < 4; i++)
	{
		for (int FuluNum = 0; FuluNum < all_fulu[i].groupsum; FuluNum++)
		{
			fulugroup ThisFuluGroup = all_fulu[i].allgroup[FuluNum];
			switch (ThisFuluGroup.thistype)
			{
			case shun:
				for (int j = 0; j < 3; j++)
					ISeeRemainTiles.change_tilenum(ThisFuluGroup.keytype, ThisFuluGroup.key + j, -1);
				break;
			case ke:
				ISeeRemainTiles.change_tilenum(ThisFuluGroup.keytype, ThisFuluGroup.key, -3);
				break;
			case kan:
				ISeeRemainTiles.change_tilenum(ThisFuluGroup.keytype, ThisFuluGroup.key, -4);
				break;
			default:
				break;
			}
		}
		for (int PaiheNum = 0; PaiheNum < all_paihe[i].paihesum; PaiheNum++)
		{
			paihetile ThisPaiheTile = all_paihe[i].paihe_tile[PaiheNum];
			if (ThisPaiheTile.visible)
				ISeeRemainTiles.change_tilenum(ThisPaiheTile, -1);
		}
	}
	for (int KanNum = 0; KanNum < match_info.kansum + 1; KanNum++)
		ISeeRemainTiles.change_tilenum(match_info.dora[KanNum], -1);
	int TileSum = this->get_tilesum();
	for (int TileNum = 0; TileNum < TileSum; TileNum++)
		ISeeRemainTiles.change_tilenum(this->get_ordertile(TileNum), -1);
	
	bool flag = true;
	return;
}