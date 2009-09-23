// bycle.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "bycle.h"

#include "GameFrame.h"
#include "MainMenuFrame.h"
#include "tinyxml.h"

#include "tokenizer.h"
#include "ggf_parser.h"



// 유일한 응용 프로그램 개체입니다.
//CWinApp theApp;
using namespace std;
using namespace irr;

//CSystem		CBycleApp::m_System;
//CBycleApp*	CBycleApp::ms_pBycleApp;
//CBycleApp theGameApp;

bool DB_VehiclePhysics::LoadXml(const char *fn)
{
	TiXmlDocument doc(fn);
	
	bool loadOkay = doc.LoadFile();
	if ( !loadOkay )
	{
		ggf::irr_util::DebugOutputFmt(NULL, "Could not load test file user DB file 'user.xml'. Error='%s'. Exiting.\n", doc.ErrorDesc() );		
		return false;
	}

	std::string strTemp;
	ggf::parser::VectorInt vi;
	ggf::parser::VectorFloat vf;
	ggf::parser::VectorString vs;
	TiXmlNode *pRoot;
	TiXmlElement *pElement;
	pRoot = doc.RootElement();


	pElement = pRoot->FirstChildElement("header");

	m_strName = pElement->Attribute("name");
	ggf::parser::GetAttrib(pElement->Attribute("MaxSpeed"),m_MaxSpeed);
	ggf::parser::GetAttrib(pElement->Attribute("EngineForce"),m_EngineForce);
	ggf::parser::GetAttrib(pElement->Attribute("BreakForce"),m_BreakeForce);
	ggf::parser::GetAttrib(pElement->Attribute("ResistForce"),m_ResistForce);
	ggf::parser::GetAttrib(pElement->Attribute("SteeringLimit"),m_SteeringLimit);	
	ggf::parser::GetAttrib(pElement->Attribute("modelscale"),m_ModelScale);	


	pElement = pRoot->FirstChildElement("suspension");

	while(pElement)
	{
		Suspension Sus;
		ggf::parser::GetAttrib(pElement->Attribute("Stiffness"),Sus.Stiffness);
		ggf::parser::GetAttrib(pElement->Attribute("Damping"),Sus.Damping);
		ggf::parser::GetAttrib(pElement->Attribute("Compression"),Sus.Compression);
		ggf::parser::GetAttrib(pElement->Attribute("RestLength"),Sus.RestLength);

		m_vtSuspension.push_back(Sus);

		pElement = pElement->NextSiblingElement("suspension");
	}

	pElement = pRoot->FirstChildElement("body");
	m_Body.AccelType = pElement->Attribute("Accel");
	ggf::parser::GetAttrib(pElement->Attribute("wheelNum"),m_Body.WheelNum);
	ggf::parser::GetAttrib(pElement->Attribute("size"),m_Body.BodySize);
	ggf::parser::GetAttrib(pElement->Attribute("mass"),m_Body.mass);
	ggf::parser::GetAttrib(pElement->Attribute("centerOfMass"),m_Body.CenterOfMass);
	m_Body.ModelFile = pElement->Attribute("model");


	{
		pElement = pRoot->FirstChildElement("wheelFR");
		Wheel wh;
		ggf::parser::GetAttrib(pElement->Attribute("pos"),wh.pos);		
		ggf::parser::GetAttrib(pElement->Attribute("Radius"),wh.Radius);
		ggf::parser::GetAttrib(pElement->Attribute("width"),wh.width);
		ggf::parser::GetAttrib(pElement->Attribute("friction"),wh.friction);
		ggf::parser::GetAttrib(pElement->Attribute("rollInfluence"),wh.rollInfluence);
		ggf::parser::GetAttrib(pElement->Attribute("suspension"),wh.suspensionIndex);
		
		ggf::parser::GetAttrib(pElement->Attribute("model"),wh.ModelFile);

		m_FR = wh;
	}

	{
		pElement = pRoot->FirstChildElement("wheelFL");
		Wheel wh;
		ggf::parser::GetAttrib(pElement->Attribute("pos"),wh.pos);
		ggf::parser::GetAttrib(pElement->Attribute("Radius"),wh.Radius);
		ggf::parser::GetAttrib(pElement->Attribute("width"),wh.width);
		ggf::parser::GetAttrib(pElement->Attribute("friction"),wh.friction);
		ggf::parser::GetAttrib(pElement->Attribute("rollInfluence"),wh.rollInfluence);
		ggf::parser::GetAttrib(pElement->Attribute("suspension"),wh.suspensionIndex);		
		ggf::parser::GetAttrib(pElement->Attribute("model"),wh.ModelFile);
		m_FL = wh;
	}

	{
		pElement = pRoot->FirstChildElement("wheelRR");
		Wheel wh;
		ggf::parser::GetAttrib(pElement->Attribute("pos"),wh.pos);
		ggf::parser::GetAttrib(pElement->Attribute("width"),wh.width);
		ggf::parser::GetAttrib(pElement->Attribute("Radius"),wh.Radius);
		ggf::parser::GetAttrib(pElement->Attribute("friction"),wh.friction);
		ggf::parser::GetAttrib(pElement->Attribute("rollInfluence"),wh.rollInfluence);
		ggf::parser::GetAttrib(pElement->Attribute("suspension"),wh.suspensionIndex);		
		ggf::parser::GetAttrib(pElement->Attribute("model"),wh.ModelFile);
		m_RR = wh;
	}
	{
		pElement = pRoot->FirstChildElement("wheelRL");
		Wheel wh;
		ggf::parser::GetAttrib(pElement->Attribute("pos"),wh.pos);
		ggf::parser::GetAttrib(pElement->Attribute("width"),wh.width);
		ggf::parser::GetAttrib(pElement->Attribute("Radius"),wh.Radius);
		ggf::parser::GetAttrib(pElement->Attribute("friction"),wh.friction);
		ggf::parser::GetAttrib(pElement->Attribute("rollInfluence"),wh.rollInfluence);
		ggf::parser::GetAttrib(pElement->Attribute("suspension"),wh.suspensionIndex);
		ggf::parser::GetAttrib(pElement->Attribute("model"),wh.ModelFile);

		m_RL = wh;
	}



	return true;	
}

