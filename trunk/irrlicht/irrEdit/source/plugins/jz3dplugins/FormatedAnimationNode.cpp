#include "FormatedAnimationNode.h"

namespace irr 
{
	namespace scene
	{
		namespace jz3d 
		{
			const char *CFormatedAnimationNode::Name = "FormatedAnimationNode";

			const c8* CFormatedAnimationNode::ActionNames[] =			
			{
				"Stand",
				"Walk",
				"Attack",
				"damage",
				"die"
			};			

			const c8* CFormatedAnimationNode::CmdListNames[] =			
			{
				"---",
				"joint"				
			};	

			CFormatedAnimationNode::CFormatedAnimationNode(ISceneNode* parent, ISceneManager* mgr, s32 id)
				: ISceneNode(parent, mgr, id),
				m_CurAnimation(ActionNames[0]),
				m_TargetNodeName("body"),
				m_CurCmd("---"),
				m_status(0)
			{
#ifdef _DEBUG
				setDebugName("CFormatedAnimationNode");
#endif
				setAutomaticCulling(scene::EAC_OFF);				

				//액션정보 초기화
				irr::s32 i;
				irr::s32 AniNum = sizeof(CFormatedAnimationNode::ActionNames)/sizeof(c8*);
				for(i=0;i< AniNum ; i++)
				{
					m_mapActionDesc[std::string(ActionNames[i])] = irr::core::vector3df(0,0,0);
				}

				
			}


			//! pre render event
			void CFormatedAnimationNode::OnRegisterSceneNode()
			{
				if (IsVisible)
					SceneManager->registerNodeForRendering(this);

				ISceneNode::OnRegisterSceneNode();
			}


			//! render
			void CFormatedAnimationNode::render()
			{
				// do nothing
			}


			//! returns the axis aligned bounding box of this node
			const core::aabbox3d<f32>& CFormatedAnimationNode::getBoundingBox() const
			{
				return Box;
			}


			//! Creates a clone of this scene node and its children.
			ISceneNode* CFormatedAnimationNode::clone(ISceneNode* newParent, ISceneManager* newManager)
			{
				if (!newParent) newParent = Parent;
				if (!newManager) newManager = SceneManager;

				CFormatedAnimationNode* nb = new CFormatedAnimationNode(newParent, 
					newManager, ID);

				nb->cloneMembers(this, newManager);
				nb->Box = Box;
				nb->m_CurAnimation = m_CurAnimation;
				nb->m_mapActionDesc = m_mapActionDesc;
				nb->m_TargetNodeName = m_TargetNodeName;

				nb->drop();
				return nb;
			}


			void CFormatedAnimationNode::serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options) const
			{
				ISceneNode::serializeAttributes(out, options);				
				
				out->addEnum("cmd",m_CurCmd.c_str(),CFormatedAnimationNode::CmdListNames);
				out->addEnum("animation",m_CurAnimation.c_str(),CFormatedAnimationNode::ActionNames);	
				out->addString("target",m_TargetNodeName.c_str());
				
				
				/*if(m_mapActionDesc.size())
				{
					out->addEnum("animation",
						m_CurAnimation.c_str(),
						m_EnumActionList						
					);
				}*/
				
				//out->addBool("add Action",m_bAddActionBtn);				
				//out->addString("add Action Nanme",m_strAddActionName.c_str());

				std::map<std::string,irr::core::vector3df>::const_iterator it;

				for(it = m_mapActionDesc.begin();it !=m_mapActionDesc.end();it++)
				{  
					out->addVector3d(it->first.c_str(),it->second);
				}
				/*out->addArray("action list",m_strActionList);
				char szBuf[256];

				out->addString("Mesh", SceneManager->getMeshCache()->getMeshFilename(Mesh));
				out->addBool("ReadOnlyMaterials", ReadOnlyMaterials);

				sprintf_s(szBuf,256,"%d,%d",m_TileCount.Width,m_TileCount.Height);
				out->addString("TileCount",szBuf);

				sprintf_s(szBuf,256,"%.2f,%.2f",m_TileSize.Width,m_TileSize.Height);
				out->addString("TileSize",szBuf);*/
			}

