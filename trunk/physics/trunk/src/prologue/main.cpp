#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <algorithm>

#include <tchar.h>
#include <comutil.h>


#include <irrlicht.h>
#include <windows.h>
#include <string>

#include <d3d9.h>
#pragma warning( disable : 4996 ) // disable deprecated warning 
#include <strsafe.h>
#pragma warning( default : 4996 ) 


#include "Singleton.h"
#include "irrbaseApp.h"

#include "TutApp5.h"


int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{	
	std::tr1::shared_ptr<ggf::irr_base::IBaseApp> spApp;
	spApp = std::tr1::shared_ptr<CTutApp5>(new CTutApp5);	

	spApp->Init(L"irr+dx sample");
	
	MSG msg;
	ZeroMemory( &msg, sizeof(msg) );	

	bool quit = false;
	irr::u32 uLastTick = spApp->m_pDevice->getTimer()->getTime();
	while (!quit)
	{
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		

			if (msg.message == WM_QUIT)
				quit = true;
		}

		//일리히트 자체루프
		irr::f32 fTick =  (float)(spApp->m_pDevice->getTimer()->getTime() - uLastTick) / 1000.f;
		uLastTick = spApp->m_pDevice->getTimer()->getTime();		

		if(spApp->Apply(fTick) && !quit) //이렇게 해야 dxlost 에러가 발생하지않는다.
			spApp->Render(fTick);			
		else
			break;
	}
	spApp->Close();	

	return 0;	

}