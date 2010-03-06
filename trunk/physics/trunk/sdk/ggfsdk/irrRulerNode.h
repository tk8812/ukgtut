#pragma once

//2006.8.7 최초작성

namespace irr
{
namespace scene
{	
namespace scene_ggf
{
	
	class CRulerSceneNode : public irr::scene::ISceneNode
	{
	protected:		
		irr::video::SMaterial Material;
		irr::core::aabbox3d<irr::f32> Box;

	public:		
		CRulerSceneNode(irr::scene::ISceneNode* parent, irr::scene::ISceneManager* mgr, irr::s32 id)
			: irr::scene::ISceneNode(parent, mgr, id)
		{ 
			Material.Wireframe = false;
			Material.Lighting = false;		
			
		}		

		~CRulerSceneNode()
		{

		}	

		//virtual void OnPreRender()
		virtual void OnRegisterSceneNode()
		{  
			if (IsVisible)
			{
				//왜 이렇게 하냐하면..투명기능이있는 면과 그렇지않은면이있을때
				//이런식으로 구분시키지않으면 알파값이 이상하게나올수있음
				
				irr::video::IVideoDriver* driver = SceneManager->getVideoDriver();

				irr::video::IMaterialRenderer* rnd = 
						driver->getMaterialRenderer(Material.MaterialType);
				if(rnd && rnd->isTransparent())
				{
					SceneManager->registerNodeForRendering(this, scene::ESNRP_TRANSPARENT);
				}
				else
				{
					SceneManager->registerNodeForRendering(this, scene::ESNRP_SOLID);
				}
			}

			ISceneNode::OnRegisterSceneNode();
			//OnPreRender();			
		}

		virtual void render()
		{  				
			irr::video::IVideoDriver* driver = SceneManager->getVideoDriver();

			driver->setMaterial(Material);			

			irr::core::matrix4 _TempTrans;
			_TempTrans = driver->getTransform(irr::video::ETS_WORLD);

			driver->setTransform(irr::video::ETS_WORLD, AbsoluteTransformation);

			{
				
				driver->draw3DLine(irr::core::vector3df(0,0,-250),irr::core::vector3df(0,0,250)
					,irr::video::SColor(255,255,255,255));

				driver->draw3DLine(irr::core::vector3df(-250,0,0),irr::core::vector3df(250,0,0)
					,irr::video::SColor(255,255,255,255));

				driver->draw3DLine(irr::core::vector3df(0,-250,0),irr::core::vector3df(0,250,0)
					,irr::video::SColor(255,255,255,255));

				int i;			
				irr::f32 fRuler = -250;
				for(i=0; i < 10;i++)
				{

					driver->draw3DLine(
						irr::core::vector3df(-10,0,fRuler),
						irr::core::vector3df(10,0,fRuler)
						,irr::video::SColor(255,255,255,255)
						);
					driver->draw3DLine(
						irr::core::vector3df(-10,fRuler,0),
						irr::core::vector3df(10,fRuler,0)
						,irr::video::SColor(255,255,255,255)
						);
					driver->draw3DLine(
						irr::core::vector3df(fRuler,0,-10),
						irr::core::vector3df(fRuler,0,10)
						,irr::video::SColor(255,255,255,255)
						);
					fRuler = fRuler + 50;
				}
			}


			
			driver->setTransform(irr::video::ETS_WORLD,_TempTrans);		//월드변환복귀	
		}

		/*
		virtual void OnPostRender(irr::u32 timeMS)
		{			
			ISceneNode::OnPostRender(timeMS);
		}
		*/

		virtual const irr::core::aabbox3d<irr::f32>& getBoundingBox() const  
		{    
			return Box;  
		}

		//virtual irr::u32 getMaterialCount()
		virtual irr::u32 getMaterialCount() const		
		{
			return 1;
		}
		virtual irr::video::SMaterial& getMaterial(irr::s32 i)
		{
			return Material;
		}

		irr::video::ITexture *getMatrialTexture()
		{
			//return Material.Textures[0];
			return getMaterial(0).getTexture(0);
		}


	};



}
}
}