//#include "StdAfx.h"
#include "bycle.h"
#include "MainMenuFrame.h"

CMainMenuFrame::CMainMenuFrame(void)
{
}

CMainMenuFrame::~CMainMenuFrame(void)
{
}


void CMainMenuFrame::Init()
{
	m_bEnable = true;
	m_bTabPressed = false;
	if(CBycleApp::GetPtr()->m_strUserID == "")
	{
		SetPhase(PH_LOGIN,NULL);
	}
	else
	{
		SetPhase(CMainMenuFrame::PH_MAINMENU,NULL);
	}
	LoadRes();
}

void CMainMenuFrame::LoadRes()
{

	CBycleApp::Get().m_System.m_pSmgr->clear();

	irr::video::IImage *pImg;
	irr::core::stringc str;	

	str = CBycleApp::GetPtr()->m_System.m_ConfigData.m_strResFolder.c_str();
	str += "/texture/main_back.jpg";
	
	pImg = CBycleApp::Get().m_System.m_pDriver->createImageFromFile(str.c_str());
	if(pImg)
	{
		CBycleApp::Get().m_System.m_pDriver->addTexture("main_back",pImg);
		ggf::irr_util::DebugOutputFmt(NULL,"load success....%s \n",str.c_str());
	}
	else
	{
		ggf::irr_util::DebugOutputFmt(NULL,"load failed....%s \n",str.c_str());
	}
	pImg->drop();
}

void CMainMenuFrame::Release()
{
}

bool CMainMenuFrame::OnEvent(irr::SEvent ev)
{
	switch(ev.EventType)
	{
	case irr::EET_KEY_INPUT_EVENT:
		{
			irr::gui::IGUIEnvironment *pEnv = CBycleApp::Get().m_System.m_pDevice->getGUIEnvironment();	
			
			switch(ev.KeyInput.Key)
			{
			case irr::KEY_TAB:
				//m_bTabPressed = true;
				break;
			case irr::KEY_RETURN:
				{
					irr::gui::IGUIEditBox *pEdit = (irr::gui::IGUIEditBox *)pEnv->getRootGUIElement()->getElementFromId(GUIID_LOGIN_EDIT,true);
					irr::gui::IGUIEditBox *pEditPass = (irr::gui::IGUIEditBox *)pEnv->getRootGUIElement()->getElementFromId(GUIID_LOGIN_EDIT_PASS,true);

					if(pEdit && pEditPass)
					{
						if(CBycleApp::GetPtr()->ProcessLogin(pEdit->getText(),pEditPass->getText()) )
						{
							CBycleApp::GetPtr()->hideAllMenu();
							SetPhase(PH_MAINMENU,0);
						}
					}
				}
				break;
			}
		}
		break;
	case irr::EET_GUI_EVENT:
		{
			irr::gui::IGUIEnvironment *pEnv = CBycleApp::Get().m_System.m_pDevice->getGUIEnvironment();	
			int hID = ev.GUIEvent.Caller->getID();
			switch(ev.GUIEvent.EventType)
			{			
			case irr::gui::EGET_BUTTON_CLICKED:	
				{
					switch(hID)
					{
					case GUIID_BTN_START:
						{
							CBycleApp::GetPtr()->hideAllMenu();
							pEnv->getRootGUIElement()->getElementFromId(GUIID_STAGE_SELECT)->setVisible(true);
						}
						break;
					case GUIID_BTN_EXIT: 
						{
							CBycleApp::Get().hideAllMenu();		
							//로그인창으로 나가기
							pEnv->getRootGUIElement()->getElementFromId(GUIID_LOGIN)->setVisible(true);
							
						}
						break;
					case GUIID_LOGIN_CANCEL://종료하기
						{
							CBycleApp::Get().m_System.m_pDevice->closeDevice();
						}
						break;
					case GUIID_BTN_MULTI:
						{
							
						}
						break;						
					case GUIID_BTN_STAGE_SELECT_BACK: 
						{
							CBycleApp::Get().hideAllMenu();
							pEnv->getRootGUIElement()->getElementFromId(GUIID_GAMEMENU)->setVisible(true);//메인메뉴로
						}
						break;
					case GUIID_LOGIN_OK:
						{
							{
								irr::gui::IGUIListBox *pLb = (irr::gui::IGUIListBox *)pEnv->getRootGUIElement()->getElementFromId(GUIID_LOGIN_USERLIST,true);
								if(pLb->getSelected() >= 0)
								{
									CBycleApp::Get().ProcessLogin(pLb->getListItem(pLb->getSelected()),L"");				
									CBycleApp::Get().hideAllMenu();
									SetPhase(PH_MAINMENU,0);
								}
							}							
						}
						break;
					case GUIID_BTN_GAMESTART:
						{
							irr::gui::IGUIListBox *pLb = (irr::gui::IGUIListBox *)pEnv->getRootGUIElement()->getElementFromId(GUIID_LIST_STAGE_SELECT,true);
							if(pLb->getSelected() >= 0)
							{
								ggf::irr_util::DebugOutputFmt(NULL,"stage sel: %d %s\n",pLb->getSelected(),
									CBycleApp::GetPtr()->m_System.m_ConfigData.m_vtStageRes[pLb->getSelected()].m_strName.c_str());							
								//게임 스타트
								CBycleApp::GetPtr()->m_nStage = pLb->getSelected();
								CBycleApp::GetPtr()->m_System.m_pFrameMng->SetActiveFrame("frame_game");
							}
						}
						break;
					}
				}
				break;
			case irr::gui::EGET_ELEMENT_HOVERED:
				{		
					
				}
				break;
			case irr::gui::EGET_ELEMENT_LEFT:
				{
					
				}
				break;
			case irr::gui::EGET_CHECKBOX_CHANGED:
				{
					
				}
				break;
			}
		}//gui event
		break;
	}
	return true;
}

