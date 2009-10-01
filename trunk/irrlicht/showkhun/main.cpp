#include <irrlicht.h>
#include <math.h>
#include <iostream>
#include <vector>

#include <windows.h>

#include <comutil.h>
#pragma comment (lib,"comsuppw.lib" )
#pragma comment (lib,"irrlicht.lib" )



using namespace irr;
using namespace gui;

enum
{
	GUI_ID_DIALOG_ROOT_WINDOW  = 0x10000,

	GUI_ID_X_SCALE,
	GUI_ID_Y_SCALE,
	GUI_ID_Z_SCALE,

	GUI_ID_OPEN_MODEL,
	GUI_ID_SET_MODEL_ARCHIVE,
	GUI_ID_LOAD_AS_OCTREE,

	GUI_ID_SKY_BOX_VISIBLE,
	GUI_ID_TOGGLE_DEBUG_INFO,

	GUI_ID_DEBUG_OFF,
	GUI_ID_DEBUG_BOUNDING_BOX,
	GUI_ID_DEBUG_NORMALS,
	GUI_ID_DEBUG_SKELETON,
	GUI_ID_DEBUG_WIRE_OVERLAY,
	GUI_ID_DEBUG_HALF_TRANSPARENT,
	GUI_ID_DEBUG_BUFFERS_BOUNDING_BOXES,
	GUI_ID_DEBUG_ALL,

	GUI_ID_MODEL_MATERIAL_SOLID,
	GUI_ID_MODEL_MATERIAL_TRANSPARENT,
	GUI_ID_MODEL_MATERIAL_REFLECTION,

	GUI_ID_CAMERA_MAYA,
	GUI_ID_CAMERA_FIRST_PERSON,

	GUI_ID_POSITION_TEXT,

	GUI_ID_ABOUT,
	GUI_ID_QUIT,

	// And some magic numbers
	MAX_FRAMERATE = 1000,
	DEFAULT_FRAMERATE = 30
};




class CShowkhunApp : public IEventReceiver
{
public:	
	IrrlichtDevice *Device;
	bool Octree;
	scene::ISceneNode* Model;// = 0;
	scene::ISceneNode* SkyBox; //= 0;

	CShowkhunApp()
		: Octree(false),
		SkyBox(0),
		Model(0)
	{
	}

