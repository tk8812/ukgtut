//#include "StdAfx.h"
#include "bycle.h"
#include "GameFrame.h"
#include "VehicleAnimator.h"

using namespace std;
using namespace irr;

#include "irr_utill.h"

#include "BulletCollision/NarrowPhaseCollision/btGjkPairDetector.h"
#include "BulletCollision/NarrowPhaseCollision/btPointCollector.h"
#include "BulletCollision/NarrowPhaseCollision/btVoronoiSimplexSolver.h"
#include "BulletCollision/NarrowPhaseCollision/btConvexPenetrationDepthSolver.h"
#include "BulletCollision/NarrowPhaseCollision/btGjkEpaPenetrationDepthSolver.h"




////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////자전거 오브잭트//////////////////////////////
////////////////////////////////////////////////////////////////////////////////////


void CObj_ThisBycle::Init(CEsenFrame *pFrame)
{	

	{//디버그 카메라세팅

		irr::scene::ICameraSceneNode *pCam;				
		pCam = CBycleApp::Get().m_System.m_pSmgr->addCameraSceneNodeFPS(0,100,50); 	
		pCam->setName("debug_cam");							
	}

	m_fMaxSpeed = 120.f;


	CConfigData *pConfig = &(CBycleApp::Get().m_System.m_ConfigData);
	CStage_Res stageRes = pConfig->m_vtStageRes[CBycleApp::Get().m_nStage];

	m_pFrame = pFrame;
	CGameFrame *pGameFrame = (CGameFrame *)m_pFrame;

	SetStatus(CObj_ThisBycle::ST_SLEEP,NULL);
	m_bEnable = true;

	m_pRootSceneNode = CBycleApp::Get().m_System.m_pSmgr->getRootSceneNode();	
	
	{
		m_pStartSignal[0] = CBycleApp::Get().m_System.m_pSmgr->getSceneNodeFromName("start_signal_1");	
		m_pStartSignal[1] = CBycleApp::Get().m_System.m_pSmgr->getSceneNodeFromName("start_signal_2");	

		if(m_pStartSignal[0] && m_pStartSignal[1])
		{
			{

				irr::scene::ISceneNode *pNode = m_pStartSignal[0];

				irr::core::list<scene::ISceneNode*>::ConstIterator begin = pNode->getChildren().begin(); 
				irr::core::list<scene::ISceneNode*>::ConstIterator end   = pNode->getChildren().end(); 

				for (/**/; begin != end; ++begin) 
					(*begin)->setVisible(false);
			}

			{

				irr::scene::ISceneNode *pNode = m_pStartSignal[1];

				irr::core::list<scene::ISceneNode*>::ConstIterator begin = pNode->getChildren().begin(); 
				irr::core::list<scene::ISceneNode*>::ConstIterator end   = pNode->getChildren().end(); 

				for (/**/; begin != end; ++begin) 
					(*begin)->setVisible(false);
			}
		}
	}
	

	////변수초기화
	//{
	//	m_v3Heading = irr::core::vector3df(0,0,1.0f);
	//	m_fHeading  = m_v3Heading.getHorizontalAngle().Y;	

	//	m_fVelocity = 0.0f;
	//	m_u32StartTick = 0;
	//	m_fTurn = 0.0f;
	//	m_fCamYaw = 0;
	//}

	//자동차 초기위치설정은 물리세팅함수 내에서한다.
	InitVechilePhysics();	

	//자동차 모델보여줄것인지?
	if(CBycleApp::GetPtr()->IsHideVehicleMode())
	{
		HideVehicle();
	}

	

	//카메라세팅 활동자등록
	//카메라 씬노드에 카메라를 움직이는 활동자와 차량 활동자를 동시에 등록 시켜놓는다.
	{
		m_pCamNode = CBycleApp::GetPtr()->m_System.m_pSmgr->addCameraSceneNode();
		m_pCamLookatNode = CBycleApp::GetPtr()->m_System.m_pSmgr->addEmptySceneNode();
		//m_pCamLookatNode->setParent(m_pVehicleNode);

		//카메라 제어용 애니메이터등록
		CBycleApp::GetPtr()->m_System.m_pSmgr->setActiveCamera(m_pCamNode);
		irr::scene::CThirdPersonAnimator *pAnim;// 
		pAnim = new irr::scene::CThirdPersonAnimator();

		pAnim->setTargetNode(m_pCamLookatNode);
		pAnim->setBaseDirection(irr::core::vector3df(0,0,-1));
		pAnim->setDistance( CBycleApp::GetPtr()->m_System.m_ConfigData.m_fCamDist);

		m_pCamNode->addAnimator(pAnim);
		m_pCamAnimator = pAnim;
		pAnim->drop();

		//차량 제어용 애니메이터등록
		m_pCamNode->addAnimator(this);
	}
}