			//! Reads attributes of the scene node.
			void CFormatedAnimationNode::deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options)
			{	
				m_CurCmd = in->getAttributeAsEnumeration("cmd"); //커멘트 읽기

				if(m_CurCmd == "---")
				{
				}
				else if(m_CurCmd == "joint")
				{
					irr::scene::IAnimatedMeshSceneNode *pTarNode = 
						(irr::scene::IAnimatedMeshSceneNode *)SceneManager->getSceneNodeFromName(
						m_TargetNodeName.c_str(),this);

					if(pTarNode)
					{
						int i;

						for(i=0;i<pTarNode->getJointCount();i++)
						{
							irr::scene::IBoneSceneNode *pBone =
								pTarNode->getJointNode(i);
						}
					}

					m_CurCmd = "---";
				}

				if(m_mapActionDesc.size())
				{
					m_CurAnimation = in->getAttributeAsEnumeration("animation");//,CFormatedAnimationNode::ActionNames);
				}

				m_TargetNodeName = in->getAttributeAsString("target");

				//bool oldAddActionBtn = m_bAddActionBtn;
				//액션을 추가한다.
				//m_bAddActionBtn = in->getAttributeAsBool("add Action");	
				//m_strAddActionName = in->getAttributeAsString("add Action Nanme").c_str();
				/*if(m_bAddActionBtn != oldAddActionBtn && m_mapActionDesc.count(m_strAddActionName) == 0)
				{					
					if(m_mapActionDesc.count(m_strAddActionName) == 0)
					{
						m_strActionList.push_back(irr::core::stringw(m_strAddActionName.c_str()));
					}

					m_mapActionDesc[m_strAddActionName] = irr::core::vector3df(0,0,0);
				}

				std::map<std::string,irr::core::vector3df>::iterator it;
				if(m_EnumActionList)
				{
					delete m_EnumActionList;
				}

				m_EnumActionList = new char*[m_mapActionDesc.size()];
				*/

				//액션정보얻기
				{
					std::map<std::string,irr::core::vector3df>::iterator it;
					for(it = m_mapActionDesc.begin();it !=m_mapActionDesc.end();it++)
					{
						it->second = in->getAttributeAsVector3d(it->first.c_str());
						//m_EnumActionList[i++] = (char *)it->first.c_str();
						//m_EnumActionList.push_back(it->first.c_str());
					}
				}

				changeAction(m_CurAnimation);

				ISceneNode::deserializeAttributes(in, options);				
			}

			void CFormatedAnimationNode::OnAnimate(irr::u32 timeMs)
			{
				/*irr::scene::IAnimatedMeshSceneNode *pTarNode = 
					(irr::scene::IAnimatedMeshSceneNode *)SceneManager->getSceneNodeFromName(
					m_TargetNodeName.c_str(),this);

				if(pTarNode)
				{

					switch(m_status)
					{
					case 0:
						{
							int i;

							for(i=0;i<pTarNode->getJointCount();i++)
							{
								irr::scene::IBoneSceneNode *pBone =
									pTarNode->getJointNode(i);
							}
							
							changeAction(m_CurAnimation);

							m_status = 1;
						}
						break;
					}
				}*/
				
				ISceneNode::OnAnimate(timeMs);
			}

			void CFormatedAnimationNode::changeAction(std::string ActionName)
			{
				irr::scene::IAnimatedMeshSceneNode *pAni = 
					(irr::scene::IAnimatedMeshSceneNode *)SceneManager->getSceneNodeFromName(
					m_TargetNodeName.c_str(),this);

				
				if(pAni)
				{
					pAni->setAnimationSpeed(m_mapActionDesc[ActionName].Z);
					pAni->setFrameLoop(
						(irr::s32)m_mapActionDesc[ActionName].X,
						(irr::s32)m_mapActionDesc[ActionName].Y
						);					
				}					
			}
		}
	}
}
