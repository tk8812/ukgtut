#pragma once

class CAIPlayer : public CPlayer
{
	irr::scene::CBulletChracterAnimator *m_pChracterAnimator;

	irr::core::vector3df m_vTargetDir;

	irr::f32 m_accTick; //누적 시간계산용


public:
	CAIPlayer(void);
	virtual ~CAIPlayer(void);

	virtual void Signal(std::string strSignal,void *pParam);
	virtual void Update(irr::f32 fDelta);

	virtual bool Init(irr::scene::ISceneNode *pNode);
};
