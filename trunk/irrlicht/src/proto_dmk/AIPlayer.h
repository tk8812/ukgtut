#pragma once

class CAIPlayer : public CPlayer
{
public:
	CAIPlayer(void);
	virtual ~CAIPlayer(void);

	virtual void Signal(std::string strSignal,void *pParam);
	virtual void Update(irr::f32 fTick);
};