///////////////////////////////////////////////////////////////////////////////

bool CBycleApp::LoadGUIDBlock(char *filename)
{
	ggf::irr_util::DebugOutputFmt(NULL,"Load UI DataBlock....\n");
	//설정로드,초기화	

	CConfigData *pConfig = &m_System.m_ConfigData;

	TiXmlDocument doc(filename);
	bool loadOkay = doc.LoadFile();
	if ( !loadOkay )
	{
		ggf::irr_util::DebugOutputFmt(NULL, "Could not load test file 'ui.xml'. Error='%s'. Exiting.\n", doc.ErrorDesc() );		
		return false;
	}
	else
	{
		TiXmlNode *pRoot;		
		TiXmlElement *pElement;
		TiXmlElement *pElement2;
		pRoot = doc.RootElement();

		//디버깅플래그...
		pElement = pRoot->FirstChildElement("main_frame");

		pElement2 = pElement->FirstChildElement("GUIID_LOGIN");
		ggf::parser::GetAttrib(pElement2->Attribute("rect"),pConfig->m_uiDBlock.GUIID_LOGIN.body[0]);

		pElement2 = pElement->FirstChildElement("GUIID_LOGIN_USERLIST");
		ggf::parser::GetAttrib(pElement2->Attribute("rect"),pConfig->m_uiDBlock.GUIID_LOGIN_USERLIST.body[0]);

		pElement2 = pElement->FirstChildElement("GUIID_LOGIN_OK");
		ggf::parser::GetAttrib(pElement2->Attribute("rect"),pConfig->m_uiDBlock.GUIID_LOGIN_OK.body[0]);
		ggf::parser::GetAttrib(pElement2->Attribute("caption"),pConfig->m_uiDBlock.GUIID_LOGIN_OK.caption);

		pElement2 = pElement->FirstChildElement("GUIID_LOGIN_CANCEL");
		ggf::parser::GetAttrib(pElement2->Attribute("rect"),pConfig->m_uiDBlock.GUIID_LOGIN_CANCEL.body[0]);
		ggf::parser::GetAttrib(pElement2->Attribute("caption"),pConfig->m_uiDBlock.GUIID_LOGIN_CANCEL.caption);

		pElement = pRoot->FirstChildElement("GameMenu");

		pElement2 = pElement->FirstChildElement("GUIID_GAMEMENU");
		ggf::parser::GetAttrib(pElement2->Attribute("rect"),pConfig->m_uiDBlock.GUIID_GAMEMENU.body[0]);

		pElement2 = pElement->FirstChildElement("GUIID_BTN_START");
		ggf::parser::GetAttrib(pElement2->Attribute("rect"),pConfig->m_uiDBlock.GUIID_BTN_START.body[0]);
		ggf::parser::GetAttrib(pElement2->Attribute("caption"),pConfig->m_uiDBlock.GUIID_BTN_START.caption);

		pElement2 = pElement->FirstChildElement("GUIID_BTN_MULTI");
		ggf::parser::GetAttrib(pElement2->Attribute("rect"),pConfig->m_uiDBlock.GUIID_BTN_MULTI.body[0]);
		ggf::parser::GetAttrib(pElement2->Attribute("caption"),pConfig->m_uiDBlock.GUIID_BTN_MULTI.caption);

		pElement2 = pElement->FirstChildElement("GUIID_BTN_EXIT");
		ggf::parser::GetAttrib(pElement2->Attribute("rect"),pConfig->m_uiDBlock.GUIID_BTN_EXIT.body[0]);
		ggf::parser::GetAttrib(pElement2->Attribute("caption"),pConfig->m_uiDBlock.GUIID_BTN_EXIT.caption);

		pElement = pRoot->FirstChildElement("StageSelect");

		pElement2 = pElement->FirstChildElement("GUIID_STAGE_SELECT");
		ggf::parser::GetAttrib(pElement2->Attribute("rect"),pConfig->m_uiDBlock.GUIID_STAGE_SELECT.body[0]);

		pElement2 = pElement->FirstChildElement("GUIID_BTN_STAGE_SELECT_BACK");
		ggf::parser::GetAttrib(pElement2->Attribute("rect"),pConfig->m_uiDBlock.GUIID_BTN_STAGE_SELECT_BACK.body[0]);
		ggf::parser::GetAttrib(pElement2->Attribute("caption"),pConfig->m_uiDBlock.GUIID_BTN_STAGE_SELECT_BACK.caption);

		pElement2 = pElement->FirstChildElement("GUIID_BTN_GAMESTART");
		ggf::parser::GetAttrib(pElement2->Attribute("rect"),pConfig->m_uiDBlock.GUIID_BTN_GAMESTART.body[0]);
		ggf::parser::GetAttrib(pElement2->Attribute("caption"),pConfig->m_uiDBlock.GUIID_BTN_GAMESTART.caption);		

		pElement2 = pElement->FirstChildElement("GUIID_CMB_CAR_SEL");
		ggf::parser::GetAttrib(pElement2->Attribute("rect"),pConfig->m_uiDBlock.GUIID_CMB_CAR_SEL.body[0]);
		ggf::parser::GetAttrib(pElement2->Attribute("caption"),pConfig->m_uiDBlock.GUIID_CMB_CAR_SEL.caption);
		ggf::parser::GetAttrib(pElement2->Attribute("title_rect"),pConfig->m_uiDBlock.GUIID_CMB_CAR_SEL.body[1]);

		pElement2 = pElement->FirstChildElement("GUIID_LIST_STAGE_SELECT");
		ggf::parser::GetAttrib(pElement2->Attribute("rect"),pConfig->m_uiDBlock.GUIID_LIST_STAGE_SELECT.body[0]);
		ggf::parser::GetAttrib(pElement2->Attribute("caption"),pConfig->m_uiDBlock.GUIID_LIST_STAGE_SELECT.caption);		
		ggf::parser::GetAttrib(pElement2->Attribute("title_rect"),pConfig->m_uiDBlock.GUIID_LIST_STAGE_SELECT.body[1]);
	}
	return true;
}

