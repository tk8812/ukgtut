#pragma once

class CMainMenuFrame:
	public CEsenFrame
{
public:
	
	enum {
		PH_MAINMENU,
		PH_STAGESELECT,
		PH_LOGIN		
	};

	virtual void Init();
	virtual void Release();
	virtual void Apply(irr::u32 timeMs);
	virtual void Render();
	virtual void LoadRes();
	virtual bool OnEvent(irr::SEvent ev);
	virtual void DecodePacket(irr::s32 nCode,void *pBuf)
	{
	}
	

	bool m_bTabPressed;


	CMainMenuFrame(void);
	virtual ~CMainMenuFrame(void);
	
	CMainMenuFrame(LPCSTR strName):CEsenFrame(strName)
	{		
				
	}
};
