#include <irrlicht.h>
#include <string>

//#include "HeroPlayer.h"

#include "CdmkApp.h"



CHeroPlayer::CHeroPlayer(irr::scene::ISceneNode *pNode)
: IFSMObject(),
m_pNode(pNode)

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
	{
		irr::scene::jz3d::CFormatedAnimationNode *pNode = 
			(irr::scene::jz3d::CFormatedAnimationNode *)m_pNode;
		pNode->setVisible(true);		
		pNode->changeAction("stand");	

		
		pNode->setPosition(			
			pSmgr->getSceneNodeFromName("start",pSmgr->getSceneNodeFromName("usr/triger"))->getAbsolutePosition()
			);
	}

	return true;
}

