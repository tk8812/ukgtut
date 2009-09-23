//#include "StdAfx.h"
#include "bycle.h"
#include "GameFrame.h"

using namespace std;
using namespace irr;

#include "irr_utill.h"

//CGameFrame* CGameFrame::ms_pGameFrame;

void recursivePhysicsReg(irr::scene::ISceneManager *pSmgr,irr::s32 worldID,irr::scene::ISceneNode *pStartNode)
{
	if(pStartNode == 0)
	{
		pStartNode = pSmgr->getRootSceneNode();
	}

	irr::core::list<irr::scene::ISceneNode *>::ConstIterator it = pStartNode->getChildren().begin();

	for(;it != pStartNode->getChildren().end();it++)
	{
		recursivePhysicsReg(pSmgr,worldID,*it);


		switch( (*it)->getType())
		{
		case irr::scene::jz3d::CCollusionMngNode::TypeID:
			{				
			}
			break;
		case irr::scene::jz3d::CBulletPhysicsObjectNode::TypeID:
			{
				irr::scene::CBulletAnimatorManager *ani_factory =
					static_cast<irr::scene::CBulletAnimatorManager *>(ggf::util::findAnimatorFactory(pSmgr,
					irr::scene::ESNAT_BULLET_OBJECT));

				irr::scene::jz3d::CBulletPhysicsObjectNode *pNode = irr::scene::jz3d::CBulletPhysicsObjectNode::upcast((*it));	
				pNode->register2BulletPhysicsWorld(ani_factory->getBulletWorldByID(worldID)->getWorld());
			}
			break;
		case irr::scene::ESNT_MESH:
		case irr::scene::jz3d::CTiledPlaneNode::TypeID:			
			break;
		}

		
	}
}




CGameFrame::CGameFrame(void)
:CEsenFrame(),
m_pmsGroundSelecter(0),
m_pmsRoadSelecter(0),
m_pmsWallSelecter(0)
{
	m_pmsGroundSelecter = 0;
	m_pmsRoadSelecter = 0;
	m_pmsWallSelecter = 0;
	//m_pTsFinishLine = 0;	
}

CGameFrame::CGameFrame(LPCSTR strName)
: CEsenFrame(strName),
m_pmsGroundSelecter(0),
m_pmsRoadSelecter(0),
m_pmsWallSelecter(0)
//m_pTsFinishLine(0)
{
				
}

CGameFrame::~CGameFrame(void)
{	
	if(m_bEnable)
		Release();
}

void CGameFrame::Init()
{
	m_bEnable = true;	
	m_bCollGround = false;
	m_bPauseKey = false;
	m_bResume = false;
	
	SetPhase(PH_READY,NULL);
}


void CGameFrame::Release()
{
	
	if(m_pmsGroundSelecter)
	{
		m_pmsGroundSelecter->drop();
		m_pmsRoadSelecter->drop();
		if(m_pmsWallSelecter)
			m_pmsWallSelecter->drop();
		//if(m_pTsFinishLine)
		//	m_pTsFinishLine->drop();	
	}	

	irr::scene::ISceneManager *pSmgr = CBycleApp::Get().m_System.m_pSmgr;
	pSmgr->clear();	

	m_bEnable = false;

	
}