void CMainMenuFrame::Apply(irr::u32 timeMs)
{
	if(!m_bEnable) return;
	irr::gui::IGUIEnvironment *pEnv = CBycleApp::Get().m_System.m_pDevice->getGUIEnvironment();	

	switch(m_nPhase)
	{
	case PH_LOGIN:
		switch(m_nStep)
		{
		case 0:
			{
				CBycleApp::Get().hideAllMenu();
				pEnv->getRootGUIElement()->getElementFromId(GUIID_LOGIN)->setVisible(true);
				pEnv->setFocus(pEnv->getRootGUIElement()->getElementFromId(GUIID_LOGIN_EDIT,true));
				m_nStep = 10;
			}
			break;
		case 10:
			if(m_bTabPressed)
			{
				m_bTabPressed = false;
				pEnv->setFocus(pEnv->getRootGUIElement()->getElementFromId(GUIID_LOGIN_EDIT_PASS,true));
			}
			m_nStep++;
			break;
		case 11:
			if(m_bTabPressed)
			{
				m_bTabPressed = false;
				pEnv->setFocus(pEnv->getRootGUIElement()->getElementFromId(GUIID_LOGIN_EDIT,true));
			}
			m_nStep = 10;
			break;
		}
		break;
	case PH_MAINMENU:
		switch(m_nStep)
		{
		case 0:
			{
				CBycleApp::Get().hideAllMenu();
				pEnv->getRootGUIElement()->getElementFromId(GUIID_GAMEMENU)->setVisible(true);
				//pEnv->getRootGUIElement()->getElementFromId(GUIID_STAGE_SELECT)->setVisible(false);

				m_nStep=10;
			}
			break;
		case 10: //메뉴샌타꾸대기
			break;
		}
		break;
	case PH_STAGESELECT:
		break;
	}
}

void CMainMenuFrame::Render()
{
	if(!m_bEnable) return;

	CBycleApp::Get().m_System.m_pSmgr->drawAll();
	CBycleApp::Get().m_System.m_pDriver->draw2DImage(CBycleApp::Get().m_System.m_pDriver->getTexture("main_back"),irr::core::position2d<irr::s32>(0,0));
	CBycleApp::Get().m_System.m_pDevice->getGUIEnvironment()->drawAll();			

	switch(m_nPhase)
	{
	case PH_LOGIN:
		break;
	case PH_MAINMENU:
		break;
	case PH_STAGESELECT:
		break;
	}
}