void CObj_ThisBycle::InitVechilePhysics()
{
	CConfigData *pConfig = &(CBycleApp::Get().m_System.m_ConfigData);
	CStage_Res stageRes = pConfig->m_vtStageRes[CBycleApp::Get().m_nStage];
	

	//m_pFrame = pFrame;
	CGameFrame *pGameFrame = (CGameFrame *)m_pFrame;

	irr::gui::IGUIEnvironment *pEnv = CBycleApp::Get().m_System.m_pDevice->getGUIEnvironment();	
	irr::gui::IGUIComboBox *pCmbCarSel = (irr::gui::IGUIComboBox *)pEnv->getRootGUIElement()->getElementFromId(GUIID_CMB_CAR_SEL,true);

	std::string strPath = CBycleApp::GetPtr()->m_System.m_ConfigData.m_strResFolder;
	std::string strOldPath;
	strOldPath = CBycleApp::GetPtr()->m_System.m_pDevice->getFileSystem()->getWorkingDirectory();
	CBycleApp::GetPtr()->m_System.m_pDevice->getFileSystem()->changeWorkingDirectoryTo(strPath.c_str());

	std::string strVehicleDir = pConfig->m_vtVehiclePhysicsFile[pCmbCarSel->getSelected()];
	DB_VehiclePhysics dbVph;
	//물리 스크립트 로딩
		

	dbVph.LoadXml((strVehicleDir + std::string("physics.xml")).c_str());	

	m_dbVehiclePhysics = dbVph;
	

	//자동차모델로드
	{		
		irr::scene::ISceneNode *pRoot = CBycleApp::Get().m_System.m_pSmgr->addEmptySceneNode();
		pRoot->setName("Vehicle_Root");

		irr::scene::IMesh *pMesh = CBycleApp::Get().m_System.m_pSmgr->getMesh(
			//"../res/Vehicle/BMW_X5/body.x"
			(strVehicleDir + dbVph.m_Body.ModelFile).c_str()
			);

		irr::scene::IMeshSceneNode *pNode = CBycleApp::Get().m_System.m_pSmgr->addMeshSceneNode(pMesh,pRoot);

		pNode->setMaterialFlag(irr::video::EMF_LIGHTING,false);	
		pNode->setRotation(irr::core::vector3df(0,180.f,0));
		pNode->setScale(m_dbVehiclePhysics.m_ModelScale);		
		//pNode->setScale(irr::core::vector3df(.6f,.6f,.6f));

		m_pVehicleNode = pRoot;
	}

	//바퀴모델로드
	{
		irr::scene::ISceneNode *pRoot = CBycleApp::Get().m_System.m_pSmgr->addEmptySceneNode();
		pRoot->setName("Wheel_FR");

		irr::scene::IMesh *pMesh = CBycleApp::Get().m_System.m_pSmgr->getMesh(			
			(strVehicleDir + m_dbVehiclePhysics.m_FR.ModelFile).c_str()
			);

		irr::scene::IMeshSceneNode *pNode = CBycleApp::Get().m_System.m_pSmgr->addMeshSceneNode(pMesh,pRoot);

		pNode->setMaterialFlag(irr::video::EMF_LIGHTING,false);	
		pNode->setRotation(irr::core::vector3df(0,180.f,0));
		pNode->setScale(m_dbVehiclePhysics.m_ModelScale);
		//pNode->setScale(irr::core::vector3df(.06f,.06f,.06f));


		m_pWheelFR = pRoot;
	}

	//바퀴모델로드
	{
		irr::scene::ISceneNode *pRoot = CBycleApp::Get().m_System.m_pSmgr->addEmptySceneNode();
		pRoot->setName("Wheel_FL");

		irr::scene::IMesh *pMesh = CBycleApp::Get().m_System.m_pSmgr->getMesh(			
			(strVehicleDir + m_dbVehiclePhysics.m_FL.ModelFile).c_str()
			);

		irr::scene::IMeshSceneNode *pNode = CBycleApp::Get().m_System.m_pSmgr->addMeshSceneNode(pMesh,pRoot);

		pNode->setMaterialFlag(irr::video::EMF_LIGHTING,false);	
		pNode->setRotation(irr::core::vector3df(0,180.f,0));
		pNode->setScale(m_dbVehiclePhysics.m_ModelScale);
		//pNode->setScale(irr::core::vector3df(.06f,.06f,.06f));


		m_pWheelFL = pRoot;
	}

	//바퀴모델로드
	{
		irr::scene::ISceneNode *pRoot = CBycleApp::Get().m_System.m_pSmgr->addEmptySceneNode();
		pRoot->setName("Wheel_RR");

		irr::scene::IMesh *pMesh = CBycleApp::Get().m_System.m_pSmgr->getMesh(			
			(strVehicleDir + m_dbVehiclePhysics.m_RR.ModelFile).c_str()
			);

		irr::scene::IMeshSceneNode *pNode = CBycleApp::Get().m_System.m_pSmgr->addMeshSceneNode(pMesh,pRoot);

		pNode->setMaterialFlag(irr::video::EMF_LIGHTING,false);	
		pNode->setRotation(irr::core::vector3df(0,180.f,0));
		pNode->setScale(m_dbVehiclePhysics.m_ModelScale);
		//pNode->setScale(irr::core::vector3df(.06f,.06f,.06f));

		m_pWheelRR = pRoot;
	}

	//바퀴모델로드
	{
		irr::scene::ISceneNode *pRoot = CBycleApp::Get().m_System.m_pSmgr->addEmptySceneNode();
		pRoot->setName("Wheel_RL");

		irr::scene::IMesh *pMesh = CBycleApp::Get().m_System.m_pSmgr->getMesh(			
			(strVehicleDir + m_dbVehiclePhysics.m_RL.ModelFile).c_str()
			);

		irr::scene::IMeshSceneNode *pNode = CBycleApp::Get().m_System.m_pSmgr->addMeshSceneNode(pMesh,pRoot);

		pNode->setMaterialFlag(irr::video::EMF_LIGHTING,false);	
		pNode->setRotation(irr::core::vector3df(0,180.f,0));
		pNode->setScale(m_dbVehiclePhysics.m_ModelScale);
		//pNode->setScale(irr::core::vector3df(0.06f,.06f,.06f));

		m_pWheelRL = pRoot;
	}
	//m_pVehicleNode = CBycleApp::Get().m_System.m_pSmgr->getSceneNodeFromName("Vehicle_Root");

	//작업 폴더 원래대로
	CBycleApp::GetPtr()->m_System.m_pDevice->getFileSystem()->changeWorkingDirectoryTo(strOldPath.c_str());

	//블릿물리 자동차 만들기
	{
		irr::scene::ISceneManager *pSmgr = CBycleApp::Get().m_System.m_pSmgr;
		irr::video::IVideoDriver *pVideo = CBycleApp::Get().m_System.m_pDriver;


		m_EngineForce = 0;// = 0.f;
		m_BreakingForce =0;// = 0.f;
		m_VehicleSteering=0;// = 0.f;

		//4륜자동차만들기(후륜방식)
		{
			irr::core::vector3df carsize = m_dbVehiclePhysics.m_Body.BodySize;//irr::core::vector3df(6.f,1.0f,8.f);
			//btCollisionShape* chassisShape = new btBoxShape(btVector3(carsize.X,carsize.Y,carsize.Z));			
			m_spChassisShape = std::tr1::shared_ptr<btCollisionShape>(new btBoxShape(btVector3(carsize.X,carsize.Y,carsize.Z)));

			//btCompoundShape* compound = new btCompoundShape();
			m_spCompound = std::tr1::shared_ptr<btCompoundShape>(new btCompoundShape());

			if(CBycleApp::Get().IsDebugMode())
			{
				irr::scene::ISceneNode *pNode = pSmgr->addCubeSceneNode(1);

				pNode->setPosition(irr::core::vector3df(0,0,0));
				//pNode->setMaterialTexture(0,pVideo->getTexture("../res/t351sml.jpg"));
				pNode->setMaterialFlag(irr::video::EMF_LIGHTING,false);
				pNode->setMaterialFlag(irr::video::EMF_WIREFRAME,true);
				pNode->setScale(carsize);
				//pNode->setParent(m_pVehicleNode);	
				pNode->setName("debugVehicleBody");
			}



			btTransform localTrans;                                                                                                                                                                                                                                                                              
			localTrans.setIdentity();
			//localTrans effectively shifts the center of mass with respect to the chassis
			localTrans.setOrigin(btVector3(0,.0f,0));
			m_spCompound->addChildShape(localTrans,m_spChassisShape.get());


			//자동차 시작위치지정
			{
				//시작포인트
				irr::scene::ISceneNode *pSpawnPointNode = CBycleApp::Get().m_System.m_pSmgr->getSceneNodeFromName(
					stageRes.m_strSpawnPoint.c_str()
					);
				
				btTransform tr;
				tr.setIdentity();				
				tr.setOrigin(btVector3(
					pSpawnPointNode->getPosition().X,
					pSpawnPointNode->getPosition().Y,
					pSpawnPointNode->getPosition().Z
					)
					);
				btQuaternion qt;
				qt.setEuler(pSpawnPointNode->getRotation().Y*irr::core::DEGTORAD,
					pSpawnPointNode->getRotation().X*irr::core::DEGTORAD,
					pSpawnPointNode->getRotation().Z*irr::core::DEGTORAD);					
				tr.setRotation(qt); //각도지정

				btScalar mass = m_dbVehiclePhysics.m_Body.mass; // 800;


				btVector3 localInertia(0,0,0);	
				m_spChassisShape->calculateLocalInertia(mass,localInertia);				
				m_spCarMotionState = std::tr1::shared_ptr<btDefaultMotionState>(new btDefaultMotionState(tr));

				btRigidBody::btRigidBodyConstructionInfo cInfo(mass,m_spCarMotionState.get(),m_spCompound.get(),localInertia);				
				m_carChassis = std::tr1::shared_ptr<btRigidBody>(new btRigidBody(cInfo));

				pGameFrame->m_pWorldAnimator->getWorld()->addRigidBody(m_carChassis.get());			


			}

			/// create vehicle
			{
				btDynamicsWorld *m_dynamicsWorld = pGameFrame->m_pWorldAnimator->getWorld();

				//m_vehicleRayCaster = new btDefaultVehicleRaycaster(m_dynamicsWorld);
				//m_vehicle = new btRaycastVehicle(m_tuning,m_carChassis,m_vehicleRayCaster);
				m_vehicleRayCaster = std::tr1::shared_ptr<btDefaultVehicleRaycaster>(new btDefaultVehicleRaycaster(m_dynamicsWorld));
				m_vehicle = std::tr1::shared_ptr<btRaycastVehicle>(new btRaycastVehicle(m_tuning,m_carChassis.get(),m_vehicleRayCaster.get()));

				///never deactivate the vehicle
				m_carChassis->setActivationState(DISABLE_DEACTIVATION);

				m_dynamicsWorld->addVehicle(m_vehicle.get());


				//float connectionHeight = .8f;
				//const int CUBE_HALF_EXTENTS = 6;
				float	wheelRadius = 1.5f;
				float	wheelWidth = .3f;
				//float	wheelFriction = 1000;//1e30f; //바퀴의 마찰계수
				//float	suspensionStiffness = 20.f; //스프링의 탄성 높을 수록 통통튄다.
				//float	suspensionDamping = 2.3f; // 저항값
				//float	suspensionCompression = 4.4f;
				//float	rollInfluence = .5f;//1.0f;

				int rightIndex = 0;
				int upIndex = 1;
				int forwardIndex = 2;
				btVector3 wheelDirectionCS0(0,-1,0);
				btVector3 wheelAxleCS(-1,0,0);

				//btScalar suspensionRestLength(.6f); //서스펜센의 탄성범위 클수록 차체가 출렁거린다. 스프링의길이?

				//choose coordinate system
				m_vehicle->setCoordinateSystem(rightIndex,upIndex,forwardIndex);

				//바퀴붙이기
				bool isFrontWheel=true; //전륜				
				//btVector3 connectionPointCS0((carsize.X/2)-(.3f*wheelWidth),connectionHeight,carsize.Z/2-wheelRadius);				
				//m_vehicle->addWheel(connectionPointCS0,wheelDirectionCS0,wheelAxleCS,suspensionRestLength,wheelRadius,m_tuning,isFrontWheel);

				irr::scene::ISceneNode *pDebugWheelRootNode = pSmgr->addEmptySceneNode();
				pDebugWheelRootNode->setName("DebugWheelRoot");

				{
					DB_VehiclePhysics::Wheel wheel = m_dbVehiclePhysics.m_FR;
					DB_VehiclePhysics::Suspension sus = m_dbVehiclePhysics.m_vtSuspension[wheel.suspensionIndex];
					btVector3 connectionPointCS0(wheel.pos.X,wheel.pos.Y,wheel.pos.Z);
					btWheelInfo& wheelinfo = m_vehicle->addWheel(connectionPointCS0,wheelDirectionCS0,wheelAxleCS,
						sus.RestLength,						
						wheel.Radius,						
						m_tuning,
						isFrontWheel);

					wheelinfo.m_suspensionStiffness = sus.Stiffness;//suspensionStiffness;
					wheelinfo.m_wheelsDampingRelaxation = sus.Damping;// suspensionDamping;
					wheelinfo.m_wheelsDampingCompression = sus.Compression;// suspensionCompression;
					wheelinfo.m_frictionSlip = wheel.friction;// wheelFriction;
					wheelinfo.m_rollInfluence = wheel.rollInfluence;// rollInfluence;
					

					{
						irr::scene::IAnimatedMesh *pAniMesh =  pSmgr->addArrowMesh("arrow_wheel",
							irr::video::SColor(0,255,0,0),
							irr::video::SColor(0,0,255,0),
							8,8,wheel.width,.2f,wheel.Radius,0.3f			
							);
						{
							irr::scene::ISceneNode *pDummy;

							if(!CBycleApp::Get().IsDebugMode())
							{
								//pDummy->setVisible(false);								
							}
							else
							{
								pDummy = pSmgr->addEmptySceneNode();
								irr::scene::IAnimatedMeshSceneNode *pNode = pSmgr->addAnimatedMeshSceneNode(pAniMesh,pDummy);
								pNode->setMaterialFlag(irr::video::EMF_LIGHTING,false);		
								pNode->setPosition(irr::core::vector3df(0,0,0));
								pNode->setRotation(
									irr::core::vector3df(0.f,0,90.f)
									);
								pDummy->setName("arrow_node0");
								pDebugWheelRootNode->addChild(pDummy);
							}						
						}

					}

					{
						DB_VehiclePhysics::Wheel wheel = m_dbVehiclePhysics.m_FL;
						DB_VehiclePhysics::Suspension sus = m_dbVehiclePhysics.m_vtSuspension[wheel.suspensionIndex];
						btVector3 connectionPointCS0(wheel.pos.X,wheel.pos.Y,wheel.pos.Z);
						btWheelInfo& wheelinfo = m_vehicle->addWheel(connectionPointCS0,wheelDirectionCS0,wheelAxleCS,
							sus.RestLength,						
							wheel.Radius,						
							m_tuning,
							isFrontWheel);

						wheelinfo.m_suspensionStiffness = sus.Stiffness;//suspensionStiffness;
						wheelinfo.m_wheelsDampingRelaxation = sus.Damping;// suspensionDamping;
						wheelinfo.m_wheelsDampingCompression = sus.Compression;// suspensionCompression;
						wheelinfo.m_frictionSlip = wheel.friction;// wheelFriction;
						wheelinfo.m_rollInfluence = wheel.rollInfluence;// rollInfluence;


						{
							irr::scene::IAnimatedMesh *pAniMesh =  pSmgr->addArrowMesh("arrow_wheel1",
								irr::video::SColor(0,255,0,0),
								irr::video::SColor(0,0,255,0),
								8,8,wheel.width,.2f,wheel.Radius,0.3f			
								);

							irr::scene::ISceneNode *pDummy;

							if(!CBycleApp::Get().IsDebugMode())
							{
								//pDummy->setVisible(false);
							}
							else
							{
								pDummy = pSmgr->addEmptySceneNode();
								irr::scene::IAnimatedMeshSceneNode *pNode = pSmgr->addAnimatedMeshSceneNode(pAniMesh,pDummy);
								pNode->setMaterialFlag(irr::video::EMF_LIGHTING,false);		
								pNode->setPosition(irr::core::vector3df(0,0,0));
								pNode->setRotation(
									irr::core::vector3df(0.f,0,-90.f)
									);
								pDummy->setName("arrow_node1");
								pDebugWheelRootNode->addChild(pDummy);
							}
						}

					}

					isFrontWheel = false;//후륜

					{
						DB_VehiclePhysics::Wheel wheel = m_dbVehiclePhysics.m_RL;
						DB_VehiclePhysics::Suspension sus = m_dbVehiclePhysics.m_vtSuspension[wheel.suspensionIndex];
						btVector3 connectionPointCS0(wheel.pos.X,wheel.pos.Y,wheel.pos.Z);
						btWheelInfo& wheelinfo = m_vehicle->addWheel(connectionPointCS0,wheelDirectionCS0,wheelAxleCS,
							sus.RestLength,						
							wheel.Radius,						
							m_tuning,
							isFrontWheel);

						wheelinfo.m_suspensionStiffness = sus.Stiffness;//suspensionStiffness;
						wheelinfo.m_wheelsDampingRelaxation = sus.Damping;// suspensionDamping;
						wheelinfo.m_wheelsDampingCompression = sus.Compression;// suspensionCompression;
						wheelinfo.m_frictionSlip = wheel.friction;// wheelFriction;
						wheelinfo.m_rollInfluence = wheel.rollInfluence;// rollInfluence;

						{
							irr::scene::IAnimatedMesh *pAniMesh =  pSmgr->addArrowMesh("arrow_wheel2",
								irr::video::SColor(0,255,0,0),
								irr::video::SColor(0,0,255,0),
								8,8,wheel.width,.2f,wheel.Radius,0.3f			
								);

							irr::scene::ISceneNode *pDummy;
							if(!CBycleApp::Get().IsDebugMode())
							{
								//pDummy->setVisible(false);
							}
							else
							{
								pDummy = pSmgr->addEmptySceneNode();
								irr::scene::IAnimatedMeshSceneNode *pNode = pSmgr->addAnimatedMeshSceneNode(pAniMesh,pDummy);
								pNode->setMaterialFlag(irr::video::EMF_LIGHTING,false);		
								pNode->setPosition(irr::core::vector3df(0,0,0));
								pNode->setRotation(
									irr::core::vector3df(0.f,0,-90.f)
									);
								pDummy->setName("arrow_node2");
								pDebugWheelRootNode->addChild(pDummy);
							}
						}
					}

					{
						DB_VehiclePhysics::Wheel wheel = m_dbVehiclePhysics.m_RR;
						DB_VehiclePhysics::Suspension sus = m_dbVehiclePhysics.m_vtSuspension[wheel.suspensionIndex];
						btVector3 connectionPointCS0(wheel.pos.X,wheel.pos.Y,wheel.pos.Z);
						btWheelInfo& wheelinfo = m_vehicle->addWheel(connectionPointCS0,wheelDirectionCS0,wheelAxleCS,
							sus.RestLength,						
							wheel.Radius,						
							m_tuning,
							isFrontWheel);

						wheelinfo.m_suspensionStiffness = sus.Stiffness;//suspensionStiffness;
						wheelinfo.m_wheelsDampingRelaxation = sus.Damping;// suspensionDamping;
						wheelinfo.m_wheelsDampingCompression = sus.Compression;// suspensionCompression;
						wheelinfo.m_frictionSlip = wheel.friction;// wheelFriction;
						wheelinfo.m_rollInfluence = wheel.rollInfluence;// rollInfluence;

						{
							irr::scene::IAnimatedMesh *pAniMesh =  pSmgr->addArrowMesh("arrow_wheel3",
								irr::video::SColor(0,255,0,0),
								irr::video::SColor(0,0,255,0),
								8,8,wheel.width,.2f,wheel.Radius,0.3f			
								);

							irr::scene::ISceneNode *pDummy;

							if(!CBycleApp::Get().IsDebugMode())
							{
								//pDummy->setVisible(false);								
							}
							else
							{
								pDummy = pSmgr->addEmptySceneNode();
								irr::scene::IAnimatedMeshSceneNode *pNode = pSmgr->addAnimatedMeshSceneNode(pAniMesh,pDummy);
								pNode->setMaterialFlag(irr::video::EMF_LIGHTING,false);		
								pNode->setPosition(irr::core::vector3df(0,0,0));
								pNode->setRotation(
									irr::core::vector3df(0.f,0,90.f)
									);
								pDummy->setName("arrow_node3");
								pDebugWheelRootNode->addChild(pDummy);
							}
						}				
					}			
				}
			}/// create vehicle end
		}//4륜자동차만들기(후륜방식) end
	}
}


