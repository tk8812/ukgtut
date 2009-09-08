#include "CSAApp.h"
#include "HeroPlayer.h"

#include "AIPlayer.h"

CAIPlayer::CAIPlayer(void)
{
	m_strTypeName = "CAIPlayer";	
}

CAIPlayer::~CAIPlayer(void)
{
}

bool CAIPlayer::Init(irr::scene::ISceneNode *pNode)
{
	if(CPlayer::Init(pNode))
	{

		irr::scene::CBulletObjectAnimatorParams physicsParams;
		irr::scene::ISceneManager *pSmgr = CSAApp::GetPtr()->m_pSmgr;
		irr::IrrlichtDevice *pDevice = CSAApp::GetPtr()->m_pDevice;
		irr::scene::CBulletAnimatorManager* pBulletPhysicsFactory = CSAApp::GetPtr()->m_pBulletPhysicsFactory; 
		irr::scene::CBulletWorldAnimator* pWorldAnimator = CSAApp::GetPtr()->m_pWorldAnimator;

		if(m_pCollMngNode && 
			m_pTrigerNode && 			
			m_pCollMngNode_Body && //오브잭트가 널인지 먼저 검사하고
			(m_pNode->getType() == irr::scene::jz3d::CFormatedAnimationNode::TypeID) //아니라면 해당오브잭트에 접근한다.
			)
		{
			//m_pNode->setVisible(false);		

			m_pTrigerNode = pSmgr->getSceneNodeFromName("usr/triger");
			m_pCollMngNode = (irr::scene::jz3d::CCollusionMngNode *)pSmgr->getSceneNodeFromName("col_main",m_pNode);//캐릭터볼륨얻기(이동제어용)
			m_pCollMngNode_Kick = (irr::scene::jz3d::CCollusionMngNode *)pSmgr->getSceneNodeFromName("col_kick",m_pNode); //발차기 충돌정보
			m_pCollMngNode_Body = (irr::scene::jz3d::CCollusionMngNode *)pSmgr->getSceneNodeFromName("col_body",m_pNode); //몸통 충돌정보		

			physicsParams.mass = 70.f; //70kg
			physicsParams.gravity = core::vector3df(0, 0, 0);
			physicsParams.friction = 10.f; //마찰값		


			irr::scene::CBulletChracterAnimator *pAnim = pBulletPhysicsFactory->createBulletCharcterAnimator(
				pSmgr,
				m_pNode,
				pWorldAnimator->getID(),
				m_pCollMngNode->getGeometryInfo(),
				&physicsParams
				);

			m_pNode->addAnimator(pAnim);
			m_pChracterAnimator = pAnim;	

			//부모노드의 스케일변환을 고려하기 위해서 한단계위의 스캐일변환을 곱해준다.
			m_pChracterAnimator->setLocalPosition(
				-(m_pCollMngNode->getPosition() * m_pCollMngNode->getParent()->getScale())
				);

			pAnim->drop();

			return true;
		}
		else
		{
			char szBuf[256];
			sprintf_s(szBuf,256,"can not process CPlayer::Init [%s] scenenode check node format!!",m_pNode->getName());
			pDevice->getLogger()->log(szBuf,irr::ELL_ERROR);
			return false;
		}
	}

	return false;
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
	irr::scene::ISceneManager *pSmgr = CSAApp::GetPtr()->m_pSmgr;
	irr::IrrlichtDevice *pDevice = CSAApp::GetPtr()->m_pDevice;
	irr::scene::CBulletAnimatorManager* pBulletPhysicsFactory = CSAApp::GetPtr()->m_pBulletPhysicsFactory; 
	irr::scene::CBulletWorldAnimator* pWorldAnimator = CSAApp::GetPtr()->m_pWorldAnimator;

	switch(GetStatus())
	{
	case FSM_READY:
		{
			//시작위치 지정
			m_pNode->setVisible(true);			
			irr::core::vector3df pos_Spwan = pSmgr->getSceneNodeFromName("genpos/zombie",m_pTrigerNode)->getAbsolutePosition();
			m_pChracterAnimator->setPosition(pos_Spwan);
			m_pChracterAnimator->zeroForces();
			//캐릭터 중심점을 보정해준다.
			

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