void CGameFrame::Apply(irr::u32 timeMs)
{
	if(!m_bEnable) return;
	
	irr::scene::ISceneManager *pSmgr = CBycleApp::Get().m_System.m_pSmgr;
	
	switch(m_nPhase)
	{
	case PH_READY:
		switch(m_nStep)
		{
		case 0:
			LoadRes();	

			// 배경음악 시작			
			CBycleApp::Get().playBgm();			
			CBycleApp::Get().hideAllMenu();
			SetPhase(PH_PLAY,NULL);			
			break;		
		}
		break;	
	case PH_PLAY:		
		{
			switch(m_nStep)
			{
			case 0:
				if(m_bResume == true)
				{
					m_nStep = 10;
				}
				else
				{
					m_ThisBycle.SetStatus(CObj_ThisBycle::ST_READY);
					m_nStep = 5;					
				}
				break;
			case 5:
				if(m_ThisBycle.GetStatus() == CObj_ThisBycle::ST_RACE) // 출발 신호떨어지면
				{
					m_nStep = 10;					
				}
				break;
			case 10:
				if(m_bPauseKey)
				{
					SetPhase(PH_PAUSE,NULL);
				}				
				break;
			}			

			//물리 스탭
			m_pBulletPhysicsFactory->OnUpdate(timeMs*1000);
			//플래이어 컨트롤
			m_ThisBycle.Apply(timeMs);

			if(m_ThisBycle.IsMissionComplete())
			{
				m_ThisBycle.CompleteRace();
				SetPhase(PH_END,NULL);
			}
			
		}
		break;
	case PH_PAUSE:
		switch(m_nStep)
		{
		case 0:
			if(m_bPauseKey == false) //플릭킹방지
			{
				//배경음악 정지
				CBycleApp::Get().stopBgm();
				//게임 종료버튼활성화
				CBycleApp::Get().activateGameResultTab();
				m_nStep++;
			}
			break;
		case 1:
			if(m_bPauseKey == true) //플릭킹방지
			{
				m_nStep++;
				
			}
			break;
		case 2:
			if(m_bPauseKey == false) //플릭킹방지
			{
				CBycleApp::Get().DeactivateGameResultTab();
				SetPhase(PH_PLAY,NULL);
				m_bResume = true;
			}
			break;
		}		
		break;
	case PH_END:
		switch(m_nStep)
		{
		case 0:
			if(m_bPauseKey == false) //플릭킹방지
			{
				//배경음악 정지
				CBycleApp::Get().stopBgm();
				//게임 종료버튼활성화
				CBycleApp::Get().activateGameResultTab();
				m_nStep++;
			}
			break;
		case 1:
			if(m_bPauseKey == true) //플릭킹방지
			{
				m_nStep++;
				
			}
			break;
		case 2:
			//if(m_bPauseKey == false) //플릭킹방지
			//{
			//	CBycleApp::Get().DeactivateGameResultTab();
			//	SetPhase(PH_PLAY,NULL);
			//	m_bResume = true;
			//}
			break;
		}
		break;
	case PH_TESTPLAY:		
		break;
	}

			
	
}
void CGameFrame::Render()
{
	if(!m_bEnable) return;

	CBycleApp::Get().m_System.m_pSmgr->drawAll();
	CBycleApp::Get().m_System.m_pDevice->getGUIEnvironment()->drawAll();			
	

	switch(m_nPhase)
	{
	case PH_READY:		
		break;
	case PH_PLAY:
		m_ThisBycle.Render();	
		{
			irr::video::IVideoDriver *pVideo = CBycleApp::Get().m_System.m_pDevice->getVideoDriver();
			//pVideo->D

			char szBuf[256];

			float speedkmh = m_ThisBycle.getSpeedKmh();

			sprintf_s(szBuf,"%d",(int)speedkmh);

			irr::core::stringw strw = szBuf;

			CBycleApp::Get().m_pFont_Arial36->draw(strw.c_str(),
				irr::core::rect<irr::s32>(20,500,100,600),
				irr::video::SColor(255,255,255,0));		

			//경과시간표시
			irr::u32 tick = (irr::u32)m_ThisBycle.getRecord();
			sprintf_s(szBuf,"%02d:%02d",tick/60,tick%60);

			strw = szBuf;

			CBycleApp::Get().m_pFont_Arial36->draw(strw.c_str(),
				irr::core::rect<irr::s32>(400-42,0,500,36),
				irr::video::SColor(255,255,255,0));	

		}
		break;
	case PH_END:
		{
			irr::video::IVideoDriver *pVideo = CBycleApp::Get().m_System.m_pDevice->getVideoDriver();			

			char szBuf[256];

			float speedkmh = m_ThisBycle.getSpeedKmh();

			sprintf_s(szBuf,"result: %f",m_ThisBycle.getRecord());

			irr::core::stringw strw = szBuf;

			CBycleApp::Get().m_pFont_Arial36->draw(strw.c_str(),
				irr::core::rect<irr::s32>(200,100,200+100,100+60),
				irr::video::SColor(255,255,255,0));	
			
		}
		break;
	case PH_TESTPLAY:
		break;
	}
}



