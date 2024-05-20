#include "stdafx.h"
#include "cmath"
#include "MahjongRobot.h"
#include "structs.h"
#include "functions.h"

MahjongRobot::MahjongRobot(): RobotName(_T("电脑")), MyPai(), MySeat(east), MyPoints(25000), PaiStatus(0x0), FuluLock(true), LastDecision(), LastDecisionValid(false), ISeeRemainTiles(){}

MahjongRobot::MahjongRobot(const MahjongRobot &other): RobotName(other.RobotName), MyPai(other.MyPai), MySeat(other.MySeat), MyPoints(other.MyPoints), PaiStatus(other.PaiStatus), FuluLock(other.FuluLock), LastDecision(other.LastDecision), LastDecisionValid(other.LastDecisionValid), ISeeRemainTiles(){}

void MahjongRobot::set_paihetile_invisible()
{
	MyPai.set_paihetile_invisible();
}

void MahjongRobot::add_paistatus(BYTE PaiStatus)
{
	this->PaiStatus |= PaiStatus;
}

void MahjongRobot::remove_paistatus(BYTE PaiStatus)
{
	this->PaiStatus &= ~PaiStatus;
}

void MahjongRobot::reset_pai()
{
	MyPai.reset_all();
	MyPai.set_fulu(menqianqing);
	MyPai.reset_paihe();
	MyPai.remove_tile_status();
	FuluLock = true;
	PaiStatus = 0x0;
	LastDecisionValid = false;
}

