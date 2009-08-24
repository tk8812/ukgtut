#include "CollusionMngPluginNode.h"


namespace irr 
{
	namespace scene
	{
		namespace jz3d 
		{

			const c8* CCollusionMngNode::CollShapeNames[] =			
			{
				"sphere",
				"box",
				"cylinder",
				"capsule",
				0
			};	

			const char *CCollusionMngNode::Name  = "CCollusionMngNode";

			CCollusionMngNode::CCollusionMngNode(ISceneNode* parent, ISceneManager* mgr, s32 id)
				: ISceneNode(parent,mgr,id),
				m_strBone(""),
				m_strShape(CollShapeNames[0])
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

				if ( DebugDataVisible & scene::EDS_BBOX )
				{
					if(Parent)
					{
						if(Parent->getType() == ESNT_ANIMATED_MESH)
						{
							irr::scene::IAnimatedMeshSceneNode *pNode = (irr::scene::IAnimatedMeshSceneNode *)Parent;
							irr::core::stringc strc =  m_strBone.c_str();
							irr::scene::IBoneSceneNode *pBone = pNode->getJointNode(strc.c_str());
							if(pBone)
							{
								if(m_GeometryInfo.type == CBPAGT_BOX)
								{
									irr::core::vector3df box_size =
										irr::core::vector3df(m_GeometryInfo.box.X ,m_GeometryInfo.box.Y ,m_GeometryInfo.box.Z );

									irr::core::aabbox3df box(
										//irr::core::vector3df(-1,-1,-1), 
										//irr::core::vector3df(1,1,1)
										-(box_size/2),
										(box_size/2)
										);								

									irr::core::matrix4 mat;
									//Parent->getAbsoluteTransformation().getInverse(mat);
									//mat = getAbsoluteTransformation() * mat;//로컬변환구하기
									mat = pBone->getAbsoluteTransformation() * getRelativeTransformation();

									driver->setTransform(video::ETS_WORLD,mat);

									{
										irr::video::SMaterial m;
										m.Lighting = false;
										m.ZWriteEnable = true;										
										driver->setMaterial(m);
									}

									driver->draw3DBox(box,irr::video::SColor(255,255,0,0));
								}
							}
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
				nb->m_strBone = m_strBone;
				nb->m_strShape = m_strShape;
				nb->m_GeometryInfo = m_GeometryInfo;

				nb->drop();
				return nb;
			}		


			void CCollusionMngNode::serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options) const
			{
				ISceneNode::serializeAttributes(out, options);
				
				//데이터 쓰기 코드추가
				//...
				out->addString("bone",m_strBone.c_str());
				out->addEnum("Shape",
					irr::core::stringc(m_strShape.c_str()).c_str(),
					CCollusionMngNode::CollShapeNames
					);


				if(m_strShape == L"sphere")
				{
					out->addFloat("radius",m_GeometryInfo.sphere.radius);
				}
				if(m_strShape == L"box")
				{
					out->addVector3d("box_size",irr::core::vector3df(m_GeometryInfo.box.X ,m_GeometryInfo.box.Y ,m_GeometryInfo.box.Z ));
				}

				//out->addEnum("bones",0,(char **)&m_BoneList[0]);

			}

			//! Reads attributes of the scene node.
			void CCollusionMngNode::deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options)
			{	
				//데이터 읽기 코드추가
				//...

				m_strBone = in->getAttributeAsStringW("bone");
				m_strShape = irr::core::stringw(in->getAttributeAsEnumeration("Shape"));


				if(m_strShape == L"sphere")
				{
					m_GeometryInfo.sphere.radius = in->getAttributeAsFloat("radius");
					m_GeometryInfo.type = CBPAGT_SPHERE;
				}
				else
				{
					irr::core::vector3df box_size = in->getAttributeAsVector3d("box_size");
					m_GeometryInfo.box.X = box_size.X;
					m_GeometryInfo.box.Y = box_size.Y;
					m_GeometryInfo.box.Z = box_size.Z;
					m_GeometryInfo.type = CBPAGT_BOX;
				}

				ISceneNode::deserializeAttributes(in, options);				
			}			
		}
	}
}