void CGameFrame::Octree2Bullet(irr::scene::ISceneNode *pRoadRoot)
{
	//irr::scene::ISceneNode *pRoadRoot = CBycleApp::Get().m_System.m_pSmgr->getSceneNodeFromName(stageRes.m_strRoad.c_str());

	irr::scene::ISceneManager *pSmgr = CBycleApp::Get().m_System.m_pDevice->getSceneManager();
	irr::io::IFileSystem* ifs = CBycleApp::Get().m_System.m_pDevice->getFileSystem();
	irr::core::list<irr::scene::ISceneNode*>::ConstIterator it = pRoadRoot->getChildren().begin(); 

	for (; it != pRoadRoot->getChildren().end(); ++it) 
	{ 
		if((*it)->getType() == irr::scene::ESNT_OCT_TREE ||
			(*it)->getType() == irr::scene::ESNT_MESH
			)
		{
			irr::scene::ISceneNode *pNode = (*it);
			{ 
				irr::io::IAttributes* attribs = ifs->createEmptyAttributes(); 
				if (attribs) 
				{ 
					pNode->serializeAttributes(attribs); 

					// get the mesh name out 
					irr::core::stringc name = attribs->getAttributeAsString("Mesh"); 

					attribs->drop(); 

					// get the animated mesh for the object 
					irr::scene::IAnimatedMesh* mesh = pSmgr->getMesh(name.c_str()); 

					irr::scene::CBulletObjectAnimatorGeometry geom;
					irr::scene::CBulletObjectAnimatorParams physicsParams;
					//pNode->setMaterialFlag(irr::video::EMF_WIREFRAME,true);		

					// add level static mesh
					geom.type = scene::CBPAGT_CONCAVE_MESH;					
					geom.mesh.irrMesh = NULL;//pNode->getMesh();////pMesh->getMesh(0);					
					geom.meshFile = name;
					physicsParams.mass = 0.0f;

					scene::CBulletObjectAnimator* levelAnim = 
						m_pBulletPhysicsFactory->createBulletObjectAnimator(
						pSmgr,
						pNode,
						m_pWorldAnimator->getID(),
						&geom,
						&physicsParams
						);
					pNode->addAnimator(levelAnim);
					levelAnim->drop();

				} 
			}						
		}		
		else if((*it)->getType() == irr::scene::ESNT_TERRAIN)
		{
		}		
	} 	
}

