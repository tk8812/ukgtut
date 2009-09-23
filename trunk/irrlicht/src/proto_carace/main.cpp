
#include "bycle.h"

CBycleApp theApp;

void main()
{
	
	//std::cout << "Loading [" << argv[1] << "] Bme Application" << std::endl;
	//switch(CBycleApp::Get().Create(argv[1]))// pApp->Init())
	switch(CBycleApp::Get().Create("../../res/proto_carace"))
	{	
	case 0:

		CBycleApp::Get().MainLoop();			
		break;
	case 1:
		//TRACE("system.xml 파싱에러\n");
		break;
	case 2:				
		//TRACE("디바이스에러 \n");
		break;
	case 3:
		//TRACE("네트웍 에러 \n");
		break;
	default:
		break;
	}
}
