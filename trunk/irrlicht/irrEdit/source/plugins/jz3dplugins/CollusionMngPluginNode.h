#pragma once

#include <irrlicht.h>

#include "CBulletObjectAnimator.h"

namespace irr 
{
	namespace scene
	{
		namespace jz3d 
		{

			class CCollusionMngNode : public ISceneNode
			{
			public:

				static const c8 *CollShapeNames[];

				CBulletObjectAnimatorGeometry m_GeometryInfo;

				core::aabbox3d<f32> Box;
				irr::core::stringw m_strBone;
				irr::core::stringw m_strShape;

				//irr::core::array<char *> m_BoneList;

				static const int TypeID = MAKE_IRR_ID('@','C','L','M');
				static const char *Name;

				CCollusionMngNode(ISceneNode* parent, ISceneManager* mgr, s32 id);				

				//! returns the axis aligned bounding box of this node
				virtual const core::aabbox3d<f32>& getBoundingBox() const;

				//! This method is called just before the rendering process of the whole scene.
				virtual void OnRegisterSceneNode();
				
				virtual void render();

				//! Returns type of the scene node
				virtual ESCENE_NODE_TYPE getType() const { return (ESCENE_NODE_TYPE)TypeID; }

				//! Creates a clone of this scene node and its children.
				virtual ISceneNode* clone(ISceneNode* newParent=0, ISceneManager* newManager=0);

				//! Writes attributes of the scene node.
				virtual void serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options=0) const;

				//! Reads attributes of the scene node.
				virtual void deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options=0);

				////! Updates the absolute position based on the relative and the parents position
				////본이 존재하면 본의 변환을 상속받는다.
				//inline virtual void updateAbsolutePosition()
				//{					
				//	if (Parent)
				//	{
				//		if(Parent->getType() == ESNT_ANIMATED_MESH)
				//		{
				//			irr::scene::IAnimatedMeshSceneNode *pNode = (irr::scene::IAnimatedMeshSceneNode *)Parent;
				//			irr::core::stringc strc =  m_strBone.c_str();
				//			irr::scene::IBoneSceneNode *pBone = pNode->getJointNode(strc.c_str());
				//			if(pBone)
				//			{
				//				AbsoluteTransformation =
				//					pBone->getAbsoluteTransformation() * getRelativeTransformation();
				//			}
				//			else
				//			{
				//				AbsoluteTransformation =
				//					Parent->getAbsoluteTransformation() * getRelativeTransformation();
				//			}
				//		}
				//		else
				//		{
				//			AbsoluteTransformation =
				//				Parent->getAbsoluteTransformation() * getRelativeTransformation();
				//		}
				//	}
				//	else
				//		AbsoluteTransformation = getRelativeTransformation();
				//}
				
			};		
		}
	}
}
