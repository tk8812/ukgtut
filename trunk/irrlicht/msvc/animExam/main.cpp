#include <irrlicht.h>

#include <windows.h>


#include "../../irrEdit/source/plugins/jz3dplugins/FormatedAnimationNode.h"
#include "../../irrEdit/source/plugins/jz3dplugins/JZ3DSceneNodeFactory.h"



void main()
{
	irr::IrrlichtDevice *pDevice = irr::createDevice(     
		irr::video::EDT_DIRECT3D9
		);
	irr::video::IVideoDriver *pVideo = pDevice->getVideoDriver();
	irr::scene::ISceneManager *pSmgr = pDevice->getSceneManager();
	irr::gui::IGUIEnvironment *pGuiEnv = pDevice->getGUIEnvironment();

	//사용자 추가 확장 펙토리등록
	{
		irr::scene::jz3d::CJZ3DSceneNodeFactory *factory =
			new irr::scene::jz3d::CJZ3DSceneNodeFactory(pSmgr);
		pSmgr->registerSceneNodeFactory(factory);
		factory->drop();	
	}	

	pDevice->getFileSystem()->changeWorkingDirectoryTo("../../res");

	pSmgr->addCameraSceneNode(0,irr::core::vector3df(0,5,-15),irr::core::vector3df(0,0,0));

	pSmgr->loadScene("proto_dmk/obj_ninja.irr");

	{
		irr::scene::jz3d::CFormatedAnimationNode *pFANode = 
			(irr::scene::jz3d::CFormatedAnimationNode *)pSmgr->getSceneNodeFromName("usr/obj/b3d/ninja/blue");

		pFANode->changeAction("kick");
	}
	
	
	//프레임 레이트 표시용 유아이
	irr::gui::IGUIStaticText *pstextFPS = 
		pGuiEnv->addStaticText(L"Frame rate",irr::core::rect<irr::s32>(0,0,100,20),true,true,0,100,true);

	while(pDevice->run())
	{ 
		static irr::u32 uLastTick=0;
		irr::u32 uTick = pDevice->getTimer()->getTime();      
		irr::f32 fDelta = ((float)(uTick - uLastTick)) / 1000.f; //델타값 구하기
		uLastTick = uTick;

		//프레임레이트 갱신, 삼각형수 표시
		{
			wchar_t wszbuf[256];
			swprintf(wszbuf,L"Frame rate : %d\n TriAngle: %d",pVideo->getFPS(),pVideo->getPrimitiveCountDrawn());
			pstextFPS->setText(wszbuf);
		}

		pVideo->beginScene(true, true, irr::video::SColor(255,100,101,140));

		pSmgr->drawAll();
		pGuiEnv->drawAll();  

		pVideo->endScene(); 
	}

	pDevice->drop();
}