robotresponse MahjongRobot::get_response(matchinfo match_info, const fuluinfo (&all_fulu)[4], const paihe (&all_paihe)[4])
{
	if (MyPai.get_tilesum() <= 0) return robotresponse();
	if (LastDecisionValid && LastDecision.response != RESPONSE_PASS) return LastDecision;
	direction active_direction = match_info.active_direction;
	singletile active_tile = match_info.active_tile;
	direction FrameMySeat = direction((MySeat - match_info.this_dealer + 4) % 4);
	LastDecisionValid = true;
	//获取当前视角所见的剩余牌
	MyPai.RefreshISeeRemainTiles(ISeeRemainTiles, match_info, all_fulu, all_paihe);
	int remaintiles_toget = ISeeRemainTiles.get_tilesum();
	remaintiles_toget -= 14 - (match_info.kansum + 1);
	for (int SeatNum = 0; SeatNum < 4; SeatNum ++)
		remaintiles_toget -= 13 - all_fulu[SeatNum].groupsum * 3;
	remaintiles_toget += MyPai.get_tilesum();

	if (active_direction == noneed || active_direction == MySeat)
	{
		if (!(active_tile == defaulttile))
			MyPai.change_tilenum(active_tile, 1);
		if (MyPai.ifhu().ifhupai)
		{
			UINT HuFlags = 0;
			HuFlags |= HU_TSUMO;
			HuFlags |= HU_RIICHI * (PaiStatus & PAI_STATUS_RIICHI);
			HuFlags |= HU_IPPATSU * (PaiStatus & PAI_STATUS_IPPATSU);
			HuFlags |= HU_TENHOU * match_info.tenhou_possible;
			HuFlags |= HU_RINSHAN * (PaiStatus & PAI_STATUS_RINSHAN);
			hupaiinfo TempHuInfo = MyPai.ifhu(active_tile, false, HuFlags, match_info, MySeat);
			if (strcmp(TempHuInfo.fanzhong, "") != 0)
			{
				LastDecision = robotresponse(RESPONSE_WIN, active_tile);
				return LastDecision;
			}
		}
		PaiStatus &= ~PAI_STATUS_IPPATSU;
		singletile shouldtile;
		if (PaiStatus & PAI_STATUS_RIICHI)
		{
			shouldtile = active_tile;
		}
		else
		{
			PaiStatus &= ~PAI_STATUS_FURITEN;
			if (FuluLock)
			{
				for (int j = 0; j < MyPai.get_fulusum(); j++)
				{
					groupinfo tempgroup = MyPai.get_fulu(j);
					if (tempgroup.keytype == 3 && (tempgroup.key >= 4 || tempgroup.key == FrameMySeat || tempgroup.key == match_info.match_wind))
						FuluLock = false;
				}
				if (MyPai.get_tilenum(3, FrameMySeat) >= 3 || MyPai.get_tilenum(3, match_info.match_wind) >= 3 || MyPai.get_tilenum(3, 4) >= 3 || MyPai.get_tilenum(3, 5) >= 3 || MyPai.get_tilenum(3, 6) >= 3 )
					FuluLock = false;
			}
			shouldtile = MyPai.analysis_should(ISeeRemainTiles, match_info, MySeat);
		}
		if (match_info.kansum < 4)
		{
			bool kanflag = false;
			//int pluskannum = -1;
			for (int j = 0; j < MyPai.get_fulusum(); j++)
			{
				groupinfo tempgroup = MyPai.get_fulu(j);
				if (tempgroup.thistype == ke && tempgroup.key == shouldtile.num && tempgroup.keytype == shouldtile.type)
				{
					kanflag = true;
					//pluskannum = j;
				}
			}
			if (MyPai.get_tilenum(shouldtile) == 4)
			{
				pai afterkan_pai = MyPai;
				pai beforekan_pai = MyPai;
				afterkan_pai.change_tilenum(shouldtile, -4);
				groupinfo thisgroup(ankan, shouldtile.type, shouldtile.num);
				afterkan_pai.add_fulu(thisgroup);
				if (PaiStatus & PAI_STATUS_RIICHI)			//如果已经立直，则暗杠需要检验是否改变听牌
				{
					bool tingpai_nochange = true;
					beforekan_pai.change_tilenum(shouldtile, -1);
					if (beforekan_pai.iftingpai() && afterkan_pai.iftingpai())
					{
						BYTE* tingpai_tiles_byte[2] = {beforekan_pai.get_tingpaitiles_byte(), afterkan_pai.get_tingpaitiles_byte()};
						for (int i = 0; tingpai_nochange && i < 37; i++)
							if (tingpai_tiles_byte[0][i] != tingpai_tiles_byte[1][i])
								tingpai_nochange = false;
					}
					if (tingpai_nochange)
						kanflag = true;
				}
				else			//如果没有立直，则暗杠需要检验是否对和牌率有提高
				{
					tingpaiinfo tempinfo = beforekan_pai.get_tingpaiinfo(ISeeRemainTiles, match_info, MySeat);
					tingpaiinfo iftempinfo = afterkan_pai.get_tingpaiinfo(ISeeRemainTiles, match_info, MySeat);
					if (iftempinfo.hupossibility - tempinfo.hupossibility > -pow(10.0, -12))
						kanflag = true;
				}
			}
			if (kanflag)
			{
				LastDecision = robotresponse(RESPONSE_KAN, shouldtile);
				return LastDecision;
			}
		}
		LastDecision.keytile = shouldtile;
		pai TempPai = MyPai;
		TempPai.change_tilenum(shouldtile, -1);
		if (!(PaiStatus & PAI_STATUS_RIICHI) && TempPai.iftingpai() && remaintiles_toget > 4 && !MyPai.iffeimenqing() && MyPoints >= 1000)
		{
			bool w_riichiflag = true;
			w_riichiflag = false;
			bool riichiflag = false;
			BYTE* tingpai_tiles_byte = TempPai.get_tingpaitiles_byte();
			bool furi_ten = false;
			int tingpai_tile_remainsum = 0;
			for (int type_num = 0; type_num < 37; type_num++)
			{
				if (type_num % 10 == 9) continue;
				if (tingpai_tiles_byte[type_num] == 0) continue;
				for (int PaiheNum = 0; PaiheNum < MyPai.get_paihesum(); PaiheNum++)
				{
					singletile temptile = MyPai.get_paihetile(PaiheNum);
					if (temptile.num == type_num % 10 && temptile.type == type_num / 10)
						furi_ten = true;
				}
				tingpai_tile_remainsum += ISeeRemainTiles.get_tilenum(type_num / 10, type_num % 10);
				pai TempMypai = TempPai;
				TempMypai.change_tilenum(type_num / 10, type_num % 10, 1);
				hupaiinfo temp = TempMypai.ifhu(singletile(type_num / 10, type_num % 10), false, 0, match_info, MySeat);
				if(((strcmp(temp.fanzhong, "") == 0) || w_riichiflag))
					riichiflag = true;
			}
			if (furi_ten) riichiflag = false;
			if (riichiflag)
			{
				LastDecision.response = RESPONSE_RIICHI;
				return LastDecision;
			}
		}
		LastDecision.response = RESPONSE_OUT;
	}
	else
	{
		if (MyPai.iftingpai() && !(PaiStatus & PAI_STATUS_FURITEN))
		{
			if (MyPai.get_tingpaitiles_byte()[active_tile.type * 10 + active_tile.num] != 0)
			{
				pai TempHuPai = MyPai;
				TempHuPai.change_tilenum(active_tile, 1);
				UINT HuFlags = 0;
				HuFlags |= HU_RIICHI * ((PaiStatus & PAI_STATUS_RIICHI) != 0);
				HuFlags |= HU_IPPATSU * ((PaiStatus & PAI_STATUS_IPPATSU) != 0);
				HuFlags |= HU_CHANKAN * (match_info.chankan_possible);
				HuFlags |= HU_RINSHAN * ((PaiStatus & PAI_STATUS_RINSHAN) != 0);
				hupaiinfo TempHuInfo = TempHuPai.ifhu(active_tile, false, HuFlags, match_info, MySeat);
				if (strcmp(TempHuInfo.fanzhong, "") != 0)
				{
					LastDecision = robotresponse(RESPONSE_WIN, active_tile);
					return LastDecision;
				}
				else
				{
					PaiStatus |= PAI_STATUS_FURITEN;
				}
			}
		}
		if (match_info.chankan_possible)
		{
			LastDecision.response = RESPONSE_PASS;
			return LastDecision;
		}
		
		for (int tile = 0; tile < 7; tile ++)
			if ((tile == FrameMySeat || tile == match_info.match_wind || tile > 4) && MyPai.get_tilenum(3, tile) >= 3)
				FuluLock = true;
		if (!(PaiStatus & PAI_STATUS_RIICHI) && remaintiles_toget > 0)
		{
			bool ponflag = false;
			bool final_chiflag = false;
			int final_chiway = 0;
			tingpaiinfo mypaiinfo;
			bool get_mypaiinfo = false;
			double afterpon_hupossibility = 0.0;
			singletile afterpon_shouldtile;
			singletile afterchi_shouldtile[3];
			if (MyPai.get_tilenum(active_tile) >= 2)
			{
				if (!FuluLock || (active_tile.type == 3 && (active_tile.num >= 4 || active_tile.num == FrameMySeat || active_tile.num == match_info.match_wind))) 
				{
					ponflag = true;
				}
				if (!ponflag)
				{
					pai AfterPonPai = MyPai;
					AfterPonPai.change_tilenum(active_tile, -2);
					AfterPonPai.add_fulu(fulugroup(ke, active_tile.type, active_tile.num, active_tile.num, (active_direction - MySeat + 4) % 4));
					if (AfterPonPai.iftingpai())
					{
						pai AfterPonHuPai = AfterPonPai;
						BYTE *AfterPon_TingPaiTiles = AfterPonPai.get_tingpaitiles_byte();
						for (int type_num = 0; type_num < 37; type_num ++)
						{
							if (type_num % 10 == 9) continue;
							if (AfterPon_TingPaiTiles[type_num] == 0) continue;
							AfterPonHuPai.change_tilenum(type_num / 10, type_num % 10, 1);
							hupaiinfo temp = AfterPonHuPai.ifhu(singletile(type_num / 10, type_num % 10), false, 0, match_info, MySeat);
							AfterPonHuPai.change_tilenum(type_num / 10, type_num % 10, -1);
							if(strcmp(temp.fanzhong, "") != 0)
							{
								ponflag = true;
								break;
							}
						}
					}
				}
				if (ponflag)
				{
					ponflag = false;
					mypaiinfo = MyPai.get_tingpaiinfo(ISeeRemainTiles, match_info, MySeat);
					get_mypaiinfo = true;
					if (!mypaiinfo.shisanyao && !mypaiinfo.qiduizi)
					{
						if (FuluLock && active_tile.type == 3 && (active_tile.num >= 4 || active_tile.num == FrameMySeat || active_tile.num == match_info.match_wind) && MyPai.get_tilenum(active_tile) == 2)
						{
							ponflag = true;
							pai AfterPonPai = MyPai;
							AfterPonPai.change_tilenum(active_tile, -2);
							AfterPonPai.add_fulu(fulugroup(ke, active_tile.type, active_tile.num, active_tile.num, active_direction));
							afterpon_shouldtile = AfterPonPai.analysis_should(ISeeRemainTiles, match_info, MySeat);
						}
						else if (!FuluLock)
						{
							pai AfterPonPai = MyPai;
							AfterPonPai.change_tilenum(active_tile, -2);
							AfterPonPai.add_fulu(fulugroup(ke, active_tile.type, active_tile.num, active_tile.num, active_direction));
							afterpon_shouldtile = AfterPonPai.analysis_should(ISeeRemainTiles, match_info, MySeat);
							AfterPonPai.change_tilenum(afterpon_shouldtile, -1);
							tingpaiinfo iftempinfo = AfterPonPai.get_tingpaiinfo(ISeeRemainTiles, match_info, MySeat);
							if (iftempinfo.hupossibility - mypaiinfo.hupossibility > pow(10.0, -12))
							{
								ponflag = true;
								afterpon_hupossibility = iftempinfo.hupossibility;
							}
						}
					}
				}
			}
			if (active_tile.type < 3 && MySeat == active_direction + 1 % 4)
			{
				bool chiflag[3] = {false, false, false};
				tingpaiinfo afterchi_info[3];
				
				pai temp_mypai = MyPai;
				for (int chi_way = 0; chi_way < 3; chi_way++)
				{
					if (active_tile.num < 2 - chi_way || active_tile.num > 8 - chi_way) continue;
					
					temp_mypai.change_tilenum(active_tile, 1);
					int temp_chiflag = true;
					for (int k = 0; k < 3; k++)
						if(temp_mypai.get_tilenum(active_tile.type, active_tile.num - 2 + k + chi_way) == 0)
							temp_chiflag = false;
					if (!temp_chiflag) 
					{
						temp_mypai.change_tilenum(active_tile, -1);
						continue;
					}
					for (int k = 0; k < 3; k++)
						temp_mypai.change_tilenum(active_tile.type, active_tile.num - 2 + k + chi_way, -1);
					temp_mypai.add_fulu(fulugroup(shun, active_tile.type, active_tile.num - 2 + chi_way, active_tile.num, (active_direction - MySeat + 4) % 4));
					if (!FuluLock || temp_mypai.iftingpai()) 
						chiflag[chi_way] = true;
					if (chiflag[chi_way])
					{
						chiflag[chi_way] = false;
						afterchi_shouldtile[chi_way] = temp_mypai.analysis_should(ISeeRemainTiles, match_info, MySeat);
						temp_mypai.change_tilenum(afterchi_shouldtile[chi_way], -1);
						afterchi_info[chi_way] = temp_mypai.get_tingpaiinfo(ISeeRemainTiles, match_info, MySeat);
						if (!FuluLock) 
							chiflag[chi_way] = true;
						else if (temp_mypai.iftingpai())
						{
							pai AfterChiHuPai = temp_mypai;
							BYTE *AfterPon_TingPaiTiles = temp_mypai.get_tingpaitiles_byte();
							for (int type_num = 0; type_num < 37; type_num ++)
							{
								if (type_num % 10 == 9) continue;
								if (AfterPon_TingPaiTiles[type_num] == 0) continue;
								AfterChiHuPai.change_tilenum(type_num / 10, type_num % 10, 1);
								hupaiinfo temp = AfterChiHuPai.ifhu(singletile(type_num / 10, type_num % 10), false, 0, match_info, MySeat);
								AfterChiHuPai.change_tilenum(type_num / 10, type_num % 10, -1);
								if(strcmp(temp.fanzhong, "") != 0)
								{
									chiflag[chi_way] = true;
									break;
								}
							}
						}
					}
					for (int k = 0; k < 3; k++)
						temp_mypai.change_tilenum(active_tile.type, active_tile.num - 2 + k + chi_way, 1);
					temp_mypai.change_tilenum(active_tile, -1);
					temp_mypai.delete_fulu();
				}
				if (chiflag[0] || chiflag[1] || chiflag[2])
				{
					if (!get_mypaiinfo)
						mypaiinfo = MyPai.get_tingpaiinfo(ISeeRemainTiles, match_info, MySeat);
					double maxhupossibility = 0.0;
					for (int chi_way = 0; chi_way < 3; chi_way++)
					{
						if (!chiflag[chi_way]) continue;
						if ((afterchi_info[chi_way].hupossibility - maxhupossibility) > pow(10.0, -12))
						{
							final_chiway = chi_way;
							maxhupossibility = afterchi_info[chi_way].hupossibility;
						}
					}
					if (maxhupossibility - mypaiinfo.hupossibility > pow(10.0, -12))
						final_chiflag = true;
					if (final_chiflag && ponflag)
					{
						ponflag = (afterpon_hupossibility - maxhupossibility > pow(10.0, -12));
						final_chiflag = !ponflag;
					}
				}
			}
			if (ponflag)
			{
				LastDecision.response = RESPONSE_PON;
				LastDecision.keytile = afterpon_shouldtile;
				return LastDecision;
			}
			if (final_chiflag)
			{
				LastDecision.response = RESPONSE_CHI_1 << final_chiway;
				LastDecision.keytile = afterchi_shouldtile[final_chiway];
				return LastDecision;
			}
		}
		LastDecision.response = RESPONSE_PASS;
	}
	return LastDecision;
}

