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

	irr::scene::jz3d::CCollusionMngNode *getBodyCollusionObject()
	{
		return m_pCollMngNode_Body;
	}

	virtual bool Init(irr::scene::ISceneNode *pNode);	

	inline virtual irr::core::vector3df getPosition() {
		return m_pNode->getAbsolutePosition();
	}
};
