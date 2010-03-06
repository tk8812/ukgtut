#pragma once


namespace ggf 
{
namespace irr2d
{

void Init(irr::video::IVideoDriver *pDriver);
void DrawTile(ggf::layer::CLayerMng &lm,std::string strTileName,irr::core::position2d<irr::s32> posDes,irr::video::SColor color,int nframeWidth,int nframeHeight,int nFrame);
void DrawTile(ggf::layer::CLayerMng &lm,std::string strTileName,irr::core::position2d<irr::s32> posDes,irr::video::SColor color = irr::video::SColor(255,255,255,255));
void DrawTile(ggf::layer::CLayerMng &lm,int nIndex,irr::core::position2d<irr::s32> posDes,irr::video::SColor color = irr::video::SColor(255,255,255,255));
void DrawSprite(ggf::layer::CLayerMng &lm,std::string strSprName,irr::video::SColor color= irr::video::SColor(255,255,255,255));

void DrawAniTile(ggf::layer::S_AniTileLayer *pTile,irr::video::ITexture *pTex,irr::core::position2d<irr::s32> posDes,int nFrame,irr::video::SColor color = irr::video::SColor(255,255,255,255) );
void DrawAniTile(ggf::layer::CLayerMng &lm,int nIndex,irr::core::position2d<irr::s32> posDes,int nFrame,irr::video::SColor color = irr::video::SColor(255,255,255,255) );
void DrawAniTile(ggf::layer::CLayerMng &lm,std::string strTileName,irr::core::position2d<irr::s32> posDes,int nFrame,irr::video::SColor color = irr::video::SColor(255,255,255,255) );


class CAnimation2D
{
	bool m_bEnable;
	irr::s32 m_nFrame;
	irr::s32 m_nTotalFrame;
	irr::u32 m_uTotalPlayTime;	
	
	irr::core::vector2di	m_v2Pos;
	irr::core::vector2di	m_v2RefPos;
	irr::s32				m_nPlayType;
	ggf::layer::S_AniTileLayer *m_pAniTile;
	irr::video::ITexture *m_pTex;	

	irr::u32 m_uBaseTick;
	irr::f32 m_fLinerStep; //선형보간변수

	irr::s32 m_nWork;
	irr::video::SColor m_colOverlay;

	
public:

	enum 
	{
		PLAY_STOP = 0,
		PLAY_LOOP,
		PLAY_ONESTOP,
		PLAY_ONESTOPHIDE
	};

	CAnimation2D()
	{
		m_bEnable = false;
	}
	~CAnimation2D()
	{
	}

	void Init(char *szAniTle,irr::video::IVideoDriver *pDriver,ggf::layer::CLayerMng &LayerMng,
		irr::core::vector2di pos = irr::core::vector2di(0,0),
		irr::u32 uTotalPlayTime = 500
		)
	{
		ggf::layer::S_AniTileLayer *pAniTile;
		ggf::layer::S_Texture *pTexDisc;
		irr::video::ITexture *pTex;	

		pAniTile = LayerMng.FindAniTile(std::string(szAniTle));
		pTexDisc = LayerMng.FindTexture(pAniTile->m_strTexName.c_str());
		pTex	 = pDriver->getTexture(pTexDisc->m_strName.c_str());	

		Init(pAniTile,pTex,pos,uTotalPlayTime);
	}

	/*
	void Init(ggf::layer::S_AniTileLayer *pAniTile,irr::core::vector2di pos,irr::u32 uTotalPlayTime)
	{
		//ggf::layer::S_AniTileLayer *pTile;
		ggf::layer::S_Texture *pTexDisc;
		irr::video::ITexture *pTex;	

		//pTile = CkogG5App::m_System.m_LayerMng.FindAniTile(std::string("ani_item_prize"));
		pTexDisc = CkogG5App::m_System.m_LayerMng.FindTexture(pAniTile->m_strTexName.c_str());
		pTex = CkogG5App::m_System.m_pDriver->getTexture(pTexDisc->m_strName.c_str());	

		Init(pTile,pTex,pos,uTotalPlayTime);
	}
	*/
	void Init(ggf::layer::S_AniTileLayer *pAniTile,irr::video::ITexture *pTex,irr::core::vector2di pos,irr::u32 uTotalPlayTime)
	{
		m_bEnable = true;
		m_pAniTile = pAniTile;
		m_pTex = pTex;

		m_v2Pos = pos;
		m_v2RefPos = irr::core::vector2di(pos.X,pos.Y);
		m_nTotalFrame		= m_pAniTile->m_nTotalFrame;
		m_uTotalPlayTime	= uTotalPlayTime;
		m_nPlayType = PLAY_STOP;
		m_nFrame = 0;
		m_colOverlay = irr::video::SColor(255,255,255,255);
	}

