#pragma once

#include <irrlicht.h>
#include <crtdbg.h>
#include <windows.h>
#include <memory>
#include <vector>

#include "../sdk/ggfsdk/Singleton.h"
#include "HeroPlayer.h"
//#include "../sdk/ggfsdk/util.h"


#include "../irredit/source/plugins/jz3dplugins/JZ3DSceneNodeFactory.h"

#include "../irredit/source/plugins/jz3dplugins/CollusionMngPluginNode.h"
#include "../irredit/source/plugins/jz3dplugins/FormatedAnimationNode.h"




class CdmkApp :  
	public irr::IEventReceiver,
	public ggf::oop::Singleton<CdmkApp>
{
public:
	CdmkApp(void);
	~CdmkApp(void);

	irr::IrrlichtDevice *m_pDevice;
	irr::video::IVideoDriver *m_pVideo;
	irr::scene::ISceneManager *m_pSmgr;
	irr::gui::IGUIEnvironment *m_pGuiEnv;

	std::tr1::shared_ptr<CHeroPlayer> m_spHeroPlayer;
	irr::scene::CBulletAnimatorManager* m_pBulletPhysicsFactory; 
	irr::scene::CBulletWorldAnimator* m_pWorldAnimator; 	


	//이벤트 핸들러
	bool OnEvent(const irr::SEvent& event);	

	bool Init();	
	void Update();
	void Render();

	inline void gameloop()
	{
		while(m_pDevice->run())
		{  
			Update();
			Render();			
		}
	}

};