void CGameFrame::LoadRes()
{		
	
	CConfigData *pConfig = &(CBycleApp::Get().m_System.m_ConfigData);
	CStage_Res stageRes = pConfig->m_vtStageRes[CBycleApp::Get().m_nStage];
	
	CBycleApp::Get().m_System.m_pSmgr->clear();

	//씬로딩
	{
		std::string strPath = CBycleApp::GetPtr()->m_System.m_ConfigData.m_strResFolder;
		strPath += "/res/";
		std::string strOldPath;


		strOldPath = CBycleApp::GetPtr()->m_System.m_pDevice->getFileSystem()->getWorkingDirectory();

		CBycleApp::GetPtr()->m_System.m_pDevice->getFileSystem()->changeWorkingDirectoryTo(strPath.c_str());
		CBycleApp::Get().m_System.m_pSmgr->loadScene(stageRes.m_strFile.c_str());	//"../res/stage02.xml");

		CBycleApp::GetPtr()->m_System.m_pDevice->getFileSystem()->changeWorkingDirectoryTo(strOldPath.c_str());
	}

	//자전거 로드가 있는지 검사하기
	/*if(!CBycleApp::Get().m_System.m_pSmgr->getSceneNodeFromName(stageRes.m_isoBycle.m_strMainObjNodeName.c_str()) )
	{
		std::string strPath = CBycleApp::GetPtr()->m_System.m_ConfigData.m_strResFolder;
		strPath += "/res/";
		std::string strOldPath;
		strOldPath = CBycleApp::GetPtr()->m_System.m_pDevice->getFileSystem()->getWorkingDirectory();

		CBycleApp::GetPtr()->m_System.m_pDevice->getFileSystem()->changeWorkingDirectoryTo(strPath.c_str());

		CBycleApp::Get().m_System.m_pSmgr->loadScene(stageRes.m_isoBycle.m_strFile.c_str());

		CBycleApp::GetPtr()->m_System.m_pDevice->getFileSystem()->changeWorkingDirectoryTo(strOldPath.c_str());
	}
	else
	{
		ggf::irr_util::DebugOutputFmt(NULL,"%s node alraedy exist \n",stageRes.m_isoBycle.m_strMainObjNodeName.c_str());
	}*/

	m_pSpawnNode = CBycleApp::Get().m_System.m_pSmgr->getSceneNodeFromName(stageRes.m_strSpawnPoint.c_str());
	if(!m_pSpawnNode)
	{
		ggf::irr_util::DebugOutputFmt(NULL,"not found node %s\n",stageRes.m_strSpawnPoint.c_str());
	}

	irr::scene::ISceneManager *pSmgr = CBycleApp::Get().m_System.m_pSmgr;
	irr::video::IVideoDriver *pVideo = CBycleApp::Get().m_System.m_pDevice->getVideoDriver();

	irr::scene::ICameraSceneNode *pCam;				

	pCam = (irr::scene::ICameraSceneNode *)CBycleApp::Get().m_System.m_pSmgr->getSceneNodeFromName(
		pConfig->m_strCamNode.c_str()
		);
	CBycleApp::Get().m_System.m_pSmgr->setActiveCamera(pCam);

	//샐랙터 처리 관련 초기화
	{
		//전체 씬노드 모두 샐랙터생성
		ggf::irr_util::ISceneManager_assignTriangleSelectors(
			CBycleApp::Get().m_System.m_pSmgr,
			CBycleApp::Get().m_System.m_pDevice->getFileSystem()
			);

		//지상대물용 통합삼각선택자만들기
		m_pmsGroundSelecter = CBycleApp::Get().m_System.m_pSmgr->createMetaTriangleSelector(); 
		m_pmsRoadSelecter = CBycleApp::Get().m_System.m_pSmgr->createMetaTriangleSelector(); 
		m_pmsWallSelecter = CBycleApp::Get().m_System.m_pSmgr->createMetaTriangleSelector(); 		

		irr::scene::ISceneNode *pNode;

		pNode = CBycleApp::Get().m_System.m_pSmgr->getSceneNodeFromName(stageRes.m_strTerrain.c_str());

		if(pNode)
		{
			ggf::irr_util::ISceneNode_gatherTriangleSelectors(			
				pNode, 
				m_pmsGroundSelecter
				); 
		}

		pNode = CBycleApp::Get().m_System.m_pSmgr->getSceneNodeFromName(stageRes.m_strRoad.c_str());

		if(pNode)
		{
			ggf::irr_util::ISceneNode_gatherTriangleSelectors(			
				pNode, 
				m_pmsRoadSelecter
				); 	
		}

		pNode = CBycleApp::Get().m_System.m_pSmgr->getSceneNodeFromName(stageRes.m_strWall.c_str());
		if(pNode)
		{
			ggf::irr_util::ISceneNode_gatherTriangleSelectors(			
				pNode, 
				m_pmsWallSelecter
				); 	
		}

		irr::scene::ISceneNode *pTrigerGroup = pSmgr->getSceneNodeFromName("triger");

		if(pTrigerGroup)
		{
			pNode = CBycleApp::Get().m_System.m_pSmgr->getSceneNodeFromName(
					stageRes.m_strFinishLine.c_str()		
			);

			if(pNode)
			{
				m_pColMFinishLine = (irr::scene::jz3d::CCollusionMngNode *)pNode;				
			}
			else
			{
				ggf::irr_util::DebugOutputFmt(NULL,"cant found %s node for finish line \n",stageRes.m_strFinishLine.c_str()	);				
			}
		}
		else
		{
			ggf::irr_util::DebugOutputFmt(NULL,"cant found triger group\n");
		}
	}

	
	//물리충돌정보 만들기
	{
		// Initialize bullet physics world(discret dynamics)		
		{
			m_pBulletPhysicsFactory = new scene::CBulletAnimatorManager(pSmgr);
			m_pWorldNode = pSmgr->addEmptySceneNode();
			m_pWorldNode->setName("WorldNode");
			//  기본중력 -9.8f
			irr::scene::CBulletWorldAnimatorParams worldParams;	
			worldParams.bSoft = false;

			//루트애니매이터
			//irr::scene::CBulletWorldAnimator* bulletWorldAnimator = 
			m_pWorldAnimator =
				m_pBulletPhysicsFactory->createBulletWorldAnimator(
				pSmgr,
				m_pWorldNode,
				&worldParams
				);
			//중력은 기본으로 y 축으로 -9.8
			//m_pWorldAnimator->setGravity(irr::core::vector3df(0,-9.8,0));
			m_pWorldNode->addAnimator(m_pWorldAnimator);	

			//드랍을 잊지말자
			m_pBulletPhysicsFactory->drop();
			m_pWorldAnimator->drop();
		}		
		
		//길
		{
			irr::scene::ISceneNode *pRoadRoot = CBycleApp::Get().m_System.m_pSmgr->getSceneNodeFromName(stageRes.m_strRoad.c_str());

			if(pRoadRoot)	
			{
				Octree2Bullet(pRoadRoot);

				//물리 씬노드 등록해주기
				irr::core::list<irr::scene::ISceneNode *>::ConstIterator it = pRoadRoot->getChildren().begin();
				
				recursivePhysicsReg(pSmgr,m_pWorldAnimator->getID(),(*it));
			}
		}
		//벽옥트리
		{
			irr::scene::ISceneNode *pRoot = CBycleApp::Get().m_System.m_pSmgr->getSceneNodeFromName(stageRes.m_strWall.c_str());
			if(pRoot)	
				Octree2Bullet(pRoot);
		}
		//그라운드 옥트리
		{
			irr::scene::ISceneNode *pRoot = CBycleApp::Get().m_System.m_pSmgr->getSceneNodeFromName(stageRes.m_strTerrain.c_str());
			if(pRoot)	
				Octree2Bullet(pRoot);
		}			
	}	
	
	m_ThisBycle.Init(this);	
}


