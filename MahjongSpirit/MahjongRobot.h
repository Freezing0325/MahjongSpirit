#pragma once
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#ifndef _CRT_NONSTDC_NO_DEPRECATE 
#define _CRT_NONSTDC_NO_DEPRECATE 
#endif

#define PAI_STATUS_NORMAL 0x0
#define PAI_STATUS_RIICHI 0x1
#define PAI_STATUS_WRIICHI 0x2
#define PAI_STATUS_IPPATSU 0x4
#define PAI_STATUS_RINSHAN 0x8
#define PAI_STATUS_FURITEN 0x10

#include "pai.h"
#include "structs.h"
#include "stdafx.h"


extern const singletile defaulttile;
extern const fuluinfo menqianqing;

class MahjongRobot
{
private:
	CString RobotName;
	pai MyPai;
	direction MySeat;
	int MyPoints;
	BYTE PaiStatus;
	bool FuluLock;
	robotresponse LastDecision;
	bool LastDecisionValid;
	tiles ISeeRemainTiles;
	bool AutoMode;
public:

	MahjongRobot();
	MahjongRobot(const MahjongRobot &other);
	void SetRobotName(const CString &SetRobotName){RobotName = SetRobotName;};
	void set_pai(pai SetMyPai){MyPai = SetMyPai;};
	void set_seat(direction SetMySeat){MySeat = SetMySeat;};
	void SetPoints(int SetMyPoints){MyPoints = SetMyPoints;};

	void reset_pai();
	pai get_pai(){return MyPai;};
	direction get_seat(){return MySeat;};
	CString GetRobotName(){return RobotName;};
	int GetPoints(){return MyPoints;};
	void ChangePoints(int ChangeNum){MyPoints += ChangeNum;};

	BYTE get_paistatus(){return PaiStatus;};
	void add_paistatus(BYTE PaiStatus);
	void remove_paistatus(BYTE PaiStatus);
	
	void set_paihetile_invisible();
	void abandon_response(){LastDecisionValid = false;}

	void set_automode(bool GoalMode){AutoMode = GoalMode;}

	robotresponse get_response(matchinfo match_info, const fuluinfo (&all_fulu)[4], const paihe (&all_paihe)[4]);
	singletile act(matchinfo &match_info);
};