void CObj_ThisBycle::Apply(irr::u32 timeMS)
{	
	if(!m_bEnable) return;

	

	CConfigData *pConfig = &(CBycleApp::Get().m_System.m_ConfigData);
	CGameFrame *pFrame = (CGameFrame *)m_pFrame;
	irr::ITimer *pTimer;
	pTimer = CBycleApp::Get().m_System.m_pDevice->getTimer();	
	resetDeltaTick(pTimer->getRealTime()); //타임델타값 갱신
	
	irr::scene::ISceneManager *pSmgr = CBycleApp::Get().m_System.m_pSmgr;
	irr::u32 uDeltaTick = timeMS;

	switch(m_nStatus)
	{
	case ST_SLEEP:
		//SetStatus(ST_READY);
		break;
	case ST_READY:
		{			
			switch(m_nWork)
			{
			case 0:
				m_nCountDown = 3;
				m_u32AccTick = 0;
				m_nWork = 10;				
				break;
			case 10:
				{
					//irr::scene::ISceneNode *pNode;
					m_u32AccTick += uDeltaTick;
					if( m_u32AccTick > 1000)
					{				
						m_u32AccTick = 0;
						m_nCountDown--;

						if(m_pStartSignal[0])
						{

							irr::scene::ISceneNode *pNode = m_pStartSignal[0];

							irr::core::list<scene::ISceneNode*>::ConstIterator begin = pNode->getChildren().begin(); 
							irr::core::list<scene::ISceneNode*>::ConstIterator end   = pNode->getChildren().end(); 

							for (/**/; begin != end; ++begin) 
								(*begin)->setVisible(false);

							switch(m_nCountDown)
							{
							case 2:
								pNode = CBycleApp::Get().m_System.m_pSmgr->getSceneNodeFromName("06_lamp_r",m_pStartSignal[0]);	
								pNode->setVisible(true);
								break;
							case 1:
								pNode = CBycleApp::Get().m_System.m_pSmgr->getSceneNodeFromName("06_lamp_o",m_pStartSignal[0]);	
								pNode->setVisible(true);
								break;
							case 0:
								pNode = CBycleApp::Get().m_System.m_pSmgr->getSceneNodeFromName("06_lamp_g",m_pStartSignal[0]);	
								pNode->setVisible(true);
								break;						
							}

							ggf::irr_util::DebugOutputFmt(NULL,"Count %d\n",m_nCountDown);
							//resetDeltaTick(uTick);
						}
						else
						{
							m_nCountDown = -1;
							//m_u32StartTick = pTimer->getRealTime();
							//SetStatus(ST_RACE);
							//;
						}
					}
					if(m_nCountDown <= 0)
					{
						m_u32StartTick = pTimer->getRealTime();
						m_u32EndTick = pTimer->getRealTime();
						SetStatus(ST_RACE);
					}
				}
				break;
			}

		}
		break;
	case ST_RACE:
		{
			//속도에 따른 효과음주기
			if( !CBycleApp::Get().IsFXOffMode() && !CBycleApp::Get().IsBGMOffMode()) 
			{
				float speed_km = m_vehicle->getCurrentSpeedKmHour();
				
				if(speed_km > 0.1f)
				{
					
					CBycleApp::Get().m_pFxEngineSound[0]->setIsPaused(false);
				}
				else
				{
					CBycleApp::Get().m_pFxEngineSound[0]->setIsPaused(true);
				}
			}


			//if(CBycleApp::Get().m_System.m_ConfigData.m_viDbg[0])
			{
				if(g_Aixes_X < -64)
				{
					m_VehicleSteering = (((float)g_Aixes_X / 1000.f)* m_dbVehiclePhysics.m_SteeringLimit);					
				}
				else if(g_Aixes_X > 64)
				{
					m_VehicleSteering = (((float)g_Aixes_X / 1000.f)* m_dbVehiclePhysics.m_SteeringLimit);					
				}
				else
				{
					//m_fTurn = 0.f;
				}

				if(g_Buttons[2])
				{
					ChangeSpeed(0.1f);
				}
				else if(g_Buttons[4])
				{
					ChangeSpeed(-0.1f);
				}
				else
				{
					//ChangeSpeed(-0.01f);
					ChangeSpeed(0.f);
				}

				if(g_Buttons[3])
				{
					DoBreake();
					//ChangeSpeed(-0.1f);
				}			

			}


			{
				//차량움직이기 후륜구동방식
				int wheelIndex = 2;
				
				float speed_km = m_vehicle->getCurrentSpeedKmHour();
				//ggf::irr_util::DebugOutputFmt(NULL,"speed %f km \n ",speed_km);

				if(speed_km < m_dbVehiclePhysics.m_MaxSpeed)//m_fMaxSpeed) //최고속도제한
				{
					m_vehicle->applyEngineForce(m_EngineForce,wheelIndex);
					m_vehicle->setBrake(m_BreakingForce,wheelIndex);
					wheelIndex = 3;
					m_vehicle->applyEngineForce(m_EngineForce,wheelIndex);
					m_vehicle->setBrake(m_BreakingForce,wheelIndex);
				}
				else //속도제한
				{
					m_vehicle->applyEngineForce(0,wheelIndex);	
					m_vehicle->setBrake(m_BreakingForce,wheelIndex);
					wheelIndex = 3;
					m_vehicle->applyEngineForce(0,wheelIndex);
					m_vehicle->setBrake(m_BreakingForce,wheelIndex);
				}	

				wheelIndex = 0;
				m_vehicle->setSteeringValue(m_VehicleSteering,wheelIndex);
				wheelIndex = 1;
				m_vehicle->setSteeringValue(m_VehicleSteering,wheelIndex);
			}

			
			{
				irr::gui::IGUIEnvironment *pEnv = CBycleApp::Get().m_System.m_pDevice->getGUIEnvironment();	
				irr::gui::IGUIElement *pRoot =  pEnv->getRootGUIElement();
				irr::gui::IGUIStaticText *pText = (irr::gui::IGUIStaticText *)pRoot->getElementFromId(GUIID_DEBUG_TEXT,true);				
				
			}

			m_u32EndTick = pTimer->getRealTime();
		}
		break;
	case ST_PAUSE:
		break;	
	case ST_RESULT:
		switch(m_nWork)
		{
		case 0:
			{	
				//주행시간구하기.
				m_u32EndTick = pTimer->getRealTime();
				pFrame->SetPhase(CGameFrame::PH_END,NULL); //프래익웍 종료 전환
				m_nWork = 10;
			}			
			break;
		case 10:
			break;
		}
		break;
	}				
}