bool CGameFrame::OnEvent(irr::SEvent ev)
{
	if(ev.EventType == EET_KEY_INPUT_EVENT)
	{
		if(ev.KeyInput.PressedDown == true)
		{		
			switch(ev.KeyInput.Key)
			{
			case irr::KEY_KEY_W:				
				//pCam->setPosition(pCam->getPosition() + vfront);
				//m_ThisBycle.ChangeSpeed(0.1f);
				g_Buttons[2] = 1;
				break;
			case irr::KEY_KEY_S:
				g_Buttons[3] = 1;
				//m_ThisBycle.ChangeSpeed(-0.1f);
				break;
			case irr::KEY_SPACE:
				g_Buttons[4] = 1;
				//m_ThisBycle.ChangeSpeed(-0.1f);
				break;
			case irr::KEY_KEY_A:
				m_ThisBycle.IncreaseTurn(-0.1f);
				break;
			case irr::KEY_KEY_D:
				m_ThisBycle.IncreaseTurn(0.1f);
				break;
			case irr::KEY_F1:
				//m_ThisBycle.SetCamYaw(90.f);
				//SetPhase(PH_TESTPLAY,NULL);			
				break;
			case irr::KEY_F2:
				//m_ThisBycle.SetCamYaw(-90.f);
				//SetPhase(PH_TESTPLAY,NULL);			
				break;
			case irr::KEY_F3:
				//m_ThisBycle.SetCamYaw(180.f);
				//SetPhase(PH_TESTPLAY,NULL);			
				break;
			case irr::KEY_F4:
				//m_ThisBycle.SetCamYaw(0.f);
				//SetPhase(PH_PAUSE,NULL);			
				break;
			case irr::KEY_F5:
				{
					CBycleApp::Get().m_System.m_pSmgr->getSceneNodeFromName("DebugWheelRoot")->setVisible(
						!CBycleApp::Get().m_System.m_pSmgr->getSceneNodeFromName(
						"DebugWheelRoot")->isVisible()	
						);
					//DebugWheelRoot
			/*	m_ThisBycle.SetCamYaw(0.f);
				SetPhase(PH_PAUSE,NULL);			*/
				break;
				}
			case irr::KEY_F6:
				{
					m_ThisBycle.HideVehicle();
				}
				break;

			case irr::KEY_ESCAPE:
				m_bPauseKey = true;
				//SetPhase(PH_TESTPLAY,NULL);
				break;			
			}
		}
		else
		{
			switch(ev.KeyInput.Key)
			{
			case irr::KEY_ESCAPE:
				m_bPauseKey = false;				
				break;					
			case irr::KEY_KEY_W:								
				g_Buttons[2] = 0;
				break;
			case irr::KEY_KEY_S:
				g_Buttons[3] = 0;				
				break;
			case irr::KEY_SPACE:
				g_Buttons[4] = 0;
				//m_ThisBycle.ChangeSpeed(-0.1f);
				break;
			}
		}
	}
	else if(ev.EventType == EET_MOUSE_INPUT_EVENT)
	{
		if(ev.MouseInput.Event == EMIE_LMOUSE_PRESSED_DOWN)
		{
			/*
			irr::core::position2di mouse_pos = CBycleApp::m_System.m_pDevice->getCursorControl()->getPosition();

			irr::core::line3df Ray;		
			irr::core::triangle3df triangle;

			Ray = CBycleApp::m_System.m_pSmgr->getSceneCollisionManager()->getRayFromScreenCoordinates(mouse_pos);

			if (CBycleApp::m_System.m_pSmgr->getSceneCollisionManager()->getCollisionPoint(
				Ray, 
				m_pmsTestObject, Ray.end, triangle))
			{
//				m_obj_백수인간.SetStatus(CObj_서성이는사람::ST_STAND,NULL);
				//ggf::irr_util::DebugOutputFmt(NULL,"colii\n");
			}
			*/

		}
	}
	else if(ev.EventType == irr::EET_GUI_EVENT)
	{
		irr::gui::IGUIEnvironment *pEnv = CBycleApp::Get().m_System.m_pDevice->getGUIEnvironment();	
		int hID = ev.GUIEvent.Caller->getID();
		switch(ev.GUIEvent.EventType)
		{
		case irr::gui::EGET_BUTTON_CLICKED:	
			{		

				switch(hID)
				{
				case GUIID_BTN_FINISH:
					{
						//irr::gui::IGUIElement *pguiEm =  pEnv->getRootGUIElement()->getElementFromId(GUIID_BTN_FINISH,true);
						//pguiEm->setVisible(false);
						CBycleApp::Get().hideAllMenu();

						CBycleApp::Get().SaveRecord(m_ThisBycle.getRecord());

						CBycleApp::Get().m_System.m_pFrameMng->SetActiveFrame("frame_menu");
						
					}
					break;
				case GUIID_BTN_FINISH_NOTSAVE:
					{
						CBycleApp::Get().hideAllMenu();
						CBycleApp::Get().m_System.m_pFrameMng->SetActiveFrame("frame_menu");
					}
				}
			}
		}

	}
	
		
	return true;
}

