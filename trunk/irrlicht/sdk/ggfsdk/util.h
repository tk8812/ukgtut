#pragma once
namespace ggf {
	namespace utils {

		inline void RecursiveEvent(irr::scene::ISceneNode *pNode, const irr::SEvent &event)
		{			
			irr::core::list<irr::scene::ISceneNodeAnimator*> Animators = pNode->getAnimators();

			irr::core::list<irr::scene::ISceneNodeAnimator*>::Iterator ait = Animators.begin();
			for (; ait != Animators.end(); ++ait)
			{
				if((*ait)->isEventReceiverEnabled())
				{
					(*ait)->OnEvent(event);
				}
			}

			irr::core::list<irr::scene::ISceneNode*> Children = pNode->getChildren();

			irr::core::list<irr::scene::ISceneNode*>::Iterator it = Children.begin();
			for (; it != Children.end(); ++it)	
			{
				RecursiveEvent((*it),event);				
			}
		}
	}
}