#pragma once

//#include "tutapp1.h"



namespace irr
{
	namespace scene
	{
		class CBulletAnimatorManager;
		class CBulletObjectAnimator;
		class CBulletWorldAnimator;
	};
};


class CTutApp5
	:public ggf::irr_base::IBaseApp, public ggf::oop::Singleton<CTutApp5>,public irr::IEventReceiver
{
public:
	CTutApp5(void);
	~CTutApp5(void);

	irr::scene::CBulletAnimatorManager* m_pBulletPhysicsFactory;
	irr::scene::ISceneNode*  m_pWorldNode;
	irr::scene::CBulletWorldAnimator* m_pWorldAnimator;

	virtual int Init(LPCWSTR lpclassName);
	virtual bool Apply(float fElapsedTime);
	virtual void Render(float fElapsedTime);	


	bool OnEvent(const irr::SEvent& event);

	void Close()
	{
		m_pDevice->closeDevice();
		m_pDevice->drop();	

		UnregisterClass( _bstr_t(m_strClassName.c_str()),m_hInstance );
	}
};
