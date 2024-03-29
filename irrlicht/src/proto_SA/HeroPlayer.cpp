#include "CSAApp.h"

#include "ChsBullet.h"
#include "HeroPlayer.h"

/*

해결해야할문제점
1. 캐릭터 컨트롤러의 터널링문제 - 사양이 낮은 곳에서 터널링이발생할수있다.
해결방법은 이동방향으로 광선 검사를 수행해서 근접물체가 있으면 움직임을 멈추게 해야한다.

if( 캐릭컨트롤러의 이동방향 앞으로 물체가 근접했는가? == false)
	m_pChracterAnimator->controlStep_Walker(WalkVelocity);

*/


CHeroPlayer::CHeroPlayer()
{
	m_strTypeName = "CHeroPlayer";	
	int i;
	for(i=0;i<irr::KEY_KEY_CODES_COUNT;i++)
	{
		m_Key[i] = false;
	}	

	SetStatus(FSM_READY);
}

CHeroPlayer::~CHeroPlayer(void)
{
			
}

bool CHeroPlayer::Init(irr::scene::ISceneNode *pNode)
{
	if(CPlayer::Init(pNode))
	{

		irr::scene::CBulletObjectAnimatorParams physicsParams;
		irr::scene::ISceneManager *pSmgr = CSAApp::GetPtr()->m_pSmgr;
		irr::IrrlichtDevice *pDevice = CSAApp::GetPtr()->m_pDevice;
		irr::scene::CBulletAnimatorManager* pBulletPhysicsFactory = CSAApp::GetPtr()->m_pBulletPhysicsFactory; 
		irr::scene::CBulletWorldAnimator* pWorldAnimator = CSAApp::GetPtr()->m_pWorldAnimator;


		m_pTrigerNode = pSmgr->getSceneNodeFromName("usr/triger");

		{
			irr::scene::ISceneNode *eyeNode = (irr::scene::jz3d::CCollusionMngNode *)pSmgr->getSceneNodeFromName("eye",m_pNode);
			irr::scene::ISceneNode *BodyNode = (irr::scene::jz3d::CCollusionMngNode *)pSmgr->getSceneNodeFromName("body",m_pNode);
			m_pCollMngNode = (irr::scene::jz3d::CCollusionMngNode *)pSmgr->getSceneNodeFromName("col_main",m_pNode);//캐릭터볼륨얻기(이동제어용)
			//m_pCollMngNode_Kick = (irr::scene::jz3d::CCollusionMngNode *)pSmgr->getSceneNodeFromName("col_kick",m_pNode); //발차기 충돌정보
			//m_pCollMngNode_Body = (irr::scene::jz3d::CCollusionMngNode *)pSmgr->getSceneNodeFromName("col_body",m_pNode); //몸통 충돌정보		

			physicsParams.mass = 70.f; //70kg
			physicsParams.gravity = core::vector3df(0, 0, 0);
			physicsParams.friction = 10.f; //마찰값	
			
			//현재 카메라얻기
			irr::scene::ISceneNode *pCamNode = pSmgr->getActiveCamera();
			
			irr::scene::CBulletFpsCamAnimator *pAnim = pBulletPhysicsFactory->createBulletFpsCamAnimator(
				pSmgr,
				pCamNode,
				pDevice->getCursorControl(),
				pWorldAnimator->getID(),
				m_pCollMngNode->getGeometryInfo(),
				&physicsParams
				);

			

			pCamNode->addAnimator(pAnim);	
			m_pChracterAnimator = pAnim;	
			pAnim->drop();

			//카메라에 캐릭터 노드 붙이기			
			m_pNode->setParent(pCamNode);
			BodyNode->setPosition(BodyNode->getPosition() - eyeNode->getPosition()); //눈을 기준으로 무기의 위치 잡아주기
			m_pChracterAnimator->setLocalPosition(eyeNode->getPosition());//눈의 위치 재조정
			
		}
		return true;
	}
	
	
	return false;
}