singletile MahjongRobot::act(matchinfo &match_info)
{
	direction active_direction = match_info.active_direction;
	singletile active_tile = match_info.active_tile;
	if (!LastDecisionValid) return defaulttile;
	LastDecisionValid = false;
	switch (LastDecision.response)
	{
	case RESPONSE_PASS:
		return defaulttile;
		break;
	case RESPONSE_CHI_1:
	case RESPONSE_CHI_2:
	case RESPONSE_CHI_3:
		{
			int final_chiway = 0;
			for (final_chiway = 0; final_chiway < 3; final_chiway ++)
				if (LastDecision.response == RESPONSE_CHI_1 << final_chiway)
					break;
			MyPai.change_tilenum(active_tile, 1);
			for (int k = 0; k < 3; k++)
				MyPai.change_tilenum(active_tile.type, active_tile.num + k - 2 + final_chiway, -1);
			MyPai.add_fulu(fulugroup(shun, active_tile.type, active_tile.num - 2 + final_chiway, active_tile.num, (active_direction - MySeat + 4) % 4));
			LastDecision.response = RESPONSE_OUT;
			LastDecisionValid = true;
			match_info.active_direction = MySeat;
			return LastDecision.keytile;
			break;
		}
	case RESPONSE_PON:
		MyPai.change_tilenum(active_tile, -2);
		MyPai.add_fulu(fulugroup(ke, active_tile.type, active_tile.num, active_tile.num, (active_direction - MySeat + 4) % 4));
		if (FuluLock && active_tile.type == 3 && (active_tile.num >= 4 || active_tile.num == (MySeat - match_info.this_dealer + 4) % 4 || active_tile.num == match_info.match_wind))
			FuluLock = false;
		LastDecision.response = RESPONSE_OUT;
		LastDecisionValid = true;
		match_info.active_direction = MySeat;
		return LastDecision.keytile;
		break;
	case RESPONSE_KAN:
		if (active_direction == MySeat)
		{
			if (MyPai.get_tilenum(LastDecision.keytile) == 4)
			{
				MyPai.change_tilenum(LastDecision.keytile, -4);
				MyPai.add_fulu(fulugroup(ankan, LastDecision.keytile.type, LastDecision.keytile.num));
			}
			else
			{
				int FuluSum = MyPai.get_fulusum();
				for (int FuluNum = 0; FuluNum < FuluSum; FuluNum ++)
				{
					fulugroup ThisFulu = MyPai.get_fulu(FuluNum);
					if (ThisFulu.thistype == ke && ThisFulu.keytype == LastDecision.keytile.type && ThisFulu.key == LastDecision.keytile.num)
					{
						MyPai.change_tilenum(LastDecision.keytile, -1);
						ThisFulu.thistype = kan;
						ThisFulu.other_num = -1;
						MyPai.delete_fulu(FuluNum);
						MyPai.add_fulu(ThisFulu, FuluNum);
						break;
					}
				}
			}
		}
		else
		{
			MyPai.change_tilenum(LastDecision.keytile, -3);
			MyPai.add_fulu(fulugroup(kan, LastDecision.keytile.type, LastDecision.keytile.num, LastDecision.keytile.num, (active_direction - MySeat + 4) % 4));
		}
		PaiStatus |= PAI_STATUS_RINSHAN;
		match_info.active_direction = MySeat;
		match_info.kansum++;
		break;
	case RESPONSE_RIICHI:
		PaiStatus |= PAI_STATUS_RIICHI;
		PaiStatus |= PAI_STATUS_IPPATSU;
		MyPai.set_paihetile_riichi();
		match_info.RiichiBarSum ++;
		MyPoints -= 1000;
	case RESPONSE_OUT:
		PaiStatus &= ~PAI_STATUS_RINSHAN;
		MyPai.change_tilenum(LastDecision.keytile, -1);
		MyPai.add_paihetile(LastDecision.keytile);
		match_info.active_tile = LastDecision.keytile;
		if (MyPai.iftingpai())
		{
			BYTE* TingPaiTiles = MyPai.get_tingpaitiles_byte();
			for (int RiverNum = 0; RiverNum < MyPai.get_paihesum(); RiverNum++)
			{
				singletile ThisTile = MyPai.get_paihetile(RiverNum);
				if (TingPaiTiles[ThisTile.type * 10 + ThisTile.num] != 0)
					PaiStatus |= PAI_STATUS_FURITEN;
			}
		}
		return LastDecision.keytile;
	case RESPONSE_WIN:
		match_info.win_direction = MySeat;
		match_info.frame_status = FRAME_WIN;
		if (match_info.chankan_possible)
			match_info.kansum --;
		return LastDecision.keytile;
	default:
		break;
	}
	return defaulttile;
}
