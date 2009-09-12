#pragma once


class CHeroPlayer : public CPlayer,public irr::IEventReceiver
	
{
	bool m_Key[irr::KEY_KEY_CODES_COUNT];

	irr::scene::CBulletFpsCamAnimator *m_pChracterAnimator;

public:

	std::map<btCollisionObject *,SP_IFSMObject> m_mapBullets;
	

	CHeroPlayer();
	virtual ~CHeroPlayer(void);

	virtual void Signal(std::string strSignal,void *pParam);
	virtual void Update(irr::f32 fTick);

	virtual bool OnEvent(const irr::SEvent& event);	
	virtual bool Init(irr::scene::ISceneNode *pNode);

	inline virtual irr::core::vector3df getPosition() {	return m_pChracterAnimator->getPosition();}

};
