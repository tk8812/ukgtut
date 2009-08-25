#include "CdmkApp.h"

#include "HeroPlayer.h"



CHeroPlayer::CHeroPlayer()
: IFSMObject()
{
	m_strTypeName = "CHeroPlayer";	
}

CHeroPlayer::~CHeroPlayer(void)
{
}

void CHeroPlayer::Update(irr::f32 fTick)
{

}

void CHeroPlayer::Signal(std::string strSignal, void *pParam)
{

}

bool CHeroPlayer::OnEvent(const irr::SEvent &event)
{
	switch(event.EventType)
	{
	case irr::EET_GUI_EVENT:
		{

		}
		break;
	case irr::EET_KEY_INPUT_EVENT:
		{
		}
		break;
	case irr::EET_MOUSE_INPUT_EVENT:
		{

		}
		break;
	case irr::EET_USER_EVENT:
		break;
	}
	return false;
}

bool CHeroPlayer::Init()
{

	irr::scene::ISceneManager *pSmgr = CdmkApp::GetPtr()->m_pSmgr;
	irr::IrrlichtDevice *pDevice = CdmkApp::GetPtr()->m_pDevice;
	irr::scene::CBulletAnimatorManager* pBulletPhysicsFactory = CdmkApp::GetPtr()->m_pBulletPhysicsFactory; 
	irr::scene::CBulletWorldAnimator* pWorldAnimator = CdmkApp::GetPtr()->m_pWorldAnimator;


	//irr::scene::ISceneNode *pNode;	

	m_pNode = (irr::scene::jz3d::CFormatedAnimationNode *)pSmgr->getSceneNodeFromName("usr/obj/b3d/ninja/blue");
	m_pTrigerNode = pSmgr->getSceneNodeFromName("usr/triger");
	//m_pNode = 0;

	
	if(m_pNode && m_pTrigerNode && //오브잭트가 널인지 먼저 검사하고
		(m_pNode->getType() == irr::scene::jz3d::CFormatedAnimationNode::TypeID) //아니라면 해당오브잭트에 접근한다.
		)
	{
		irr::scene::CBulletObjectAnimatorGeometry geom;
		irr::scene::CBulletObjectAnimatorParams physicsParams;

		//irr::scene::ISceneNode *pNode = m_pNode;
		
		m_pNode->setVisible(true);		
		m_pNode->changeAction("stand");			

		
		irr::core::vector3df start_Pos = 
			pSmgr->getSceneNodeFromName("start",m_pTrigerNode)->getAbsolutePosition();

		m_pNode->setPosition(start_Pos);

		geom.type = irr::scene::CBPAGT_CAPSULE;
		geom.Capsule.hight = .5f;
		geom.Capsule.radius = 0.5f;


		physicsParams.mass = 70.f; //70kg
		physicsParams.gravity = core::vector3df(0, 0, 0);
		physicsParams.friction = .6f; //마찰값		
		//physicsParams.centerOfMassOffset = irr::core::vector3df(0,10,0);


		irr::scene::CBulletObjectAnimator *pAnim = pBulletPhysicsFactory->createBulletCharcterAnimator(
			pSmgr,
			m_pNode,
			pWorldAnimator->getID(),
			&geom,
			&physicsParams
			);
		m_pNode->addAnimator(pAnim);			
		m_pObjectAnimator = pAnim;			
		pAnim->drop();

		pSmgr->getSceneNodeFromName("body",m_pNode)->setPosition(irr::core::vector3df(0,-5,0));

		//CHeroPlayer *pHero = new CHeroPlayer();
		//pHero->Init();
		//m_spHeroPlayer = std::tr1::shared_ptr<IFSMObject>(pHero);		
	}
	else
	{
		pDevice->getLogger()->log("can not found [usr/obj/b3d/ninja/blue] scenenode",irr::ELL_ERROR);
		return false;
	}

	
	{
		
	}

	return true;
}

