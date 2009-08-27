#pragma once

#include "FSMObject.h"

class CPlayer :	
	public IFSMObject
{

protected:
	irr::scene::jz3d::CFormatedAnimationNode *m_pNode;
	irr::scene::CBulletChracterAnimator *m_pChracterAnimator;

	//충돌정보관리
	irr::scene::jz3d::CCollusionMngNode * m_pCollMngNode;
	irr::scene::jz3d::CCollusionMngNode * m_pCollMngNode_Kick;
	irr::scene::jz3d::CCollusionMngNode * m_pCollMngNode_Body;

	irr::scene::ISceneNode *m_pTrigerNode;

public:

	enum {
		FSM_READY,
		FSM_STAND,
		FSM_WALK,
		FSM_KICK,
		FSM_ATTACKED,
		FSM_COUNT
	};

	CPlayer(void) : IFSMObject(),
		m_pNode(0)		
	{
	}

	virtual ~CPlayer(void)
	{
	}

	inline virtual bool Init(irr::scene::ISceneNode *pNode)
	{



		irr::scene::CBulletObjectAnimatorParams physicsParams;
		irr::scene::ISceneManager *pSmgr = CdmkApp::GetPtr()->m_pSmgr;
		irr::IrrlichtDevice *pDevice = CdmkApp::GetPtr()->m_pDevice;
		irr::scene::CBulletAnimatorManager* pBulletPhysicsFactory = CdmkApp::GetPtr()->m_pBulletPhysicsFactory; 
		irr::scene::CBulletWorldAnimator* pWorldAnimator = CdmkApp::GetPtr()->m_pWorldAnimator;


		if(pNode->getType() != irr::scene::jz3d::CFormatedAnimationNode::TypeID)
			return false;

		m_pNode = (irr::scene::jz3d::CFormatedAnimationNode *)pNode;//pSmgr->getSceneNodeFromName("usr/obj/b3d/ninja/blue");
		//m_pNode->OnAnimate(0);		


		if(m_pCollMngNode && 
			m_pTrigerNode && 			
			m_pCollMngNode_Body && //오브잭트가 널인지 먼저 검사하고
			(m_pNode->getType() == irr::scene::jz3d::CFormatedAnimationNode::TypeID) //아니라면 해당오브잭트에 접근한다.
			)
		{
			m_pNode->setVisible(false);		

			
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
			m_pChracterAnimator->setLocalPosition(-m_pCollMngNode->getAbsolutePosition());			
			//m_pChracterAnimator->getRigidBody()->setDamping(.9,0);

			pAnim->drop();	

		}

		else
		{
			char szBuf[256];
			sprintf_s(szBuf,256,"can not process CPlayer::Init [%s] scenenode check node format!!",m_pNode->getName());
			pDevice->getLogger()->log(szBuf,irr::ELL_ERROR);
			return false;
		}

		return true;
	}
};