bool CBycleApp::LoadSetting(char *fn)
{
	ggf::irr_util::DebugOutputFmt(NULL,"Load Config....\n");
	//설정로드,초기화	

	CConfigData *pConfig = &m_System.m_ConfigData;

	TiXmlDocument doc(fn);
	bool loadOkay = doc.LoadFile();
	if ( !loadOkay )
	{
		ggf::irr_util::DebugOutputFmt(NULL, "Could not load test file 'system.xml'. Error='%s'. Exiting.\n", doc.ErrorDesc() );		
		return false;
	}
	else
	{
		std::string strTemp;
		ggf::parser::VectorInt vi;
		ggf::parser::VectorFloat vf;
		ggf::parser::VectorString vs;
		TiXmlNode *pRoot;
		//TiXmlAttribute *pAttr;
		TiXmlElement *pElement;

		pRoot = doc.RootElement();

		
		//디버깅플래그...
		pElement = pRoot->FirstChildElement("DEVTest");
		ggf::parser::GetAttrib(pElement->Attribute("int"),m_System.m_ConfigData.m_viDbg);
		ggf::parser::GetAttrib(pElement->Attribute("float"),m_System.m_ConfigData.m_vfDbg);

		pElement = pRoot->FirstChildElement("Server");
		ggf::irr_util::DebugOutputFmt(NULL,"server ip: %s \n",pElement->Attribute("ip"));
		ggf::irr_util::DebugOutputFmt(NULL,"server ip: %s \n",pElement->Attribute("port"));

		//app mode setting		
		{

			pElement = pRoot->FirstChildElement("AppMode");
			ggf::parser::GetAttrib(pElement->Attribute("DebugMode"),m_System.m_ConfigData.m_bDebugMode);				

			pElement->Attribute("GUITran",&m_System.m_ConfigData.m_nGUITran);
			m_System.m_ConfigData.m_strStartAppFrame = pElement->Attribute("StartAppFrame");


			{
				pElement = pElement->FirstChildElement("Screen");
				ggf::parser::GetAttrib(pElement->Attribute("size"),vi);
				m_System.m_ConfigData.m_anScreenSize[0] = vi[0];
				m_System.m_ConfigData.m_anScreenSize[1] = vi[1];
				ggf::parser::GetAttrib(pElement->Attribute("FullScreen"),m_System.m_ConfigData.m_bFullScreen);

				strTemp = pElement->Attribute("Renderer");
				if(strTemp == "dx9")
				{
					m_System.m_ConfigData.m_DriverType = irr::video::EDT_DIRECT3D9;					
				}
				else if(strTemp == "opengl")
				{
					m_System.m_ConfigData.m_DriverType = irr::video::EDT_OPENGL;					
				}
				else
				{
					m_System.m_ConfigData.m_DriverType = irr::video::EDT_SOFTWARE;
				}
				pElement = pElement->Parent()->ToElement();
			}

			{
				//pElement = pElement->FirstChildElement("Resource");
				//ggf::parser::GetAttrib(pElement->Attribute("name"),m_System.m_ConfigData.m_strResFolder);						
				//m_System.m_ConfigData.m_strResFolder = "../res/" + m_System.m_ConfigData.m_strResFolder;				
				//pElement = pElement->Parent()->ToElement();
				//m_System.m_ConfigData.m_strResFolder = "../res/" + strRes
			}
		}

		pElement = pRoot->FirstChildElement("GamePlay");
		{
			pElement = pElement->FirstChildElement("Camera");
			{
				//m_System.m_ConfigData.m_strCamNode = pElement->Attribute("node");
				//m_System.m_ConfigData.m_strCamControlNode = pElement->Attribute("control_node");
				//double temp;
				/*pElement->Attribute("dist",&temp);
				m_System.m_ConfigData.m_fCamDist = (float)temp;
				pElement->Attribute("angle",&temp);
				m_System.m_ConfigData.m_fCamAngle = (float)temp;*/

				ggf::parser::GetAttrib(pElement->Attribute("dist"),m_System.m_ConfigData.m_fCamDist);
				ggf::parser::GetAttrib(pElement->Attribute("lookAtpos"),m_System.m_ConfigData.m_CamLookAtPosDiff);
				ggf::parser::GetAttrib(pElement->Attribute("followsensivity"),m_System.m_ConfigData.m_fCamfollowsensivity);
			}
			pElement = pElement->Parent()->ToElement();

			
			//pElement = pElement->FirstChildElement("BycleObj");
			//{	
				//m_System.m_ConfigData.m_strBycleNode = pElement->Attribute("node_name");				
			//}
			//pElement = pElement->Parent()->ToElement();

			pElement = pElement->FirstChildElement("Vehicle");

			while(pElement)
			{
				strTemp = pElement->Attribute("file");
				m_System.m_ConfigData.m_vtVehiclePhysicsFile.push_back(strTemp);

				strTemp = pElement->Attribute("name");
				m_System.m_ConfigData.m_vtVehicleName.push_back(strTemp);

				pElement = pElement->NextSiblingElement("Vehicle");
			}
		}

		pElement = pRoot->FirstChildElement("stage");
		{
			while(pElement)
			{
				CStage_Res stage;
				stage.m_strName = pElement->Attribute("name");
				stage.m_strFile = pElement->Attribute("file_name");
				pElement = pElement->FirstChildElement("point_spot");
				{
					stage.m_strSpawnPoint = pElement->Attribute("spawn");
					stage.m_strStartLine  = pElement->Attribute("start");
					stage.m_strFinishLine = pElement->Attribute("finish");
					pElement = pElement->Parent()->ToElement();
				}
				pElement = pElement->FirstChildElement("collision");
				{
					stage.m_strRoad = pElement->Attribute("road");
					stage.m_strTerrain = pElement->Attribute("terrain");
					stage.m_strWall = pElement->Attribute("wall");

					pElement = pElement->Parent()->ToElement();
				}

				pElement = pElement->FirstChildElement("sound");
				{	
					stage.m_strBgm = pElement->Attribute("bgm");

					stage.m_strBgm = "res/sound/" + stage.m_strBgm;

					pElement = pElement->Parent()->ToElement();
				}

				pElement = pElement->FirstChildElement("CIRRSceneObject");
				{
					TiXmlElement *pTempElement = pElement;

					while(pElement) {

						if(std::string("BycleObj") == pElement->Attribute("name"))
						{
							stage.m_isoBycle.m_strName =  pElement->Attribute("name");
							stage.m_isoBycle.m_strMainObjNodeName =  pElement->Attribute("main_node");
							stage.m_isoBycle.m_strFile =  pElement->Attribute("file");
						}

						pElement = pElement->NextSiblingElement("CIRRSceneObject");
					}

					pElement = pTempElement->Parent()->ToElement();
				}

				pElement = pElement->NextSiblingElement("stage");

				pConfig->m_vtStageRes.push_back(stage);

				
			}
		}

		ggf::irr_util::DebugOutputFmt(NULL,"parse success!!\n");		
	}
	
	
	return true;
}

