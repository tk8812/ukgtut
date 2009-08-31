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

void CAIPlayer::Signal(std::string strSignal,void *pParam)
{
	if(strSignal == "kicked")
	{
		CPlayer *doer = (CPlayer *)pParam;
		irr::core::vector3df vdodir = doer->getPosition() - getPosition();
		vdodir.normalize();
		
		irr::core::vector3df vblowDir(-vdodir.X,0,-vdodir.Z);		
		m_pChracterAnimator->applyImpulse(700.f * vblowDir);
		m_pChracterAnimator->applyImpulse(70.f * irr::core::vector3df(0,1,0));

		SetStatus(FSM_ATTACKED);
	}
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
	case FSM_ATTACKED:
		if(GetStep() == 0)
		{
			m_pNode->changeAction("damage",false);			

			IncStep();
		}
		else if(GetStep() == 1)
		{	
			if(m_pNode->IsActionFinished()) 
			{	
				SetStatus(FSM_STAND);
			}
		}
		break;	
	}
}
