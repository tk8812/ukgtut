#include "CollusionMngPluginNode.h"

namespace irr 
{
	namespace scene
	{
		namespace jz3d 
		{
			const char *CCollusionMngNode::Name  = "CCollusionMngNode";

			CCollusionMngNode::CCollusionMngNode(ISceneNode* parent, ISceneManager* mgr, s32 id)
				: ISceneNode(parent,mgr,id)
			{
#ifdef _DEBUG
				setDebugName(CCollusionMngNode::Name);
#endif
				setAutomaticCulling(scene::EAC_OFF);
			}
			

			//! pre render event
			void CCollusionMngNode::OnRegisterSceneNode()
			{
				if (IsVisible)
					SceneManager->registerNodeForRendering(this);

				ISceneNode::OnRegisterSceneNode();
			}


			//! render
			void CCollusionMngNode::render()
			{				

				video::IVideoDriver* driver = SceneManager->getVideoDriver();


				if(Parent)
				{
					if(Parent->getType() == ESNT_ANIMATED_MESH)
					{
						irr::scene::IAnimatedMeshSceneNode *pNode = (irr::scene::IAnimatedMeshSceneNode *)Parent;

						int i;//count = pNode->getJointCount();

						for(i=0;i<pNode->getJointCount();i++)
						{
							irr::scene::IBoneSceneNode *pBone =
								pNode->getJointNode(i);

							/*irr::core::aabbox3df box(
								irr::core::vector3df(-1,-1,-1), 
								irr::core::vector3df(1,1,1));

							driver->setTransform(video::ETS_WORLD,pBone->getAbsoluteTransformation());

							driver->draw3DBox(box,irr::video::SColor(255,255,0,0));*/
							
						}						
					}
				}
			}


			//! returns the axis aligned bounding box of this node
			const core::aabbox3d<f32>& CCollusionMngNode::getBoundingBox() const
			{
				return Box;
			}

			//! Creates a clone of this scene node and its children.
			ISceneNode* CCollusionMngNode::clone(ISceneNode* newParent, ISceneManager* newManager)
			{
				if (!newParent) newParent = Parent;
				if (!newManager) newManager = SceneManager;

				CCollusionMngNode* nb = new CCollusionMngNode(newParent,
					newManager, ID);

				nb->cloneMembers(this, newManager);
				nb->Box = Box;

				nb->drop();
				return nb;
			}		


			void CCollusionMngNode::serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options) const
			{
				ISceneNode::serializeAttributes(out, options);		
				
				//데이터 쓰기 코드추가
				//...

			}

			//! Reads attributes of the scene node.
			void CCollusionMngNode::deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options)
			{	
				//데이터 읽기 코드추가
				//...

				ISceneNode::deserializeAttributes(in, options);				
			}			
		}
	}
}
