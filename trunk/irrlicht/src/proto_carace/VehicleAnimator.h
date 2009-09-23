#pragma once

class CVehicleAnimator : public irr::scene::ISceneNodeAnimator
{
public:
	CVehicleAnimator(void);
	virtual ~CVehicleAnimator(void);

	void *m_pProxy;
	inline void setProxy(void *proxy)
	{
		m_pProxy = proxy;
	}


	virtual void animateNode(irr::scene::ISceneNode* node, u32 timeMs);
};
