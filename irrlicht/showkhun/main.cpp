

#include "CilryhwaApp.h"

/*
ÀÏ¸®È÷Æ® ¿£Áø¿ë ¿¡µðÅÍ ìé×âü£ 

*/




void main()
{
	irr::IrrlichtDevice *pDevice = irr::createDevice(					
		irr::video::EDT_DIRECT3D9
		);	

	CiLryhwaApp App;
	
	App.Init(pDevice);

	pDevice->setEventReceiver(&App);

	pDevice->setWindowCaption(L"hello irrlicht 3d engine");

	irr::video::IVideoDriver *pVideo = pDevice->getVideoDriver();
	irr::scene::ISceneManager *pSmgr = pDevice->getSceneManager();
	irr::gui::IGUIEnvironment *pGuiEnv = pDevice->getGUIEnvironment();	

	while(pDevice->run())
	{		
		static irr::u32 uLastTick=0;
		irr::u32 uTick = pDevice->getTimer()->getTime();						
		irr::f32 fDelta = ((float)(uTick - uLastTick)) / 1000.f; //µ¨Å¸°ª ±¸ÇÏ±â
		uLastTick = uTick;

		App.Update(fDelta);
		App.Render();
	}

	pDevice->drop();
}