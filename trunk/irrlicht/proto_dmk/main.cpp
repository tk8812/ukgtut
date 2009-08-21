#include <iostream>
#include <string>

#include <irrlicht.h>
#include "cdmkapp.h"

void main()
{
	static CdmkApp App;

	App.Init();
	App.gameloop();
}