	void setOverlayColor(irr::video::SColor col)
	{
		m_colOverlay = col;
	}

	irr::video::SColor getOverlayColor()
	{
		return m_colOverlay;
	}


	void setPosition(irr::core::vector2di pos)
	{
		m_v2Pos = pos;
	}

	void setPosition(irr::core::vector2df pos)
	{
		m_v2Pos = irr::core::vector2di((irr::s32)pos.X,(irr::s32)pos.Y);
	}

	void setPosition(irr::core::position2d<irr::s32> pos)
	{
		m_v2Pos = irr::core::vector2di(pos.X,pos.Y);
	}

	/////////////////////////////////
	//참조정점을 이미지정중앙으로한다.
	void setCenterRefrencePosition()
	{
		m_v2RefPos = irr::core::vector2di(m_pAniTile->m_rectFrame.getWidth()/2,m_pAniTile->m_rectFrame.getHeight()/2);
	}

	void setRefrencePosition(irr::core::vector2di pos)
	{
		m_v2RefPos = pos;
	}

	void setFrame(irr::s32 nFrame)
	{
		m_nFrame = nFrame;
	}

	irr::s32 getTotalFrame()
	{
		return m_nTotalFrame;
	}

	irr::s32 getCurrentFrame()
	{
		return m_nFrame;
	}

	ggf::layer::S_AniTileLayer *getAniTile()
	{
		return m_pAniTile;
	}

	void setVisiable(bool bVisible)
	{
		m_bEnable = bVisible;
	}

	void startAnimation(irr::u32 uStatTick,irr::s32 nPlayType)
	{

		m_nPlayType = nPlayType;
		m_nWork = 0;

		m_uBaseTick = uStatTick;

		m_fLinerStep = (irr::f32)(m_nTotalFrame - 0) / (irr::f32)m_uTotalPlayTime;
	}

	////////////////////
	//애니메이션을멈춘다
	//@nFrame 멈춰있을프레임 -1이면 현재프레임
	void stopAnimation(irr::s32 nFrame = -1)
	{
		m_nPlayType = PLAY_STOP;
		if(nFrame >= 0)
			m_nFrame = nFrame;
	}

	void Apply(irr::u32 uTick)
	{	
		if(m_bEnable == false)
			return;

		switch(m_nPlayType)
		{
		case PLAY_STOP:
			break;
		case PLAY_LOOP:			
			{
				irr::u32 uDelta = uTick - m_uBaseTick;	

				m_nFrame = (irr::s32)(m_fLinerStep * uDelta);

				m_nFrame %= m_nTotalFrame;
			}
			break;
		case PLAY_ONESTOP:
			{
				switch(m_nWork)
				{
				case 0:
					{
						irr::u32 uDelta = uTick - m_uBaseTick;	

						m_nFrame = (irr::s32)(m_fLinerStep * uDelta);

						if(m_nFrame >= m_nTotalFrame)
						{
							m_nFrame = m_nTotalFrame-1;
							m_nWork = 10;
						}
					}
					break;
				case 10:					
					break;
				}
								
			}
			break;
		case PLAY_ONESTOPHIDE:
			{
				switch(m_nWork)
				{
				case 0:
					{
						irr::u32 uDelta = uTick - m_uBaseTick;	

						m_nFrame = (irr::s32)(m_fLinerStep * uDelta);

						if(m_nFrame >= m_nTotalFrame)
						{
							m_nFrame = m_nTotalFrame-1;
							m_nWork = 10;
						}
					}
					break;
				case 10:	
					m_bEnable = false;
					break;
				}
								
			}
			break;
		}
	}

	void Render()
	{
		if(m_bEnable == false)
			return;

		irr::core::vector2di pos;
		pos = m_v2Pos - m_v2RefPos;

		ggf::irr2d::DrawAniTile(
			m_pAniTile,m_pTex,
			irr::core::position2d<irr::s32>(pos.X,pos.Y),
			m_nFrame,m_colOverlay
		);
	}

};


}
}
