#pragma once

#include <irrlicht.h>
#include <string>
#include <map>

namespace irr 
{
	namespace scene
	{
		namespace jz3d 
		{

			class CFormatedAnimationNode : public irr::scene::ISceneNode
			{
			public:				

				static const int TypeID = MAKE_IRR_ID('@','F','A','N');
				static const char *Name;
				static const c8* ActionNames[];
				static const c8* CmdListNames[];

				//! constructor
				CFormatedAnimationNode(ISceneNode* parent, ISceneManager* mgr, s32 id);

				//! returns the axis aligned bounding box of this node
				virtual const core::aabbox3d<f32>& getBoundingBox() const;

				//! This method is called just before the rendering process of the whole scene.
				virtual void OnRegisterSceneNode();

				//! does nothing.
				virtual void render();
				//virtual irr::u32 getMaterialCount() const {return 1;}

				//! Returns type of the scene node
				virtual ESCENE_NODE_TYPE getType() const { return (ESCENE_NODE_TYPE)TypeID; }

				//! Creates a clone of this scene node and its children.
				virtual ISceneNode* clone(ISceneNode* newParent=0, ISceneManager* newManager=0);

				//! Writes attributes of the scene node.
				virtual void serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options=0) const;

				//! Reads attributes of the scene node.
				virtual void deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options=0);

				void changeAction(std::string ActionName);

				virtual void OnAnimate(u32 timeMs);

			private:

				irr::core::stringc m_TargetNodeName;
				std::string m_CurAnimation;							
				std::string m_CurCmd;
				std::map<std::string,irr::core::vector3df> m_mapActionDesc;

				irr::s32 m_status;


				//std::string m_strAddActionName;
				//bool m_bAddActionBtn;
				//irr::core::array<irr::core::stringw> m_strActionList;
				//char **m_EnumActionList;

				core::aabbox3d<f32> Box;
			};
		}
	}
}
