#include "CSAApp.h"
#include "player.h"

bool CPlayer::Init(irr::scene::ISceneNode *pNode)
{

	if(pNode->getType() != irr::scene::jz3d::CFormatedAnimationNode::TypeID)
		return false;

	m_pNode = (irr::scene::jz3d::CFormatedAnimationNode *)pNode;//pSmgr->getSceneNodeFromName("usr/obj/b3d/ninja/blue");

	m_pNode->setVisible(false);		
	
	return true;
}