int CBycleApp::Create(char *szResName)
{

	//ms_pBycleApp = new CBycleApp();	

	std::string strResName(szResName);

	//strResName = std::string("../res/") + strResName;

	std::string strSystemXml = strResName + std::string("/script/system.xml");
	std::string strUiXml = strResName + std::string("/script/ui.xml");

	//if(LoadSetting("../res/bme/script/system.xml") == false)
	m_System.m_ConfigData.m_strResFolder = strResName;
	if(LoadSetting((char *)strSystemXml.c_str()) == false)
	{		
		//m_System.m_pDevice->getLogger()->log("");
		return 1;
	}

	//if(LoadGUIDBlock("../res/bme/script/ui.xml") == false)
	if(LoadGUIDBlock((char *)strUiXml.c_str()) == false)
	{		
		//m_System.m_pDevice->getLogger()->log("");
		return 1;
	}

	//유저 정보로딩
	{
		std::string userfile = std::string(m_System.m_ConfigData.m_strResFolder + "/database/user.xml");
		TiXmlDocument doc(userfile.c_str());
		
		bool loadOkay = doc.LoadFile();
		if ( !loadOkay )
		{
			ggf::irr_util::DebugOutputFmt(NULL,"Could not load test file user DB file 'user.xml'. Error='%s'. Exiting.\n", doc.ErrorDesc() );
			//exit( 1 );
			return false;
		}
		else
		{				

			TiXmlNode *pRoot;
			
			TiXmlElement *pElement;
			TiXmlElement *pNextElement;

			pRoot = doc.RootElement();

			//디버깅플래그...
			pElement = pRoot->FirstChildElement("users");
			

			pNextElement = pElement->FirstChildElement("entry");
			while(pNextElement)
			{
				CUserData userData;				
				
				userData.m_strPass = pNextElement->Attribute("passwd");
				userData.m_strID = pNextElement->Attribute("id");

				userData.m_strName = pNextElement->Attribute("name");
				userData.m_strPhone = pNextElement->Attribute("phone");
				userData.m_strBirthDay = pNextElement->Attribute("birthday");

				if(Get().m_Users.find(userData.m_strID) == Get().m_Users.end())
				{
					Get().m_Users[userData.m_strID] = userData;
				}
				else
				{
					ggf::irr_util::DebugOutputFmt(NULL,"duplicate id error \n,%s", userData.m_strID.c_str());
				}


				pNextElement = pNextElement->NextSiblingElement("entry");
				
			}
		}
	}

	
	ggf::irr_util::DebugOutputFmt(NULL,"Start 3dEngine....\n");
	{//엔진세팅	
		
		m_System.m_pDevice = irr::createDevice(
			m_System.m_ConfigData.m_DriverType,
			irr::core::dimension2di(m_System.m_ConfigData.m_anScreenSize[0],m_System.m_ConfigData.m_anScreenSize[1]), 32,
			m_System.m_ConfigData.m_bFullScreen, false, true,
			this);

		if(m_System.m_pDevice == NULL) 
		{			
			return 2;
		}

		//작업디랙토리세팅
		m_System.m_pDevice->getFileSystem()->changeWorkingDirectoryTo(szResName);

		m_System.m_pDriver = m_System.m_pDevice->getVideoDriver();
		m_System.m_pSmgr   = m_System.m_pDevice->getSceneManager();				
		ggf::irr_util::DebugOutputFmt(NULL,"Render Engine Successed....\n");

		//사용자 추가 확장 씬노드 펙토리등록
		{
			irr::scene::jz3d::CJZ3DSceneNodeFactory *factory =
				new irr::scene::jz3d::CJZ3DSceneNodeFactory(m_System.m_pSmgr);
			m_System.m_pSmgr->registerSceneNodeFactory(factory);
			factory->drop();	
		}

		//물리 애니메이터 펙토리등록
		{
			
			m_pBulletPhysicsFactory = new irr::scene::CBulletAnimatorManager(m_System.m_pSmgr);
		}


		//싸운드 시스템 세팅
		m_System.m_pSoundEngine = irrklang::createIrrKlangDevice();
		if(m_System.m_pSoundEngine)
			ggf::irr_util::DebugOutputFmt(NULL,"sound Engine Successed....\n");
		else
			return 3;
	}

	// i/o 디바이스세팅

	if(Get().IsNiMode())
	{
		if( Get().InitNiIO() == false)
			return 4; //io 디바이스에러
	}

	//gui 세팅
	ggf::irr_util::DebugOutputFmt(NULL,"Create GUI Object\n");
	{
		irr::gui::IGUIEnvironment *pEnv = m_System.m_pDevice->getGUIEnvironment();		

		//폰트로딩
		std::string strffn = m_System.m_ConfigData.m_strResFolder;
		strffn += "/texture/font_arial36/myfont.xml";
		
		m_pFont_Arial36 = pEnv->getFont(strffn.c_str());

		{// 폰트 바꾸기
			std::string strffn = m_System.m_ConfigData.m_strResFolder;
			strffn += "/texture/font_ms12/MainFont.xml";
			irr::gui::IGUISkin* skin = pEnv->getSkin();
			irr::gui::IGUIFont* font = pEnv->getFont(strffn.c_str());
			m_pFont_ms12 = font;

			skin->setColor(irr::gui::EGDC_3D_FACE,irr::video::SColor(255,255,0,0));

			skin->setFont(font,irr::gui::EGDF_DEFAULT);		//모든 출력에 다적용.
			
			
			//EGDF_BUTTON,
			//! Font for window title bars
			//EGDF_WINDOW,
			//! Font for menu items
			//EGDF_MENU,
			//! Font for tooltips
			//EGDF_TOOLTIP,
			//! this value is not used, it only specifies the amount of default fonts
			//! available.
			//EGDF_COUNT
		}


		irr::gui::IGUIElement *pRoot;
		{//디버깅창
			pRoot = pEnv->addStaticText(L"debug",irr::core::rect<irr::s32>(0,0,120,96),true,true,NULL,GUIID_DEBUG_TEXT);

			if(m_System.m_ConfigData.m_bDebugMode)
			{
				pRoot->setVisible(true);
			}
			else
			{
				pRoot->setVisible(false);
			}
		}

		{
		}


		//게임 플래이 종료 패널..
		{
			pRoot = pEnv->addTab(
				irr::core::rect<irr::s32>(0,0,m_System.m_pDriver->getScreenSize().Width,m_System.m_pDriver->getScreenSize().Height),
				0,
				GUIID_GAMEROOT
				);
			irr::gui::IGUIElement *pGuiElement;

			pGuiElement = pEnv->addButton(irr::core::rect<irr::s32>(500,300,700,350),pRoot,GUIID_BTN_FINISH,L"Finish & Save Record");
			pGuiElement = pEnv->addButton(irr::core::rect<irr::s32>(500,350,700,400),pRoot,GUIID_BTN_FINISH_NOTSAVE,L"Finish without Save Record");
		}		

		//로그인창
		{
			pRoot = pEnv->addTab(
				//irr::core::rect<irr::s32>(320,200,m_System.m_pDriver->getScreenSize().Width,m_System.m_pDriver->getScreenSize().Height),
				m_System.m_ConfigData.m_uiDBlock.GUIID_LOGIN.body[0],
				0,
				GUIID_LOGIN
				);
			
			pRoot->setVisible(false);
			
			irr::gui::IGUIListBox *pLb = pEnv->addListBox(
				//irr::core::rect<irr::s32>(0,0,128,64),
				m_System.m_ConfigData.m_uiDBlock.GUIID_LOGIN_USERLIST.body[0],
				pRoot,GUIID_LOGIN_USERLIST,true);

			std::map<std::string,CUserData>::iterator it;

			for(it = Get().m_Users.begin();it != Get().m_Users.end();it++)
			{
				pLb->addItem( bstr_t(it->first.c_str()));				
			}

			pEnv->addButton(
				//irr::core::rect<irr::s32>(0,128,64,128+64),
				m_System.m_ConfigData.m_uiDBlock.GUIID_LOGIN_OK.body[0],
				pRoot,GUIID_LOGIN_OK,bstr_t(m_System.m_ConfigData.m_uiDBlock.GUIID_LOGIN_OK.caption.c_str()));
			pEnv->addButton(
				//irr::core::rect<irr::s32>(0,128,64,128+64),
				m_System.m_ConfigData.m_uiDBlock.GUIID_LOGIN_CANCEL.body[0],
				pRoot,GUIID_LOGIN_CANCEL,bstr_t(m_System.m_ConfigData.m_uiDBlock.GUIID_LOGIN_CANCEL.caption.c_str()));
		}

		

		{//게임메뉴
			pRoot = pEnv->addTab(
				//irr::core::rect<irr::s32>(320,200,m_System.m_pDriver->getScreenSize().Width,m_System.m_pDriver->getScreenSize().Height),
				m_System.m_ConfigData.m_uiDBlock.GUIID_GAMEMENU.body[0],
				0,
				GUIID_GAMEMENU
				);
			pEnv->addButton(
				//irr::core::rect<irr::s32>(0,0,128,64),
				m_System.m_ConfigData.m_uiDBlock.GUIID_BTN_START.body[0],
				pRoot,GUIID_BTN_START,L"Single Play");
			pEnv->addButton(
				//irr::core::rect<irr::s32>(0,80,128,80+64),
				m_System.m_ConfigData.m_uiDBlock.GUIID_BTN_MULTI.body[0],
				pRoot,GUIID_BTN_MULTI,L"NetWork Play");
			pEnv->addButton(
				//irr::core::rect<irr::s32>(0,160,128,80*2 + 64),
				m_System.m_ConfigData.m_uiDBlock.GUIID_BTN_EXIT.body[0],
				pRoot,GUIID_BTN_EXIT,L"End");
			pRoot->setVisible(false);
		}

		{//스태이지선택
			pRoot = pEnv->addTab(
				//irr::core::rect<irr::s32>(16,-100,m_System.m_pDriver->getScreenSize().Width,m_System.m_pDriver->getScreenSize().Height),
				m_System.m_ConfigData.m_uiDBlock.GUIID_STAGE_SELECT.body[0],
				0,
				GUIID_STAGE_SELECT
				);				

			pEnv->addButton(
				//irr::core::rect<irr::s32>(148*3,200,128+148*3,264),
				m_System.m_ConfigData.m_uiDBlock.GUIID_BTN_STAGE_SELECT_BACK.body[0],
				pRoot,
				GUIID_BTN_STAGE_SELECT_BACK,L"Back");

			pEnv->addButton(
					//irr::core::rect<irr::s32>(300,200,128+148*2,264),
					m_System.m_ConfigData.m_uiDBlock.GUIID_BTN_GAMESTART.body[0],
					pRoot,
					GUIID_BTN_GAMESTART,L"Start");

			{				
				pEnv->addStaticText(
					//L"Select Vechicle",
					//irr::core::rect<irr::s32>(0,180,128,210),
					bstr_t(m_System.m_ConfigData.m_uiDBlock.GUIID_CMB_CAR_SEL.caption.c_str()),
					m_System.m_ConfigData.m_uiDBlock.GUIID_CMB_CAR_SEL.body[1],
					false,true,pRoot
					);
				irr::gui::IGUIComboBox *pCmb = pEnv->addComboBox(
					//irr::core::rect<irr::s32>(0,220,128,220+60),
					m_System.m_ConfigData.m_uiDBlock.GUIID_CMB_CAR_SEL.body[0],
					pRoot,GUIID_CMB_CAR_SEL);

				irr::u32 i;
				for(i=0;i<m_System.m_ConfigData.m_vtVehicleName.size();i++)
				{
					irr::core::stringw strw = m_System.m_ConfigData.m_vtVehicleName[i].c_str();
					pCmb->addItem(strw.c_str());
				}

				pEnv->addStaticText(
					//L"Select Stage",
					//irr::core::rect<irr::s32>(150,180,300,210),
					bstr_t(m_System.m_ConfigData.m_uiDBlock.GUIID_LIST_STAGE_SELECT.caption.c_str()),
					m_System.m_ConfigData.m_uiDBlock.GUIID_LIST_STAGE_SELECT.body[1],
					false,true,pRoot
					);

				irr::gui::IGUIListBox *pList = pEnv->addListBox(
					//irr::core::rect<irr::s32>(150,220,300,600),
					m_System.m_ConfigData.m_uiDBlock.GUIID_LIST_STAGE_SELECT.body[0],
					pRoot,GUIID_LIST_STAGE_SELECT,true);

				for(i=0;i<m_System.m_ConfigData.m_vtStageRes.size();i++)
				{
					irr::core::stringw strw = m_System.m_ConfigData.m_vtStageRes[i].m_strName.c_str();
					pList->addItem(strw.c_str());
				}

				
			}

			pRoot->setVisible(false);
		}
		
		
	}	

	{//프레임초기화

		//CGameFrame::Create();
		CEsenFrame *pGameFrame = new CGameFrame("frame_game");		
		CEsenFrame *pGameMenu = new CMainMenuFrame("frame_menu");

		//m_System.m_pFrameMng = new CFrameMng(m_System.m_pDevice,60); //화면 갱신률을 초당 60hz로설정
		m_System.m_pFrameMng = new CFrameMng(m_System.m_pDevice,0); //화면 갱신률 무재한
		
		m_System.m_pFrameMng->AddFrame(pGameFrame);
		m_System.m_pFrameMng->AddFrame(pGameMenu);
		
		if(m_System.m_ConfigData.m_bDebugMode)
		{
			CBycleApp::Get().m_strUserID = "gbox";
			CBycleApp::Get().m_nStage = m_System.m_ConfigData.m_viDbg[7]; //디버그용 스태이지
			m_System.m_pFrameMng->SetActiveFrame("frame_game");						
		}
		else
		{
			m_System.m_pFrameMng->SetActiveFrame("frame_menu");
		}

	}
	m_System.m_bIsReady = true;

	return 0;//성공
}

