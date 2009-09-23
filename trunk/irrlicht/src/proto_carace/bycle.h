#pragma once

#include <irrlicht.h>

#include <windows.h>
#include <mmsystem.h>

#include <comutil.h>
#pragma comment (lib,"comsuppw.lib" )



#include <stdio.h>

#include <irrlicht.h>
#include <irrKlang.h>

//using namespace irr;

#include <string>
#include <vector>
#include <deque>
#include <list>
#include <map>

#include "CFollowAnimator.h"
#include "CBulletAnimatorManager.h"
#include "CBulletWorldAnimator.h"

#include "btBulletDynamicsCommon.h"

#include "CThirdPersonAnimator.h"

#include "TinyXml.h"

#include "tokenizer.h"
#include "ggf_parser.h"
#include "util.h"
#include "irr_utill.h"

#include "Singleton.h"

#include "EsenFrame.h"
#include "EsenObject.h"
#include "FrameMng.h"

//#include "jz3d.h"
#include "jz3dplugins/JZ3DSceneNodeFactory.h"
#include "jz3dplugins/CollusionMngPluginNode.h"
#include "jz3dplugins/FormatedAnimationNode.h"
#include "jz3dplugins/TiledPlaneNode.h"
#include "jz3dplugins/BulletPhysicsObjectNode.h"



HRESULT InitDirectInput( HWND hDlg );
HRESULT UpdateInputState( HWND hDlg );
VOID FreeDirectInput();
extern LONG g_Aixes_X; //아날로그 축 인풋
extern LONG g_Buttons[];

enum ENUM_GUIID
{
	GUIID_START = 0,

	GUIID_LOGIN,	
	GUIID_LOGIN_EDIT,	
	GUIID_LOGIN_OK,
	GUIID_LOGIN_CANCEL,
	GUIID_LOGIN_EDIT_PASS,
	GUIID_LOGIN_USERLIST,

	GUIID_GAMEROOT,
	GUIID_DEBUG_TEXT,
	GUIID_LOADINGMSG,
	GUIID_BTN_FINISH,
	GUIID_BTN_FINISH_NOTSAVE,

	GUIID_GAMEPLAY_ROOT,
	GUIID_GAMEPLAY_TEX_SPEED, //속도표시

	GUIID_GAMEMENU,
	GUIID_BTN_START,	
	GUIID_BTN_MULTI,
	GUIID_BTN_EXIT,
	GUIID_BTN_GAMESTART,

	GUIID_STAGE_SELECT,
	GUIID_BTN_STAGE1,
	GUIID_BTN_STAGE2,
	GUIID_BTN_STAGE3,	
	GUIID_BTN_STAGE4,	
	GUIID_BTN_STAGE5,	
	GUIID_BTN_STAGE6,	
	GUIID_BTN_STAGE7,
	GUIID_CHK_FREEMODE,
	GUIID_BTN_STAGE_SELECT_BACK,
	GUIID_LIST_STAGE_SELECT,
	GUIID_CMB_CAR_SEL,	

	GUIID_END
};

//일리히트 씬오브잭트
class CIRRSceneObject
{
public:
	std::string m_strName;
	std::string m_strFile;
	std::string m_strMainObjNodeName;
};


class CStage_Res
{
public:
	std::string m_strName;
	std::string m_strFile;
	
	std::string m_strSpawnPoint;
	std::string m_strStartLine;
	std::string m_strFinishLine;
	std::string m_strRoad;
	std::string m_strTerrain;
	std::string m_strWall;

	//싸운드
	std::string m_strBgm;
	CIRRSceneObject m_isoBycle;

};

//차량물리 데이터 블럭
class DB_VehiclePhysics
{
public:
	std::string m_strName;
	float m_MaxSpeed;
	float m_EngineForce;
	float m_BreakeForce;
	float m_ResistForce;
	float m_SteeringLimit;
	irr::core::vector3df m_ModelScale;

	struct Body{
		std::string AccelType;//전륜 후륜 여부 ,front,rear,all
		int WheelNum;
		irr::core::vector3df BodySize; //차체크기

		irr::core::vector3df CenterOfMass; //질점위치
		float mass; //질량
		std::string ModelFile;
	};

	struct Suspension {
		float Stiffness;
		float Damping;
		float Compression;
		float RestLength;
	};

	struct Wheel {
		irr::core::vector3df pos;
		float Radius;
		float width;
		float friction;
		float rollInfluence;
		int suspensionIndex;
		std::string ModelFile;
	};

	Body m_Body;
	std::vector<Suspension> m_vtSuspension;
	Wheel m_FR;
	Wheel m_FL;
	Wheel m_RR;
	Wheel m_RL;
	bool LoadXml(const char *fn);
};

