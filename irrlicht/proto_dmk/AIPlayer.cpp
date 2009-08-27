#include "CdmkApp.h"
#include "HeroPlayer.h"

#include "AIPlayer.h"

CAIPlayer::CAIPlayer(void)
{
	m_strTypeName = "CAIPlayer";	
}

CAIPlayer::~CAIPlayer(void)
{
}

void CAIPlayer::Signal(std::string,void *pParam)
{
}

void CAIPlayer::Update(irr::f32 fTick)
{
	irr::scene::ISceneManager *pSmgr = CdmkApp::GetPtr()->m_pSmgr;
	irr::IrrlichtDevice *pDevice = CdmkApp::GetPtr()->m_pDevice;
	irr::scene::CBulletAnimatorManager* pBulletPhysicsFactory = CdmkApp::GetPtr()->m_pBulletPhysicsFactory; 
	irr::scene::CBulletWorldAnimator* pWorldAnimator = CdmkApp::GetPtr()->m_pWorldAnimator;

	switch(GetStatus())
	{
	case FSM_READY:
		{
			//시작위치 지정
			m_pNode->setVisible(true);			
			irr::core::vector3df pos_Spwan = pSmgr->getSceneNodeFromName("genpos/zombie",m_pTrigerNode)->getAbsolutePosition();
			m_pChracterAnimator->setPosition(pos_Spwan);
			m_pChracterAnimator->zeroForces();

			SetStatus(FSM_STAND);
		}
		break;
	case FSM_STAND:
		if(GetStep() == 0)
		{
			m_pNode->changeAction("stand");			

			IncStep();
		}
		else if(GetStep() == 1)
		{			
			
		}
		break;
	case FSM_WALK:
		break;
	case FSM_KICK:
		break;
	}
}