void CObj_ThisBycle::Render()
{

	if(CBycleApp::Get().IsDebugMode())
	{
		irr::video::IVideoDriver *pVideo = CBycleApp::Get().m_System.m_pDriver;

		{			
			irr::core::matrix4 mat;//단위행렬로초기화
			pVideo->setTransform(irr::video::ETS_WORLD, mat); //변환초기화

			irr::video::SMaterial m;
			m.Lighting = false;  //라이트를꺼야 색이 제데로나온다.
			m.ZBuffer = false;

			pVideo->setMaterial(m); 

			irr::core::vector3df Wheel[4];
			
			int i;
			for(i=0;i<4;i++)
			{
				Wheel[i] = irr::core::vector3df(
					m_vehicle->getWheelTransformWS(i).getOrigin().getX(),
					m_vehicle->getWheelTransformWS(i).getOrigin().getY(),
					m_vehicle->getWheelTransformWS(i).getOrigin().getZ()
					);

			}

			for(i=0;i<4;i+=2)
			{
				pVideo->draw3DLine(
					Wheel[i],Wheel[i+1],			
				irr::video::SColor(0,255,0,0));				
			}


			//카메라노드관련
			{
				irr::core::vector3df v = ggf::irr_util::GetWorldPosition(m_pCamLookatNode);
				pVideo->draw3DBox(irr::core::aabbox3df(v-irr::core::vector3df(1,1,1),v+irr::core::vector3df(1,1,1)),
					irr::video::SColor(0,255,255,0)
					);
			}
		}
	}
}

