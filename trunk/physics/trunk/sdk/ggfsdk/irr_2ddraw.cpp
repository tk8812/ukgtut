
#include <irrlicht.h>

using namespace irr;

#include <string>
#include <vector>
#include "tinyxml.h"
#include "ggfsdk.h"
//#include "ggf_def.h"
//#include "ggf_fileSystem.h"
//#include "ggf_xml.h"
#include "LayerMng.h"

#include "irr_2ddraw.h"


namespace ggf 
{
namespace irr2d
{

video::IVideoDriver *g_pDriver;


void Init(video::IVideoDriver *pDriver)
{
	g_pDriver = pDriver;
}

void DrawAniTile(ggf::layer::S_AniTileLayer *pTile,irr::video::ITexture *pTex,irr::core::position2d<s32> posDes,int nFrame,video::SColor color )
{
	//ggf::layer::S_Texture *pTexDisc;
	//video::ITexture *pTex;
	//pTexDisc	= lm.FindTexture(pTile->m_strTexName.c_str());
	//pTex		= g_pDriver->getTexture(pTexDisc->m_strName.c_str());	

	int fx;
	int fy;
	int a1;

	a1 = pTile->m_rectFrame.getWidth() * nFrame;

	if( a1 >= pTile->m_rectCut.getWidth())
	{
		int nTemp;
		int nGridY;
		nTemp = (pTile->m_rectFrame.getWidth() * nFrame) % pTile->m_rectCut.getWidth();
		nGridY = (pTile->m_rectFrame.getWidth() * nFrame) / pTile->m_rectCut.getWidth();
		

		fx = pTile->m_rectCut.UpperLeftCorner.X + nTemp;
		fy = pTile->m_rectCut.UpperLeftCorner.Y + (nGridY*pTile->m_rectFrame.getHeight());		

	}
	else
	{
		fx = pTile->m_rectCut.UpperLeftCorner.X + (pTile->m_rectFrame.getWidth() * nFrame);
		fy = pTile->m_rectCut.UpperLeftCorner.Y;

	}

	irr::core::rect<s32> rect = pTile->m_rectFrame + irr::core::position2d<s32>(fx,fy);

	g_pDriver->draw2DImage(
		pTex,
		posDes,
		rect,
		0,
		color,
		true 
		);
}

void DrawAniTile(ggf::layer::CLayerMng &lm,int nIndex,core::position2d<s32> posDes,int nFrame,video::SColor color )
{
	/*
	ggf::layer::S_AniTileLayer *pTile;
	ggf::layer::S_Texture *pTexDisc;
	video::ITexture *pTex;

	pTile = &(lm. m_vtAniTile[nIndex]);
	pTexDisc = lm.FindTexture(pTile->m_strTexName.c_str());

	pTex = g_pDriver->getTexture(pTexDisc->m_strName.c_str());	

	//irr::core::rect<s32> rect = pTile->m_rectFrame;

	int fx;// = 
	int fy;// = pTile->m_rectCut.UpperLeftCorner.Y;
	int a1;

	a1 = pTile->m_rectFrame.getWidth() * nFrame;

	if( a1 >= pTile->m_rectCut.getWidth())
	{
		int nTemp;
		int nGridY;
		nTemp = (pTile->m_rectFrame.getWidth() * nFrame) % pTile->m_rectCut.getWidth();
		nGridY = (pTile->m_rectFrame.getWidth() * nFrame) / pTile->m_rectCut.getWidth();
		

		fx = pTile->m_rectCut.UpperLeftCorner.X + nTemp;
		fy = pTile->m_rectCut.UpperLeftCorner.Y + (nGridY*pTile->m_rectFrame.getHeight());		

	}
	else
	{
		fx = pTile->m_rectCut.UpperLeftCorner.X + (pTile->m_rectFrame.getWidth() * nFrame);
		fy = pTile->m_rectCut.UpperLeftCorner.Y;

	}

	irr::core::rect<s32> rect = pTile->m_rectFrame + irr::core::position2d<s32>(fx,fy);

	//rect += irr::core::position2d<s32>(rect.getWidth()*n


	g_pDriver->draw2DImage(
		pTex,
		posDes,
		rect,
		0,
		color,
		true 
		);
		*/
}

void DrawAniTile(ggf::layer::CLayerMng &lm,std::string strTileName,core::position2d<s32> posDes,int nFrame,video::SColor color )
{
	ggf::layer::S_AniTileLayer *pTile;
	ggf::layer::S_Texture *pTexDisc;
	video::ITexture *pTex;

	//pTile = &(lm.m_vtAniTile[nIndex]);

	pTile = lm.FindAniTile(strTileName);	


	pTexDisc = lm.FindTexture(pTile->m_strTexName.c_str());

	pTex = g_pDriver->getTexture(pTexDisc->m_strName.c_str());	

	//irr::core::rect<s32> rect = pTile->m_rectFrame;

	int fx;// = 
	int fy;// = pTile->m_rectCut.UpperLeftCorner.Y;
	int a1;

	a1 = pTile->m_rectFrame.getWidth() * nFrame;

	if( a1 >= pTile->m_rectCut.getWidth())
	{
		int nTemp;
		int nGridY;
		nTemp = (pTile->m_rectFrame.getWidth() * nFrame) % pTile->m_rectCut.getWidth();
		nGridY = (pTile->m_rectFrame.getWidth() * nFrame) / pTile->m_rectCut.getWidth();
		

		fx = pTile->m_rectCut.UpperLeftCorner.X + nTemp;
		fy = pTile->m_rectCut.UpperLeftCorner.Y + (nGridY*pTile->m_rectFrame.getHeight());		

	}
	else
	{
		fx = pTile->m_rectCut.UpperLeftCorner.X + (pTile->m_rectFrame.getWidth() * nFrame);
		fy = pTile->m_rectCut.UpperLeftCorner.Y;

	}

	irr::core::rect<s32> rect = pTile->m_rectFrame + irr::core::position2d<s32>(fx,fy);

	//rect += irr::core::position2d<s32>(rect.getWidth()*n


	g_pDriver->draw2DImage(
		pTex,
		posDes,
		rect,
		0,
		color,
		true 
		);

}


void DrawTile(ggf::layer::CLayerMng &lm,int nIndex,core::position2d<s32> posDes,video::SColor color)
{
	ggf::layer::S_TileLayer *pTile;
	ggf::layer::S_Texture *pTexDisc;
	video::ITexture *pTex;

	//pTile = lm.FindTile(strTileName);
	pTile = &(lm.m_vtTile[nIndex]);
	pTexDisc = lm.FindTexture(pTile->m_strTexName.c_str());

	pTex = g_pDriver->getTexture(pTexDisc->m_strName.c_str());	

	g_pDriver->draw2DImage(
		pTex,
		posDes,
		core::rect<s32>(pTile->m_CutRect[0],pTile->m_CutRect[1],pTile->m_CutRect[0]+pTile->m_CutRect[2],pTile->m_CutRect[1]+pTile->m_CutRect[3]),
		0,
		color,
		true 
		);

}

void DrawTile(ggf::layer::CLayerMng &lm,std::string strTileName,core::position2d<s32> posDes,video::SColor color)
{
	ggf::layer::S_TileLayer *pTile;
	ggf::layer::S_Texture *pTexDisc;
	video::ITexture *pTex;

	pTile = lm.FindTile(strTileName);
	pTexDisc = lm.FindTexture(pTile->m_strTexName.c_str());

	pTex = g_pDriver->getTexture(pTexDisc->m_strName.c_str());	

	g_pDriver->draw2DImage(
		pTex,
		posDes,
		core::rect<s32>(pTile->m_CutRect[0],pTile->m_CutRect[1],pTile->m_CutRect[0]+pTile->m_CutRect[2],pTile->m_CutRect[1]+pTile->m_CutRect[3]),
		0,
		color,
		true 
		);
}

void DrawTile(ggf::layer::CLayerMng &lm,std::string strTileName,core::position2d<s32> posDes,video::SColor color,int nframeWidth,int nframeHeight,int nFrame)
{
	ggf::layer::S_TileLayer *pTile;
	ggf::layer::S_Texture *pTexDisc;
	video::ITexture *pTex;

	pTile = lm.FindTile(strTileName);
	pTexDisc = lm.FindTexture(pTile->m_strTexName.c_str());

	pTex = g_pDriver->getTexture(pTexDisc->m_strName.c_str());

	irr::core::rect<s32> rect = core::rect<s32>(pTile->m_CutRect[0],pTile->m_CutRect[1],
		pTile->m_CutRect[0] + nframeWidth,
		pTile->m_CutRect[1] + nframeHeight);

	rect += irr::core::position2d<s32>(nframeWidth*nFrame,0);

	g_pDriver->draw2DImage(
		pTex,
		posDes,
		rect,
		0,
		color,
		true
		);
}


/*
위치계산은 중앙정렬한다
*/
void DrawSprite(ggf::layer::CLayerMng &lm,std::string strSprName,video::SColor color)
{
	ggf::layer::S_SpriteLayer *pSpr;

	pSpr = lm.FindSprite(strSprName);

	DrawTile(lm,
		pSpr->m_stAniTileName,
		irr::core::position2d<irr::s32>(g_pDriver->getScreenSize().Width/2  + pSpr->m_RefPixel.X,g_pDriver->getScreenSize().Height/2 + pSpr->m_RefPixel.Y),
		color);
}



}
}