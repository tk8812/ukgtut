#pragma once


#include "FSMObject.h"


class CHeroPlayer :
	public irr::IEventReceiver,
	public IFSMObject
{
public:
	CHeroPlayer();
	virtual ~CHeroPlayer(void);

	virtual void Signal(std::string strSignal,void *pParam);
	virtual void Update(irr::f32 fTick);

	virtual bool OnEvent(const irr::SEvent& event);

	irr::scene::jz3d::CFormatedAnimationNode *m_pNode;
	irr::scene::CBulletObjectAnimator *m_pObjectAnimator;

	bool Init();

	irr::scene::ISceneNode *m_pTrigerNode;

};
