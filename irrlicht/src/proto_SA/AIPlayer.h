#pragma once

class CAIPlayer : public CPlayer
{
	irr::scene::CBulletChracterAnimator *m_pChracterAnimator;
public:
	CAIPlayer(void);
	virtual ~CAIPlayer(void);

	virtual void Signal(std::string strSignal,void *pParam);
	virtual void Update(irr::f32 fTick);

	virtual bool Init(irr::scene::ISceneNode *pNode);
};