	virtual bool OnEvent(const SEvent& event)
	{
		// Escape swaps Camera Input
		if (event.EventType == EET_KEY_INPUT_EVENT &&
			event.KeyInput.PressedDown == false)
		{
			
		}

		if (event.EventType == EET_GUI_EVENT)
		{
			s32 id = event.GUIEvent.Caller->getID();
			IGUIEnvironment* env = Device->getGUIEnvironment();

			switch(event.GUIEvent.EventType)
			{
			case EGET_MENU_ITEM_SELECTED:
				{
					// a menu item was clicked

					IGUIContextMenu* menu = (IGUIContextMenu*)event.GUIEvent.Caller;
					s32 id = menu->getItemCommandId(menu->getSelectedItem());

					switch(id)
					{
					case GUI_ID_OPEN_MODEL: // File -> Open Model
						env->addFileOpenDialog(L"Please select a model file to open");
						break;
					case GUI_ID_SET_MODEL_ARCHIVE: // File -> Set Model Archive
						env->addFileOpenDialog(L"Please select your game archive/directory");
						break;
					case GUI_ID_LOAD_AS_OCTREE: // File -> LoadAsOctree
						Octree = !Octree;
						menu->setItemChecked(menu->getSelectedItem(), Octree);
						break;
					case GUI_ID_QUIT: // File -> Quit
						Device->closeDevice();
						break;
					case GUI_ID_SKY_BOX_VISIBLE: // View -> Skybox
						menu->setItemChecked(menu->getSelectedItem(), !menu->isItemChecked(menu->getSelectedItem()));
						SkyBox->setVisible(!SkyBox->isVisible());
						break;
					case GUI_ID_DEBUG_OFF: // View -> Debug Information
						menu->setItemChecked(menu->getSelectedItem()+1, false);
						menu->setItemChecked(menu->getSelectedItem()+2, false);
						menu->setItemChecked(menu->getSelectedItem()+3, false);
						menu->setItemChecked(menu->getSelectedItem()+4, false);
						menu->setItemChecked(menu->getSelectedItem()+5, false);
						menu->setItemChecked(menu->getSelectedItem()+6, false);
						if (Model)
							Model->setDebugDataVisible(scene::EDS_OFF);
						break;
					case GUI_ID_DEBUG_BOUNDING_BOX: // View -> Debug Information
						menu->setItemChecked(menu->getSelectedItem(), !menu->isItemChecked(menu->getSelectedItem()));
						if (Model)
							Model->setDebugDataVisible((scene::E_DEBUG_SCENE_TYPE)(Model->isDebugDataVisible()^scene::EDS_BBOX));
						break;
					case GUI_ID_DEBUG_NORMALS: // View -> Debug Information
						menu->setItemChecked(menu->getSelectedItem(), !menu->isItemChecked(menu->getSelectedItem()));
						if (Model)
							Model->setDebugDataVisible((scene::E_DEBUG_SCENE_TYPE)(Model->isDebugDataVisible()^scene::EDS_NORMALS));
						break;
					case GUI_ID_DEBUG_SKELETON: // View -> Debug Information
						menu->setItemChecked(menu->getSelectedItem(), !menu->isItemChecked(menu->getSelectedItem()));
						if (Model)
							Model->setDebugDataVisible((scene::E_DEBUG_SCENE_TYPE)(Model->isDebugDataVisible()^scene::EDS_SKELETON));
						break;
					case GUI_ID_DEBUG_WIRE_OVERLAY: // View -> Debug Information
						menu->setItemChecked(menu->getSelectedItem(), !menu->isItemChecked(menu->getSelectedItem()));
						if (Model)
							Model->setDebugDataVisible((scene::E_DEBUG_SCENE_TYPE)(Model->isDebugDataVisible()^scene::EDS_MESH_WIRE_OVERLAY));
						break;
					case GUI_ID_DEBUG_HALF_TRANSPARENT: // View -> Debug Information
						menu->setItemChecked(menu->getSelectedItem(), !menu->isItemChecked(menu->getSelectedItem()));
						if (Model)
							Model->setDebugDataVisible((scene::E_DEBUG_SCENE_TYPE)(Model->isDebugDataVisible()^scene::EDS_HALF_TRANSPARENCY));
						break;
					case GUI_ID_DEBUG_BUFFERS_BOUNDING_BOXES: // View -> Debug Information
						menu->setItemChecked(menu->getSelectedItem(), !menu->isItemChecked(menu->getSelectedItem()));
						if (Model)
							Model->setDebugDataVisible((scene::E_DEBUG_SCENE_TYPE)(Model->isDebugDataVisible()^scene::EDS_BBOX_BUFFERS));
						break;
					case GUI_ID_DEBUG_ALL: // View -> Debug Information
						menu->setItemChecked(menu->getSelectedItem()-1, true);
						menu->setItemChecked(menu->getSelectedItem()-2, true);
						menu->setItemChecked(menu->getSelectedItem()-3, true);
						menu->setItemChecked(menu->getSelectedItem()-4, true);
						menu->setItemChecked(menu->getSelectedItem()-5, true);
						menu->setItemChecked(menu->getSelectedItem()-6, true);
						if (Model)
							Model->setDebugDataVisible(scene::EDS_FULL);
						break;
					//case GUI_ID_ABOUT: // Help->About
					//	showAboutText();
					//	break;
					//case GUI_ID_MODEL_MATERIAL_SOLID: // View -> Material -> Solid
					//	if (Model)
					//		Model->setMaterialType(video::EMT_SOLID);
					//	break;
					//case GUI_ID_MODEL_MATERIAL_TRANSPARENT: // View -> Material -> Transparent
					//	if (Model)
					//		Model->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR);
					//	break;
					//case GUI_ID_MODEL_MATERIAL_REFLECTION: // View -> Material -> Reflection
					//	if (Model)
					//		Model->setMaterialType(video::EMT_SPHERE_MAP);
					//	break;

					//case GUI_ID_CAMERA_MAYA:
					//	setActiveCamera(Camera[0]);
					//	break;
					//case GUI_ID_CAMERA_FIRST_PERSON:
					//	setActiveCamera(Camera[1]);
					//	break;

					}
					break;
				}
			case EGET_FILE_SELECTED:
				{
					// load the model file, selected in the file open dialog
					IGUIFileOpenDialog* dialog =
						(IGUIFileOpenDialog*)event.GUIEvent.Caller;
					//loadModel(core::stringc(dialog->getFileName()).c_str());
					std::cout << irr::core::stringc(dialog->getFileName()).c_str() << std::endl;
					
				}
				break;

			case EGET_SCROLL_BAR_CHANGED:
				
				break;			
			}
		}

		return false;
	}
};