//유아이 데이터 블럭
class CDBlock_UI
{
public:
	struct S_UIELEMENT {
		irr::core::rect<irr::s32> body[2];
		std::string caption;
		std::string name;
		std::string type;
	};
	
	S_UIELEMENT	GUIID_LOGIN;
	S_UIELEMENT GUIID_LOGIN_EDIT;	
	S_UIELEMENT GUIID_LOGIN_OK;
	S_UIELEMENT GUIID_LOGIN_CANCEL;
	S_UIELEMENT GUIID_LOGIN_EDIT_PASS;
	S_UIELEMENT GUIID_LOGIN_USERLIST;

	S_UIELEMENT	GUIID_GAMEROOT;
	S_UIELEMENT	GUIID_DEBUG_TEXT;
	S_UIELEMENT	GUIID_LOADINGMSG;
	S_UIELEMENT	GUIID_BTN_FINISH;
	S_UIELEMENT	GUIID_BTN_FINISH_NOTSAVE;

	S_UIELEMENT	GUIID_GAMEPLAY_ROOT;
	S_UIELEMENT	GUIID_GAMEPLAY_TEX_SPEED;

	S_UIELEMENT	GUIID_GAMEMENU;
	S_UIELEMENT	GUIID_BTN_START;	
	S_UIELEMENT	GUIID_BTN_MULTI;
	S_UIELEMENT	GUIID_BTN_EXIT;
	S_UIELEMENT	GUIID_BTN_GAMESTART;

	S_UIELEMENT	GUIID_STAGE_SELECT;
	S_UIELEMENT	GUIID_BTN_STAGE1;
	S_UIELEMENT	GUIID_BTN_STAGE2;
	S_UIELEMENT	GUIID_BTN_STAGE3;	
	S_UIELEMENT	GUIID_BTN_STAGE4;	
	S_UIELEMENT	GUIID_BTN_STAGE5;	
	S_UIELEMENT	GUIID_BTN_STAGE6;	
	S_UIELEMENT	GUIID_BTN_STAGE7;
	S_UIELEMENT	GUIID_CHK_FREEMODE;
	S_UIELEMENT	GUIID_BTN_STAGE_SELECT_BACK;
	S_UIELEMENT	GUIID_LIST_STAGE_SELECT;
	S_UIELEMENT	GUIID_CMB_CAR_SEL;
};

class CConfigData
{
public:
	CConfigData()
	{
	}
	~CConfigData()
	{
	}

	//어플리캐이션정의
	std::string m_strLoginID;
	std::string m_strLoginPass;
	std::string m_strSeverAddr;
	int m_nServerPort;
	bool m_bFullScreen;
	int m_anScreenSize[2];
	bool m_bDebugMode;
	int	m_nGUITran;
	std::string m_strStartAppFrame;	
	irr::video::E_DRIVER_TYPE m_DriverType;	

	std::string m_strResFolder;

	std::string m_strStartPoint;
	std::string m_strFinishPoint;

	std::string m_strBycleNode;
	
	//collision
	std::string m_strTerrainNode;
	std::string m_strRoadNode;
	std::string m_strWallNode;

	
	//camera
	std::string m_strCamNode;
	std::string m_strCamControlNode;
	float m_fCamDist;	
	float m_fCamAngle;
	float m_fCamfollowsensivity;
	irr::core::vector3df m_CamLookAtPosDiff;

	vector<CStage_Res> m_vtStageRes;
	vector<std::string> m_vtVehiclePhysicsFile;
	vector<std::string> m_vtVehicleName;
	
	ggf::parser::VectorInt m_viDbg;
	ggf::parser::VectorFloat m_vfDbg;

	//ui
	CDBlock_UI m_uiDBlock;

};


class CSystem
{
public:
	bool m_bIsReady; //시스템동작준비완료

	irr::IrrlichtDevice *m_pDevice;
	irr::video::IVideoDriver *m_pDriver;
	irr::scene::ISceneManager *m_pSmgr;	
	irrklang::ISoundEngine* m_pSoundEngine;

	btClock m_clock;

	CFrameMng *m_pFrameMng;
	

//	ggf::object::CObjectMng		m_ObjectMng;	//3D
//	ggf::layer::CLayerMng		m_LayerMng;		//2D
	irr::gui::IGUIFont			*m_pStdFont;		
//	irr::gui::CGUIFont_Korea	*m_pKoreaFont;		
	bool m_bPlaySound;
	int m_nGameType;	

