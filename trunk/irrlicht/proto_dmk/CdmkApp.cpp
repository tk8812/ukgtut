#include "CdmkApp.h"




CdmkApp::CdmkApp(void)
:m_pDevice(0),
m_pVideo(0),
m_pSmgr(0),
m_pGuiEnv(0)	
{
	::_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF); //메모리릭 탐지
	//메모리릭 발생원인 탐지.
	//_CrtSetBreakAlloc(145);
}


CdmkApp::~CdmkApp(void)
{
	m_pDevice->drop();
}

bool CdmkApp::Init()
{
	m_pDevice = irr::createDevice(
		irr::video::EDT_DIRECT3D9,
		irr::core::dimension2du(640,480), //1.6버전용
		32,
		false, false, false,
		this
		);
	m_pVideo = m_pDevice->getVideoDriver();
	m_pSmgr = m_pDevice->getSceneManager();
	m_pGuiEnv = m_pDevice->getGUIEnvironment();	

	//사용자 추가 확장 펙토리등록
	{
		irr::scene::jz3d::CJZ3DSceneNodeFactory *factory =
			new irr::scene::jz3d::CJZ3DSceneNodeFactory(m_pSmgr);
		m_pSmgr->registerSceneNodeFactory(factory);
		factory->drop();
	}

	//리소스폴더 지정
	m_pDevice->getFileSystem()->changeWorkingDirectoryTo("../.");

	//초기화 코드 삽입...	
	m_pSmgr->loadScene("../res/proto_dmk/stage0.irr");

	m_pSmgr->loadScene("../res/proto_dmk/object.irr");

	{
		irr::scene::ISceneNode *pNode = m_pSmgr->getSceneNodeFromName("usr/obj/b3d/ninja/white");
		pNode->setVisible(true);

		irr::scene::IAnimatedMeshSceneNode *pBodyNode = (irr::scene::IAnimatedMeshSceneNode *)m_pSmgr->getSceneNodeFromName("body",pNode);		
		pBodyNode->setFrameLoop(184,205); //대기상태
	}



	//프레임 레이트 표시용 유아이
	irr::gui::IGUIStaticText *pstextFPS = 
		m_pGuiEnv->addStaticText(L"Frame rate",
		irr::core::rect<irr::s32>(0,0,100,20),true,true,0,100,true);

	//씬그라프 전체 리프레쉬
	m_pSmgr->getRootSceneNode()->OnAnimate(0);

	return true;

}

bool CdmkApp::OnEvent(const irr::SEvent &event)
{

	//모든 애니메이터들에게 이밴트를 전달한다.
	if(m_pSmgr)
	{
		ggf::utils::RecursiveEvent(m_pSmgr->getRootSceneNode(),event);
	}

	switch(event.EventType)
	{
	case irr::EET_GUI_EVENT:
		{

		}
		break;
	case irr::EET_KEY_INPUT_EVENT:
		{
		}
		break;
	case irr::EET_MOUSE_INPUT_EVENT:
		{

		}
		break;
	case irr::EET_USER_EVENT:
		break;
	}
	return false;
}

void CdmkApp::Update()
{
	//프레임레이트 갱신
	{
		wchar_t wszbuf[256];
		swprintf(wszbuf,L"Frame rate : %d\n TriAngle: %d",m_pVideo->getFPS(),
			m_pVideo->getPrimitiveCountDrawn());			
		m_pGuiEnv->getRootGUIElement()->getElementFromId(100)->setText(wszbuf);
	}

	//게임로직 & 물리 & 구충돌처리
	{
		
	}
}

void CdmkApp::Render()
{
	m_pVideo->beginScene(true, true, irr::video::SColor(255,0,0,0));

	m_pSmgr->drawAll();
	m_pGuiEnv->drawAll();

	m_pVideo->endScene();
}
