#include <iostream>
#include <string>

#include <irrlicht.h>
#include "cdmkapp.h"

/*

필수 공유 모듈 

일리히트엔진 1.5.1
위치 : sdk/irrlicht

블릿엔진 2.5 rc7
위치 : sdk/bullet


*/

void main()
{
	static CdmkApp App;

	App.Init();
	App.gameloop();
}