	CConfigData m_ConfigData;

	
	CSystem()
	{
		m_bIsReady = false;
		m_pDevice = 0;
		m_pFrameMng =NULL;
//		m_pKoreaFont = NULL;
		//m_pDebugLog = new CLog("DebugLog");		
		//m_pFileLogAppender = new CFileLogAppender(nsCLog::debug, "../debug.dat");
		//m_pDebugLog->addAppender(m_pFileLogAppender);	
		//m_pWinLock = new cDWinlock();

#ifndef _DEBUG
		
		//m_pWinLock->DisableKey(0,VK_DELETE,MOD_CONTROL | MOD_ALT,1); // disables Ctrl+Alt+Del
		//m_pWinLock->DisableKey(0,VK_TAB,MOD_ALT,1);
		//m_pWinLock->DisableKey(0,VK_F4,MOD_ALT,1);
		//m_pWinLock->DisableKey(0,0,MOD_WIN,1);
		//m_pWinLock->DisableKey(0,0,MOD_ALT,1);		
#endif		
	}

	~CSystem()
	{
		m_bIsReady = false;
		

		if(m_pDevice)
		{			
			delete m_pFrameMng;		
			m_pDevice->drop();			
			
		}

		
		
	}	

};

class CUserData
{
public:
	std::string m_strID;
	std::string m_strPass;
	std::string m_strName;
	std::string m_strPhone;
	std::string m_strBirthDay;
};

class CRecordData
{
public:
	std::string m_strID;
	float m_fRaceTime; //주행시간
	std::string m_strCourse; //주행코스이름
	SYSTEMTIME m_RecordTime; //기록시간
};


//객체 해제순서는
//클래스의 파괴자가 호출된후 멤버들의 파괴자가 차례로 호출된다.