void CBycleApp::MainLoop()
{

	HWND hwnd = (HWND)(m_System.m_pDriver->getExposedVideoData().D3D9.HWnd);

	if(m_System.m_ConfigData.m_viDbg[0]) //Joystick Mode
	{		
		if(FAILED(InitDirectInput(hwnd)))			
		{
			MessageBox( NULL, TEXT("Error Initializing DirectInput"), 
				TEXT("DirectInput Sample"), MB_ICONERROR | MB_OK );
		}
	}

	irr::u32 uLastTick = m_System.m_pDevice->getTimer()->getTime();

	while(m_System.m_pDevice->run())
	{
		irr::f32 fDelta = (m_System.m_pDevice->getTimer()->getTime() - uLastTick)/1000.f;
		uLastTick = m_System.m_pDevice->getTimer()->getTime();

		{//gui처리
			irr::gui::IGUIElement *pRoot = m_System.m_pDevice->getGUIEnvironment()->getRootGUIElement();
		}

		//joystick입력
		if(m_System.m_ConfigData.m_viDbg[0]) //Joystick Mode
		{
			UpdateInputState(hwnd);
		}
		//객체처리		
		
		m_System.m_pFrameMng->Apply((irr::u32)(fDelta*1000.f));//m_System.m_clock.getTimeMicroseconds());
		m_System.m_clock.reset();
		

		//render
		m_System.m_pDriver->beginScene(true, true, irr::video::SColor(0,100,100,100));		
		m_System.m_pFrameMng->Render();	

		{
			irr::core::position2di mpos = m_System.m_pDevice->getCursorControl()->getPosition();

			char szBuf[256];
			sprintf_s(szBuf,"%d, %d",mpos.X,mpos.Y);
			irr::core::stringw strw = szBuf;

			m_pFont_ms12->draw(strw.c_str(),irr::core::rect<irr::s32>(0,0,100,100),irr::video::SColor(255,255,255,0));
		}
		
		m_System.m_pDriver->endScene();


#ifdef USE_NIIO
		if((m_System.m_pDevice->getTimer()->getRealTime() - m_uIODelayTick) > 100 && IsNiMode() )
		{
			try
			{	
				m_fAI_1 = m_spNiDaqReader->ReadSingleSample();
				m_fAI_2 = m_spNiDaqReader2->ReadSingleSample();
				m_fAI_3 = m_spNiDaqReader3->ReadSingleSample();
				if(m_System.m_ConfigData.m_bDebugMode)
				{
					
					ggf::irr_util::DebugOutputFmt(NULL,"%.3f, %.3f \n",
						m_fAI_1,
						m_spNiDaqReader2->ReadSingleSample()
						);			
				}

				m_uIODelayTick = m_System.m_pDevice->getTimer()->getRealTime();
				
			}
			catch(CException* exception)
			{
				exception->ReportError();
				exception->Delete();
			}
		}
#endif

	}
	
	//CloseConnection();

	if(m_System.m_ConfigData.m_viDbg[0]) //Joystick Mode
	{
		FreeDirectInput();
	}
}