void CHeroPlayer::Update(irr::f32 fDelta)
{	
	irr::scene::ISceneManager *pSmgr = CSAApp::GetPtr()->m_pSmgr;
	irr::IrrlichtDevice *pDevice = CSAApp::GetPtr()->m_pDevice;
	irr::scene::CBulletAnimatorManager* pBulletPhysicsFactory = CSAApp::GetPtr()->m_pBulletPhysicsFactory; 
	irr::scene::CBulletWorldAnimator* pWorldAnimator = CSAApp::GetPtr()->m_pWorldAnimator;		

	m_pChracterAnimator->setMoveSpeed( 1.1f * 1800.f * fDelta ); // 시속 18키로 정도

	switch(GetStatus())
	{
	case CHeroPlayer::FSM_READY:
		{
			//시작위치 지정			
			m_pNode->setVisible(true);			
			irr::core::vector3df pos_Spwan = pSmgr->getSceneNodeFromName("start",m_pTrigerNode)->getAbsolutePosition();
			m_pChracterAnimator->setPosition(pos_Spwan);
			m_pChracterAnimator->zeroForces();
			//m_pChracterAnimator->setLocalPosition(irr::core::vector3df(0,5,0));
			//m_pChracterAnimator->setMoveSpeed(1.1f);			

			
			SetStatus(FSM_STAND);
		}
		break;
	case CHeroPlayer::FSM_STAND:
		{
			if(GetStep() == 0)
			{
				m_pNode->changeAction("stand");		
				IncStep();
			}
			else
			{
				if( m_Key[irr::KEY_LBUTTON] )
				{
					irr::scene::ISceneManager *pSmgr = pDevice->getSceneManager();
					irr::core::position2di mpos = pDevice->getCursorControl()->getPosition();				

					irr::core::line3df Ray;
					irr::core::vector3df vAim;

					Ray = pDevice->getSceneManager()->getSceneCollisionManager()->getRayFromScreenCoordinates(mpos);

					vAim = Ray.getVector();
					vAim.normalize(); 	

					ChsBullet *pBullet = new ChsBullet();

					ChsBullet::Params param_bullet;

					param_bullet.Energy = 100.f;
					param_bullet.mass = .1f;
					param_bullet.vAim = vAim;				
					param_bullet.vInitPos = getPosition() + (irr::core::vector3df(0,5,0) + vAim * 3); //총구 위치 적용된 발사시작위치

					pBullet->Init(&param_bullet); //총알 초기화

					m_mapBullets[pBullet->m_pAnim->getRigidBody()] = SP_IFSMObject(pBullet);

					m_Key[irr::KEY_LBUTTON] = false;					
				}
			}
		}		
		break;
	}


	//탄도 충돌 검출	
	{
		btDynamicsWorld *pWorld = pWorldAnimator->getWorld();
		int num = pWorld->getDispatcher()->getNumManifolds();		

		for(int i=0;i<num;i++)
		{
			btPersistentManifold *contactManifold = pWorld->getDispatcher()->getManifoldByIndexInternal(i);			
			
			if(contactManifold->getNumContacts() > 0)
			{				

				btCollisionObject* obA = static_cast<btCollisionObject*>(contactManifold->getBody0());
				btCollisionObject* obB = static_cast<btCollisionObject*>(contactManifold->getBody1());

				//충돌한 오브잭트 찾기
				ChsBullet *pBullet = NULL;
				if(m_mapBullets.count(obB) != 0)
				{
					pBullet = (ChsBullet *)m_mapBullets[obB].get();
				}			
				if(m_mapBullets.count(obA) != 0)
				{
					pBullet = (ChsBullet *)m_mapBullets[obA].get();				
				}			

				if(pBullet)
				{
					pBullet->Signal("hit",NULL);

					btCollisionObject *pTar = CSAApp::Get().m_spZombie1->m_pcloBody; //몸통 물리 충돌오브잭트 얻기

					//해당타겟이 명중인지?
					if(obA == pTar || obB == pTar)
					{
						CSAApp::Get().m_spZombie1->Signal("hit",pBullet);
					}

					std::cout << "current bullet num :" << m_mapBullets.size() << std::endl;
				}
			}

		}

		
		//총알 컨테이너 관리
		{
			std::map<btCollisionObject *,SP_IFSMObject>::iterator it = m_mapBullets.begin();
			while(it != m_mapBullets.end())
			{			

				if(it->second->isDie())
				{				
					m_mapBullets.erase(it++);
					if(m_mapBullets.empty())
						break;
				}
				else
				{
					it->second->Update(fDelta);
					it++;
				}

			}
		}

	}

	

}

void CHeroPlayer::Signal(std::string strSignal, void *pParam)
{

}

bool CHeroPlayer::OnEvent(const irr::SEvent &event)
{
	switch(event.EventType)
	{
	case irr::EET_GUI_EVENT:
		{

		}
		break;
	case irr::EET_KEY_INPUT_EVENT:
		{
			if(event.KeyInput.PressedDown)
			{
				m_Key[event.KeyInput.Key] = true;
			}
			else
			{
				m_Key[event.KeyInput.Key] = false;
			}
		}
		break;
	case irr::EET_MOUSE_INPUT_EVENT:
		{
			if(event.MouseInput.Event == irr::EMIE_LMOUSE_PRESSED_DOWN)
			{
				m_Key[irr::KEY_LBUTTON] = true;
			}
			else if(event.MouseInput.Event == irr::EMIE_LMOUSE_LEFT_UP)
			{
				m_Key[irr::KEY_LBUTTON] = false;
			}

		}
		break;
	case irr::EET_USER_EVENT:
		break;
	}
	return false;
}
