#pragma once

class CHeroPlayer : public CPlayer,public irr::IEventReceiver
	
{
	bool m_Key[irr::KEY_KEY_CODES_COUNT];

public:

	

	CHeroPlayer();
	virtual ~CHeroPlayer(void);

	virtual void Signal(std::string strSignal,void *pParam);
	virtual void Update(irr::f32 fTick);

	virtual bool OnEvent(const irr::SEvent& event);	
	virtual bool Init(irr::scene::ISceneNode *pNode);

	

};