class CBycleApp : 
	public irr::IEventReceiver,
	public ggf::oop::Singleton<CBycleApp>
{
private:
public:		

	CSystem m_System;	
	irr::s32 m_nLastFps;
	irr::s32 m_nBestFps;	

	std::string m_strUserID;
	int m_nStage;	
	
	irrklang::ISound *m_pBGMsound;
	irrklang::ISound *m_pFxEngineSound[5];

	std::map<std::string,CUserData> m_Users;
	std::list<CRecordData> m_Records;

	irr::gui::IGUIFont* m_pFont_Arial36;
	irr::gui::IGUIFont* m_pFont_ms12;

	irr::scene::CBulletAnimatorManager* m_pBulletPhysicsFactory;

#ifdef USE_NIIO

	irr::f32 m_fAI_1;
	irr::f32 m_fAI_2;
	irr::f32 m_fAI_3;
	std::auto_ptr<CNiDAQmxTask> m_spNiDaqTask;
	std::auto_ptr<CNiDAQmxAnalogSingleChannelReader> m_spNiDaqReader;

	std::auto_ptr<CNiDAQmxTask> m_spNiDaqTask2;
	std::auto_ptr<CNiDAQmxAnalogSingleChannelReader> m_spNiDaqReader2;

	std::auto_ptr<CNiDAQmxTask> m_spNiDaqTask3;
	std::auto_ptr<CNiDAQmxAnalogSingleChannelReader> m_spNiDaqReader3;
#endif

	irr::u32 m_uIODelayTick;

	

	CBycleApp()
	{
		::_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF); //메모리릭 탐지

		m_nBestFps = 0;	
		m_nLastFps = 0;
		m_nStage = 0;
		m_strUserID = "";
		m_uIODelayTick = 0;
		m_pBGMsound = 0;
		//m_nIdleCounter = 0;
	}

	~CBycleApp()
	{	
		m_pBulletPhysicsFactory->drop();		
	}	

	
	//게임 리소스 씬정보 생성 및 어플리캐이션 초기화	
	//strResName : 리소스폴더이름
	int Create(char *szResName);
	
	bool InitNiIO();
	
	bool ProcessLogin(const wchar_t *wszID,const wchar_t *wszPass)
	{
		//irr::gui::IGUIEditBox *pEdit = (irr::gui::IGUIEditBox *)pEnv->getRootGUIElement()->getElementFromId(GUIID_LOGIN_EDIT,true);
		//irr::gui::IGUIEditBox *pEditPass = (irr::gui::IGUIEditBox *)pEnv->getRootGUIElement()->getElementFromId(GUIID_LOGIN_EDIT_PASS,true);
		m_strUserID = bstr_t(wszID);
		if(m_Users.find(m_strUserID) != m_Users.end() )
		{
			if( m_Users[m_strUserID].m_strPass == std::string(bstr_t(wszPass)) )
			{
				return true;
				//hideAllMenu();
				//SetPhase(PH_MAINMENU,0);
			}
		}

		return false;
	}

	CStage_Res &getCurrentStageRes()
	{
		CConfigData *pConfig = &(CBycleApp::GetPtr()->m_System.m_ConfigData);		
		return pConfig->m_vtStageRes[m_nStage];
	}


	bool SaveRecord(float fRecord)
	{
		//irr::io::IWriteFile *pwf CBycleApp::Get()->m_System.m_pDevice->getFileSystem()->createAndWriteFile("../out/record.bin",true);
		//irr::io::IXMLWriter *pXml = CBycleApp::Get()->m_System.m_pDevice->getFileSystem()->createXMLWriter(pwf);
		//pXml->
		SYSTEMTIME St;

		//국가별 지역시간얻기
		//GetSystemTime은 세계표준 시간을 반환한다.					

		GetLocalTime(&St);


		FILE *fp;
		fopen_s(&fp,"../out/record.csv","a+");

		if(!fp) return false; //실패하면
		

		fprintf_s(fp,"%s,%s,%f,%d-%d-%d %d:%d\n",
			m_strUserID.c_str(),
			getCurrentStageRes().m_strName.c_str(),
			fRecord,
			St.wYear,
			St.wMonth,
			St.wDay,
			St.wHour,
			St.wMinute
			);
		fclose(fp);

		return true;
	}

	void playBgm()
	{
		//사운드 로딩 & 연주
		if( IsBGMOffMode()) return;		

		std::string strPath = CBycleApp::GetPtr()->m_System.m_ConfigData.m_strResFolder;
		//strPath += "/res/";
		std::string strOldPath;


		strOldPath = CBycleApp::GetPtr()->m_System.m_pDevice->getFileSystem()->getWorkingDirectory();

		CBycleApp::GetPtr()->m_System.m_pDevice->getFileSystem()->changeWorkingDirectoryTo(strPath.c_str());		

		m_pBGMsound = CBycleApp::GetPtr()->m_System.m_pSoundEngine->play2D(getCurrentStageRes().m_strBgm.c_str(),true,true);
		m_pBGMsound->setIsPaused(false);
		

		int i;
		for(i=0;i<5;i++)
		{
			char strBuf[256];
			sprintf_s(strBuf,256,"res/sound/engine_%d.wav",i+1);
			m_pFxEngineSound[i] = CBycleApp::GetPtr()->m_System.m_pSoundEngine->play2D(strBuf,true,true);
			m_pFxEngineSound[i]->setIsPaused(true);
		}

		CBycleApp::GetPtr()->m_System.m_pDevice->getFileSystem()->changeWorkingDirectoryTo(strOldPath.c_str());
	}

	void stopBgm()
	{
		if(m_pBGMsound)
		{
			m_pBGMsound->stop();		
			m_pBGMsound->drop();
		}
	}

	void hideAllMenu()
	{
		irr::gui::IGUIEnvironment *pEnv = CBycleApp::m_System.m_pDevice->getGUIEnvironment();	
		pEnv->getRootGUIElement()->getElementFromId(GUIID_GAMEROOT)->setVisible(false);
		pEnv->getRootGUIElement()->getElementFromId(GUIID_LOGIN)->setVisible(false);
		pEnv->getRootGUIElement()->getElementFromId(GUIID_GAMEMENU)->setVisible(false);
		pEnv->getRootGUIElement()->getElementFromId(GUIID_STAGE_SELECT)->setVisible(false);
	}

	void activateGameResultTab()
	{
		irr::gui::IGUIEnvironment *pEnv = CBycleApp::m_System.m_pDevice->getGUIEnvironment();	
		pEnv->getRootGUIElement()->getElementFromId(GUIID_GAMEROOT)->setVisible(true);
	}
	void DeactivateGameResultTab()
	{
		irr::gui::IGUIEnvironment *pEnv = CBycleApp::m_System.m_pDevice->getGUIEnvironment();	
		pEnv->getRootGUIElement()->getElementFromId(GUIID_GAMEROOT)->setVisible(false);
	}
	
	bool IsNiMode()
	{
		if(!m_System.m_ConfigData.m_viDbg[9])
			return false;
		return true;
	}

	bool IsBGMOffMode()
	{
		if(!m_System.m_ConfigData.m_viDbg[8])
			return false;
		return true;
	}

	bool IsFXOffMode()
	{
		if(!m_System.m_ConfigData.m_viDbg[4])
			return false;
		return true;
	}

	bool IsDebugMode()
	{
		return m_System.m_ConfigData.m_bDebugMode;
	}

	bool IsHideVehicleMode()
	{
		if(m_System.m_ConfigData.m_viDbg[1])
			return true;
		return false;
	}

	bool IsShowDbgText()
	{
		if(m_System.m_ConfigData.m_viDbg[2])
			return true;
		return false;
	}

	virtual bool OnEvent(const irr::SEvent &ev);	

	bool LoadSetting(char *filename);	
	bool LoadGUIDBlock(char *filename);	

	//int Init();			
	void MainLoop();

};




