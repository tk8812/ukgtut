#include "CSAApp.h"

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
			m_pCollMngNode = (irr::scene::jz3d::CCollusionMngNode *)pSmgr->getSceneNodeFromName("col_main",m_pNode);//캐릭터볼륨얻기(이동제어용)
			m_pCollMngNode_Kick = (irr::scene::jz3d::CCollusionMngNode *)pSmgr->getSceneNodeFromName("col_kick",m_pNode); //발차기 충돌정보
			m_pCollMngNode_Body = (irr::scene::jz3d::CCollusionMngNode *)pSmgr->getSceneNodeFromName("col_body",m_pNode); //몸통 충돌정보		

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
		}
		return true;
	}
	
	/*if(CPlayer::Init(pNode))
	{		
		return true;
	}
	*/
	return false;
}



void CHeroPlayer::Update(irr::f32 fDelta)
{	
	irr::scene::ISceneManager *pSmgr = CSAApp::GetPtr()->m_pSmgr;
	irr::IrrlichtDevice *pDevice = CSAApp::GetPtr()->m_pDevice;
	irr::scene::CBulletAnimatorManager* pBulletPhysicsFactory = CSAApp::GetPtr()->m_pBulletPhysicsFactory; 
	irr::scene::CBulletWorldAnimator* pWorldAnimator = CSAApp::GetPtr()->m_pWorldAnimator;	

	switch(GetStatus())
	{
	case CHeroPlayer::FSM_READY:
		{
			//시작위치 지정
			//m_pNode->setVisible(true);			
			irr::core::vector3df pos_Spwan = pSmgr->getSceneNodeFromName("start",m_pTrigerNode)->getAbsolutePosition();
			m_pChracterAnimator->setPosition(pos_Spwan);
			m_pChracterAnimator->zeroForces();
			m_pChracterAnimator->setLocalPosition(irr::core::vector3df(0,5,0));
			m_pChracterAnimator->setMoveSpeed(1.1f);			

			SetStatus(FSM_STAND);
		}
		break;
	case CHeroPlayer::FSM_STAND:
		{
			if( m_Key[irr::KEY_LBUTTON] )
			{
				m_Key[irr::KEY_LBUTTON] = false;



			}
		}		
		break;
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

		}
		break;
	case irr::EET_USER_EVENT:
		break;
	}
	return false;
}