//! 현재위치구하기
irr::core::vector3df CObj_ThisBycle::getPosition()
{
	btVector3 p = m_carChassis->getCenterOfMassPosition();

	return irr::core::vector3df(p.getX(),p.getY(),p.getZ());
}

//! 임무완수여부 판단
bool CObj_ThisBycle::IsMissionComplete()
{
	CGameFrame *pFrame = (CGameFrame *)m_pFrame;
	{//미션목표완료여부검사

		btRigidBody *rigidBody = m_carChassis.get();		
		
		irr::core::vector3df vDir(0,0,1);
		irr::core::quaternion qtRot(rigidBody->getOrientation().getX(),
			rigidBody->getOrientation().getY(),
			rigidBody->getOrientation().getZ(),
			rigidBody->getOrientation().getW());

		vDir = qtRot * vDir;

		btCollisionShape *shape1 = pFrame->m_pColMFinishLine->getCollisionShape(); //피니쉬라인
		btTransform shape_trans1;
		shape_trans1.setFromOpenGLMatrix(pFrame->m_pColMFinishLine->getCollisionShapeTransform().pointer());

		
		btCollisionShape *shape2 = m_spChassisShape.get();
		btTransform shape_trans2;
		rigidBody->getMotionState()->getWorldTransform(shape_trans2);


		//최단거리구하기
		{	
			btVoronoiSimplexSolver sGjkSimplexSolver; //최단점 찾기
			btGjkEpaPenetrationDepthSolver epaSolver; //파고들어간정도 구하기
			btPointCollector gjkOutput; 

			btGjkPairDetector convexConvex(
				static_cast<const btConvexShape *>(shape1),
				static_cast<const btConvexShape *>(shape2),						
				&sGjkSimplexSolver,
				&epaSolver); 

			btGjkPairDetector::ClosestPointInput input; 
			input.m_transformA = shape_trans1;
			input.m_transformB = shape_trans2;

			convexConvex.getClosestPoints(input, gjkOutput, 0);

			ggf::irr_util::DebugOutputFmt(NULL,"%f \n",gjkOutput.m_distance);

			if(gjkOutput.m_distance <= 0)
				return true;

			//std::cout << gjkOutput.m_distance  << std::endl;					
		}
		//pFrame->m_pWorldAnimator->getWorld()->rayTestSingle
		/*
		irr::core::triangle3df triangle;
		irr::core::line3df Ray = irr::core::line3df(getPosition(),
			getPosition() + (vDir*10)
			);//진행방향으로 피킹레이만들기

		if(CBycleApp::Get().m_System.m_pSmgr->getSceneCollisionManager()->getCollisionPoint(
			Ray,pFrame->m_pTsFinishLine,
			Ray.end,
			triangle
			))
		{
 			return true;		
		}
		*/		
	}

	return false;
}