bool CBycleApp::InitNiIO()
{// I/O 디바이스 세팅

#ifdef USE_NIIO
	//CNiDAQmxTask myTask;
	try
	{
		m_spNiDaqTask = std::auto_ptr<CNiDAQmxTask>(new CNiDAQmxTask());
		m_spNiDaqTask->AIChannels.CreateVoltageChannel("Dev1/ai1",_T(""), 
			static_cast<DAQmxAITerminalConfiguration>(-1),
			-10.0, 10.0, DAQmxAIVoltageUnitsVolts);			

		m_spNiDaqReader = 
			std::auto_ptr<CNiDAQmxAnalogSingleChannelReader>(new CNiDAQmxAnalogSingleChannelReader(m_spNiDaqTask->Stream));

		m_spNiDaqTask2 = std::auto_ptr<CNiDAQmxTask>(new CNiDAQmxTask());
		m_spNiDaqTask2->AIChannels.CreateVoltageChannel("Dev1/ai2",_T(""), 
			static_cast<DAQmxAITerminalConfiguration>(-1),
			-10.0, 10.0, DAQmxAIVoltageUnitsVolts);			

		m_spNiDaqReader2 = 
			std::auto_ptr<CNiDAQmxAnalogSingleChannelReader>(new CNiDAQmxAnalogSingleChannelReader(m_spNiDaqTask2->Stream));

		m_spNiDaqTask3 = std::auto_ptr<CNiDAQmxTask>(new CNiDAQmxTask());
		m_spNiDaqTask3->AIChannels.CreateVoltageChannel("Dev1/ai3",_T(""), 
			static_cast<DAQmxAITerminalConfiguration>(-1),
			-10.0, 10.0, DAQmxAIVoltageUnitsVolts);			

		m_spNiDaqReader3 = 
			std::auto_ptr<CNiDAQmxAnalogSingleChannelReader>(new CNiDAQmxAnalogSingleChannelReader(m_spNiDaqTask3->Stream));


	}
	catch(CException* exception)
	{
		exception->ReportError();
		exception->Delete();

		return false;
	}
#endif

	return true;
}



bool CBycleApp::OnEvent(const irr::SEvent &ev)
{		
	irr::IrrlichtDevice *pDevice = m_System.m_pDevice;

	if(m_System.m_bIsReady) 
	{
		CEsenFrame *pFrame = m_System.m_pFrameMng->GetActiveFrame();

		//이벤트 전달
		if(pFrame)
		{
			pFrame->OnEvent(ev);
		}

	}

	return false;
}
///////////////////////////////////////////////////////////