void main()
{
	irr::IrrlichtDevice *pDevice = irr::createDevice(					
		irr::video::EDT_DIRECT3D9
		);

	

	CShowkhunApp App;
	App.Device = pDevice;

	pDevice->setEventReceiver(&App);

	pDevice->setWindowCaption(L"hello irrlicht 3d engine");

	irr::video::IVideoDriver *pVideo = pDevice->getVideoDriver();
	irr::scene::ISceneManager *pSmgr = pDevice->getSceneManager();
	irr::gui::IGUIEnvironment *pGuiEnv = pDevice->getGUIEnvironment();

	pDevice->getFileSystem()->changeWorkingDirectoryTo("../../res/");

	pSmgr->addCameraSceneNode(0, irr::core::vector3df(0,8,-10), irr::core::vector3df(0,5,0));
	
	//irr::gui::IGUIStaticText *pstextFPS = 
	//	pGuiEnv->addStaticText(L"Frame rate",irr::core::rect<irr::s32>(0,0,100,20),true,true,0,100,true);


	irr::scene::ISkinnedMesh *pSkinnedMesh = 0;			

	{
		// set a nicer font

		irr::gui::IGUIEnvironment *env = pGuiEnv;

		IGUISkin* skin = env->getSkin();
		IGUIFont* font = env->getFont("fonthaettenschweiler.bmp");
		if (font)
			skin->setFont(font);

		// create menu
		gui::IGUIContextMenu* menu = env->addMenu();
		menu->addItem(L"File", -1, true, true);
		menu->addItem(L"View", -1, true, true);
		menu->addItem(L"Camera", -1, true, true);
		menu->addItem(L"Help", -1, true, true);

		gui::IGUIContextMenu* submenu;
		submenu = menu->getSubMenu(0);
		submenu->addItem(L"Open Model File & Texture...", GUI_ID_OPEN_MODEL);
		submenu->addItem(L"Set Model Archive...", GUI_ID_SET_MODEL_ARCHIVE);
		submenu->addItem(L"Load as Octree", GUI_ID_LOAD_AS_OCTREE);
		submenu->addSeparator();
		submenu->addItem(L"Quit", GUI_ID_QUIT);

		submenu = menu->getSubMenu(1);
		submenu->addItem(L"sky box visible", GUI_ID_SKY_BOX_VISIBLE, true, false, true);
		submenu->addItem(L"toggle model debug information", GUI_ID_TOGGLE_DEBUG_INFO, true, true);
		submenu->addItem(L"model material", -1, true, true );

		submenu = submenu->getSubMenu(1);
		submenu->addItem(L"Off", GUI_ID_DEBUG_OFF);
		submenu->addItem(L"Bounding Box", GUI_ID_DEBUG_BOUNDING_BOX);
		submenu->addItem(L"Normals", GUI_ID_DEBUG_NORMALS);
		submenu->addItem(L"Skeleton", GUI_ID_DEBUG_SKELETON);
		submenu->addItem(L"Wire overlay", GUI_ID_DEBUG_WIRE_OVERLAY);
		submenu->addItem(L"Half-Transparent", GUI_ID_DEBUG_HALF_TRANSPARENT);
		submenu->addItem(L"Buffers bounding boxes", GUI_ID_DEBUG_BUFFERS_BOUNDING_BOXES);
		submenu->addItem(L"All", GUI_ID_DEBUG_ALL);

		submenu = menu->getSubMenu(1)->getSubMenu(2);
		submenu->addItem(L"Solid", GUI_ID_MODEL_MATERIAL_SOLID);
		submenu->addItem(L"Transparent", GUI_ID_MODEL_MATERIAL_TRANSPARENT);
		submenu->addItem(L"Reflection", GUI_ID_MODEL_MATERIAL_REFLECTION);

		submenu = menu->getSubMenu(2);
		submenu->addItem(L"Maya Style", GUI_ID_CAMERA_MAYA);
		submenu->addItem(L"First Person", GUI_ID_CAMERA_FIRST_PERSON);

		submenu = menu->getSubMenu(3);
		submenu->addItem(L"About", GUI_ID_ABOUT);
	}

	

	while(pDevice->run())
	{		
		static irr::u32 uLastTick=0;
		irr::u32 uTick = pDevice->getTimer()->getTime();						
		irr::f32 fDelta = ((float)(uTick - uLastTick)) / 1000.f; //델타값 구하기
		uLastTick = uTick;

		static irr::f32 fFrame = 1;
		if(::GetAsyncKeyState('1') & 0x8000)
		{
			fFrame += fDelta * 10.f;
		}
		if(::GetAsyncKeyState('2') & 0x8000)
		{
			fFrame -= fDelta * 10.f;
		}

		/*if(::GetAsyncKeyState('3') & 0x8001 != 0x8001)
		{
			irr::gui::IGUIFileOpenDialog *fileOpenDlg = pGuiEnv->addFileOpenDialog();
		}*/

		pVideo->beginScene(true, true, irr::video::SColor(255,100,101,140));					

		pSmgr->drawAll();
		pGuiEnv->drawAll();	

		{	

			

			if(pSkinnedMesh)
			{
				pSkinnedMesh->animateMesh(fFrame,1);					
				pSkinnedMesh->skinMesh();//스키닝적용

				irr::scene::IMeshBuffer *pmb = pSkinnedMesh->getMeshBuffer(0);		//칼빼고 몸통만....			


				{//월드변환
					pVideo->setTransform(irr::video::ETS_WORLD,
						irr::core::IdentityMatrix); //변환초기화					
				}

				{
					irr::video::SMaterial m;
					m.Lighting = false; //라이트를꺼야색이제데로나온다.
					//m.ZBuffer = false;
					m.Wireframe = true;
					pVideo->setMaterial(m);
				}

				pVideo->drawMeshBuffer(pmb);
			}
		}

		pVideo->endScene();		
	}

	pDevice->drop();
}