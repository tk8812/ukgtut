#include "CollusionMngPluginNode.h"

namespace irr 
{
	namespace scene
	{
		namespace jz3d 
		{
			const char *CCollusionMngNode::Name  = "CCollusionMngNode";

			CCollusionMngNode::CCollusionMngNode(ISceneNode* parent, ISceneManager* mgr, s32 id)
				: ISceneNode(parent,mgr,id),
				m_strBone("")
			{
#ifdef _DEBUG
				setDebugName(CCollusionMngNode::Name);
#endif
				setAutomaticCulling(scene::EAC_OFF);

				static char *a1 = "test";
				static char *a2 = "test2";

				//동적 열거형 만들기위한 예제
				m_BoneList.push_back(a1);
				m_BoneList.push_back(a2);
				m_BoneList.push_back(NULL);
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

				if ( DebugDataVisible & scene::EDS_BBOX )
				{
					if(Parent)
					{
						if(Parent->getType() == ESNT_ANIMATED_MESH)
						{
							irr::scene::IAnimatedMeshSceneNode *pNode = (irr::scene::IAnimatedMeshSceneNode *)Parent;
							irr::scene::IBoneSceneNode *pBone = pNode->getJointNode(m_strBone.c_str());
							if(pBone)
							{
								irr::core::aabbox3df box(
									irr::core::vector3df(-1,-1,-1), 
									irr::core::vector3df(1,1,1));

								driver->setTransform(video::ETS_WORLD,pBone->getAbsoluteTransformation());

								driver->draw3DBox(box,irr::video::SColor(255,255,0,0));
							}
						}
					}
				}


				//if(Parent)
				//{
				//	if(Parent->getType() == ESNT_ANIMATED_MESH)
				//	{
				//		irr::scene::IAnimatedMeshSceneNode *pNode = (irr::scene::IAnimatedMeshSceneNode *)Parent;

				//		int i;//count = pNode->getJointCount();

				//		for(i=0;i<pNode->getJointCount();i++)
				//		{
				//			irr::scene::IBoneSceneNode *pBone =
				//				pNode->getJointNode(i);

				//			/*irr::core::aabbox3df box(
				//				irr::core::vector3df(-1,-1,-1), 
				//				irr::core::vector3df(1,1,1));

				//			driver->setTransform(video::ETS_WORLD,pBone->getAbsoluteTransformation());

				//			driver->draw3DBox(box,irr::video::SColor(255,255,0,0));*/
				//			
				//		}						
				//	}
				//}
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
				nb->m_strBone = m_strBone;

				nb->drop();
				return nb;
			}		


			void CCollusionMngNode::serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options) const
			{
				ISceneNode::serializeAttributes(out, options);		
				
				//데이터 쓰기 코드추가
				//...
				out->addString("bone",m_strBone.c_str());
				out->addEnum("bones",0,(char **)&m_BoneList[0]);

			}

			//! Reads attributes of the scene node.
			void CCollusionMngNode::deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options)
			{	
				//데이터 읽기 코드추가
				//...

				m_strBone = in->getAttributeAsString("bone");

				ISceneNode::deserializeAttributes(in, options);				
			}			
		}
	}
}
