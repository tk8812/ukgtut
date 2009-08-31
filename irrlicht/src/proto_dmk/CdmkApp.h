#pragma once

#include <irrlicht.h>
#include <crtdbg.h>
#include <windows.h>
#include <memory>
#include <vector>
#include <string>
#include <list>
#include <map>
#include <iostream>

//bullet
#include "CFollowAnimator.h"
#include "CBulletAnimatorManager.h"
#include "CBulletWorldAnimator.h"
#include "btBulletDynamicsCommon.h"

#include "BulletCollision/NarrowPhaseCollision/btGjkPairDetector.h"
#include "BulletCollision/NarrowPhaseCollision/btPointCollector.h"
#include "BulletCollision/NarrowPhaseCollision/btVoronoiSimplexSolver.h"
#include "BulletCollision/NarrowPhaseCollision/btConvexPenetrationDepthSolver.h"
#include "BulletCollision/NarrowPhaseCollision/btGjkEpaPenetrationDepthSolver.h"

#include "btBulletDynamicsCommon.h"
#include "BulletDynamics/Character/btKinematicCharacterController.h"
#include "BulletCollision/CollisionDispatch/btGhostObject.h"


//ggf
#include "Singleton.h"
#include "util.h"

//#include "FSMObject.h"
//#include "player.h"
//#include "HeroPlayer.h"

//pulgin node
#include "jz3dplugins/JZ3DSceneNodeFactory.h"
#include "jz3dplugins/CollusionMngPluginNode.h"
#include "jz3dplugins/FormatedAnimationNode.h"
#include "jz3dplugins/TiledPlaneNode.h"
#include "jz3dplugins/BulletPhysicsObjectNode.h"


//게임플레이어컨트롤 인터패이스
#include "FSMObject.h"
#include "Player.h"

typedef std::tr1::shared_ptr<CPlayer> SP_PLAYER;



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

	SP_PLAYER m_spHeroPlayer;//주인공(컨트롤러블)
	SP_PLAYER m_spZombie1; //적오브잭트

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