void CGameFrame::DecodePacket(irr::s32 nCode,void *pBuf)
{
}

bool CGameFrame::GetCollisionHeight(float x, float z, float &y)
{
	irr::core::triangle3df triangle;
	irr::core::line3df Ray = irr::core::line3df(irr::core::vector3df(x,500,z),irr::core::vector3df(x,-500,z));
	irr::core::vector3df vColl;

	if(CBycleApp::Get().m_System.m_pSmgr->getSceneCollisionManager()->getCollisionPoint(
		Ray,m_pmsRoadSelecter,
		vColl,
		m_triIntersecGround
		))
	{
		y = vColl.Y;
		m_bCollGround = true;		
		m_nCollType = COLT_ROAD;
		

	}
	else if(CBycleApp::Get().m_System.m_pSmgr->getSceneCollisionManager()->getCollisionPoint(
		Ray,m_pmsGroundSelecter,
		vColl,
		m_triIntersecGround
		))
	{
		
		y = vColl.Y;
		m_bCollGround = true;	
		m_nCollType = COLT_GROUND;
	}	
	else
	{	
		m_bCollGround = false;
	}

	return m_bCollGround;
}

//vtRayPosDiff 충돌광선 모 오브잭트 위치에서 변위차
//
bool CGameFrame::GetCollisionWall(irr::scene::ISceneNode *pNode,irr::core::vector3df vtRayPosDiff,irr::f32 fRayLength, irr::f32 fYaw, irr::f32 &fDist, irr::core::triangle3df *pTri)
{
	irr::f32 fPosX = pNode->getPosition().X;	
	irr::f32 fPosY = pNode->getPosition().Y;
	irr::f32 fPosZ = pNode->getPosition().Z;

	irr::core::vector3df vtStart(fPosX,fPosY,fPosZ);
	irr::core::vector3df vtDir(0,0,1);

	//vtStart += vtRayPosDiff;

	vtDir.rotateXZBy(0-fYaw,irr::core::vector3df(0,0,0));

	//irr::core::triangle3df triangle;
	irr::core::line3df Ray = irr::core::line3df(vtStart,vtStart+(vtDir*fRayLength)	);
	
	bool bColl = false;	

	if(CBycleApp::Get().m_System.m_pSmgr->getSceneCollisionManager()->getCollisionPoint(
		Ray,m_pmsWallSelecter,
		Ray.end,
		m_triIntersecWall
		))
	{
		fDist = Ray.getLength();
		//y = vColl.Y;
		//m_bCollGround = true;		
		//m_nCollType = COLT_ROAD;	
		bColl = true;

		if(pTri != NULL)
		{
			*pTri = m_triIntersecWall;
		}

	}	
	else
	{	
		//m_bCollGround = false;
	}

	m_RayfrontColl = Ray;

	return bColl;

}