void CObj_ThisBycle::animateNode(irr::scene::ISceneNode *node, irr::u32 timeMs)
{
	CConfigData *pConfig = &(CBycleApp::Get().m_System.m_ConfigData);
	CGameFrame *pFrame = (CGameFrame *)m_pFrame;	
	
	irr::scene::ISceneManager *pSmgr = CBycleApp::Get().m_System.m_pSmgr;
	irr::u32 uDeltaTick = timeMs;

	//씬노드에 물리엔진의 변환적용시키기
	{
		//차체변환
		{	
			
			btRigidBody *rigidBody = m_carChassis.get();

			//if (physicsParams.mass != 0.0f && rigidBody && rigidBody->getMotionState())
			{
				// set pos
				btVector3 p = m_carChassis->getCenterOfMassPosition();
				//sceneNode->setPosition(core::vector3df(p.getX(), p.getY(), p.getZ()));

				// set rot
				core::vector3df rot;
				btQuaternion btq = rigidBody->getOrientation();
				QuaternionToEulerXYZ(btq, rot);
				rot *= (f32)BPU_180_PI;		
				
				m_pVehicleNode->setPosition(irr::core::vector3df(p.getX(), p.getY(), p.getZ()));//sceneNode->getPosition());
				m_pVehicleNode->setRotation(rot);//sceneNode->getRotation());

				m_pCamLookatNode->setPosition(
					irr::core::vector3df(p.getX(), p.getY(), p.getZ()) + pConfig->m_CamLookAtPosDiff
					);
				m_pCamLookatNode->setRotation(rot);

				//떨림 방지용
				m_pCamLookatNode->updateAbsolutePosition(); //월드변환갱신
				m_pVehicleNode->updateAbsolutePosition();
				
				//자동차의 회전값구해오기
				irr::core::quaternion qtTarget(btq.getX(),btq.getY(),btq.getZ(),btq.getW());
				m_qtCamRotation.slerp(m_qtCamRotation,qtTarget,
					pConfig->m_fCamfollowsensivity /*1.0f*/); //회전 따라가기 속도 설정 0 ~ 1.0

				((irr::scene::CThirdPersonAnimator *)m_pCamAnimator)->setRotation(m_qtCamRotation);

				if(CBycleApp::Get().IsDebugMode())
				{
					irr::scene::ISceneNode *pNode = pSmgr->getSceneNodeFromName("debugVehicleBody");
					pNode->setPosition(m_pVehicleNode->getPosition());
					pNode->setRotation(m_pVehicleNode->getRotation());
				}				
			} 
		}
		
		//바퀴 변환
		if(m_dbVehiclePhysics.m_Body.WheelNum == 4) //4륜 자동차일경우
		{
			irr::IrrlichtDevice *m_pDevice = CBycleApp::Get().m_System.m_pDevice;
			{

				irr::scene::ISceneManager *pSmgr = m_pDevice->getSceneManager();
				btTransform xform = m_vehicle->getWheelTransformWS(0);				

				btQuaternion btq = xform.getRotation();
				irr::core::vector3df rot;
				QuaternionToEulerXYZ(btq, rot);
				rot *= (f32)BPU_180_PI;		

				
				
				m_pWheelFR->setPosition(
					irr::core::vector3df(xform.getOrigin().getX(),
					xform.getOrigin().getY(),
					xform.getOrigin().getZ()
					)
					);
				m_pWheelFR->setRotation(rot);

				//디버그용
				irr::scene::ISceneNode *pNode = pSmgr->getSceneNodeFromName("arrow_node0");
				if(pNode)
				{
					pNode->setPosition( irr::core::vector3df(xform.getOrigin().getX(),
						xform.getOrigin().getY(),
						xform.getOrigin().getZ()
						)
						);
					pNode->setRotation(rot);
				}
			}		

			{
				irr::scene::ISceneManager *pSmgr = m_pDevice->getSceneManager();
				btTransform xform = m_vehicle->getWheelTransformWS(1);
				irr::scene::ISceneNode *pNode = pSmgr->getSceneNodeFromName("arrow_node1");			

				btQuaternion btq = xform.getRotation();
				irr::core::vector3df rot;
				QuaternionToEulerXYZ(btq, rot);
				rot *= (f32)BPU_180_PI;		
				

				m_pWheelFL->setPosition(
					irr::core::vector3df(xform.getOrigin().getX(),
					xform.getOrigin().getY(),
					xform.getOrigin().getZ()
					)
					);
				m_pWheelFL->setRotation(rot);

				if(pNode)
				{
					pNode->setPosition( irr::core::vector3df(xform.getOrigin().getX(),
						xform.getOrigin().getY(),
						xform.getOrigin().getZ()
						)
						);
					pNode->setRotation(rot);
				}
			}

			{
				irr::scene::ISceneManager *pSmgr = m_pDevice->getSceneManager();
				btTransform xform = m_vehicle->getWheelTransformWS(2);
				irr::scene::ISceneNode *pNode = pSmgr->getSceneNodeFromName("arrow_node2");				

				btQuaternion btq = xform.getRotation();
				irr::core::vector3df rot;
				QuaternionToEulerXYZ(btq, rot);
				rot *= (f32)BPU_180_PI;		
				

				m_pWheelRL->setPosition(
					irr::core::vector3df(xform.getOrigin().getX(),
					xform.getOrigin().getY(),
					xform.getOrigin().getZ()
					)
					);
				m_pWheelRL->setRotation(rot);

				if(pNode)
				{
					pNode->setPosition( irr::core::vector3df(xform.getOrigin().getX(),
						xform.getOrigin().getY(),
						xform.getOrigin().getZ()
						)
						);
					pNode->setRotation(rot);
				}
			}

			{
				irr::scene::ISceneManager *pSmgr = m_pDevice->getSceneManager();
				btTransform xform = m_vehicle->getWheelTransformWS(3);
				irr::scene::ISceneNode *pNode = pSmgr->getSceneNodeFromName("arrow_node3");

				btQuaternion btq = xform.getRotation();
				irr::core::vector3df rot;
				QuaternionToEulerXYZ(btq, rot);
				rot *= (f32)BPU_180_PI;		
				

				m_pWheelRR->setPosition(
					irr::core::vector3df(xform.getOrigin().getX(),
					xform.getOrigin().getY(),
					xform.getOrigin().getZ()
					)
					);
				m_pWheelRR->setRotation(rot);

				
				if(pNode)
				{
					pNode->setPosition( irr::core::vector3df(xform.getOrigin().getX(),
						xform.getOrigin().getY(),
						xform.getOrigin().getZ()
						)
						);
					pNode->setRotation(rot);
				}
			}
		}
		else if(m_dbVehiclePhysics.m_Body.WheelNum == 2) //2륜 바이크류
		{
		}
		else
		{
			ggf::irr_util::DebugOutputFmt(NULL,"unknown Wheel number!\n");
		}
	

	}	

}