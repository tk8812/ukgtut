#pragma once

#include <map>

namespace ggf
{
namespace layer
{

	struct S_Texture
	{
		std::string m_strName;
		std::string m_strFile;
	};

	struct S_TileLayer
	{
		std::string m_strName;
		std::string m_strTexName;
		ggf::parser::VectorInt m_CutRect; //좌상단 넓이높이 순서로 데이터들어감		
		irr::core::rect<irr::s32> m_CutRect2;
		unsigned int m_colTrans;

		void set(const char *szName,irr::s32 x,irr::s32 y,irr::s32 w,irr::s32 h)
		{
			m_strName = szName;
			m_CutRect[0] = x;
			m_CutRect[1] = y;
			m_CutRect[2] = w;
			m_CutRect[3] = h;
			m_CutRect2 = irr::core::rect<irr::s32>(x,y,x+w,y+h);
		}
	};

	struct S_AniTileLayer
	{
		std::string m_strName;
		std::string m_strTexName;		
		irr::core::rect<irr::s32> m_rectCut;
		irr::core::rect<irr::s32> m_rectFrame;
		int m_nTotalFrame;
		unsigned int m_colTrans;

		void set(const char *szName,irr::s32 x,irr::s32 y,irr::s32 w,irr::s32 h,int framew,int frameh,int framenum)
		{
			m_strName = szName;
			m_rectCut = irr::core::rect<irr::s32>(x,y,x+w,y+h);
			m_rectFrame = irr::core::rect<irr::s32>(0,0,framew,frameh);
			m_nTotalFrame = framenum;
		}

	};

	struct S_SpriteLayer
	{
		std::string m_strName;
		std::string m_stAniTileName;
		irr::u32	m_uTotalTime; //밀리초단위 1000이면 1초
		irr::core::position2d<irr::s32> m_RefPixel;
		ggf::parser::VectorInt m_vtIndex; //시퀀스 인덱스	
		
	};

	struct S_Xcene
	{
		std::string m_strName;
		ggf::parser::VectorString m_vsSpriteLayer;		
	};


class CLayerMng
{
public:
	CLayerMng(void);
public:
	virtual ~CLayerMng(void);

	std::vector<S_Texture>		m_vtTexture;
	std::vector<S_TileLayer>	m_vtTile;	
	std::map<std::string, S_AniTileLayer>	m_AniTiles;	
	std::vector<S_SpriteLayer>	m_vtSprite;
	std::vector<S_Xcene>		m_vtXcene;

	irr::IrrlichtDevice			*m_pDevice;
	irr::video::IVideoDriver	*m_pDriver;

	std::string m_strBasePath;

public:

	void Load(TiXmlDocument &xml);//parser::GGFXml & xml);
	void Save(TiXmlDocument &xml);//ggf::parser::GGFXml &xml);
	void Save(std::string &strXml);

	void Load(irr::IrrlichtDevice *pdevice,char *szfile);
	
	S_Texture		*FindTexture(std::string strName);
	S_TileLayer		*FindTile(std::string strName);
	S_AniTileLayer  *FindAniTile(std::string strName);
	S_SpriteLayer	*FindSprite(std::string strName);

	void Init(irr::IrrlichtDevice *pDevice)
	{
		m_pDevice = pDevice;
		m_pDriver = m_pDevice->getVideoDriver();	

	}

	bool AddTexture(char *szName,char *szTexFile)
	{
		S_Texture tex;

		tex.m_strName = szName;
		tex.m_strFile = szTexFile;

		m_vtTexture.push_back(tex);

		return true;
	}

	bool IsExistTile(const char *szName)
	{
		std::string strName = szName;
		std::vector<S_TileLayer>::iterator it;
		for(it = m_vtTile.begin();it != m_vtTile.end();it++)
		{
			if(it->m_strName == strName)
			{				
				return true;
			}
		}

		return false;
	}

	bool AddTile(char *szName,char *szTexFile,irr::core::rect<irr::s32> pos)
	{
		S_TileLayer tile;
		
		tile.m_strName = szName;
		tile.m_strTexName = szTexFile;		
		tile.m_CutRect2 = pos;//irr::core::rect<irr::s32>(tile.m_CutRect[0],tile.m_CutRect[1],tile.m_CutRect[2]+tile.m_CutRect[0],tile.m_CutRect[3]+tile.m_CutRect[1]);
		tile.m_CutRect.push_back(tile.m_CutRect2.UpperLeftCorner.X);
		tile.m_CutRect.push_back(tile.m_CutRect2.UpperLeftCorner.Y);
		tile.m_CutRect.push_back(tile.m_CutRect2.getWidth());
		tile.m_CutRect.push_back(tile.m_CutRect2.getHeight());
		
		tile.m_colTrans = 0;

		m_vtTile.push_back(tile);

		return true;
	}

	bool RemoveTile(std::string strName)
	{
		
		std::vector<S_TileLayer>::iterator it;
		for(it = m_vtTile.begin();it != m_vtTile.end();it++)
		{
			if(it->m_strName == strName)
			{
				m_vtTile.erase(it);
				return true;
			}
		}

		return false;
	}

	bool RemoveTile(const char *szName)
	{
		return RemoveTile(std::string(szName));
	}

	bool IsExistAniTile(const char *szName)
	{
		
		std::string strName = szName;

		if(m_AniTiles.count(strName) > 0)
			return true;
		return false;

		/*
		std::vector<S_AniTileLayer>::iterator it;
		for(it = m_vtAniTile.begin();it != m_vtAniTile.end();it++)
		{
			if(it->m_strName == strName)
			{				
				return true;
			}
		}
		return false;
		*/
	}

	bool AddAniTile(char *szName,char *szTexFile,irr::core::rect<irr::s32> pos,irr::core::rect<irr::s32> rectFrame,int nTotalFrame)
	{
		S_AniTileLayer AniTile;

		AniTile.m_strName = szName;
		AniTile.m_strTexName = szTexFile;
		AniTile.m_nTotalFrame = nTotalFrame;
		AniTile.m_rectCut = pos;
		AniTile.m_rectFrame = rectFrame;
		AniTile.m_colTrans = 0;

		m_AniTiles[AniTile.m_strName] = AniTile;
		//m_vtAniTile.push_back(AniTile);

		return true;
	}	

	bool RemoveAniTile(std::string strName)
	{
		m_AniTiles.erase(strName);
		/*
		std::vector<S_AniTileLayer>::iterator it;
		for(it = m_vtAniTile.begin();it != m_vtAniTile.end();it++)
		{
			if(it->m_strName == strName)
			{
				m_vtAniTile.erase(it);
				return true;
			}
		}
		*/

		return false;
		
	}

	bool RemoveAniTile(const char *szName)
	{
		return RemoveAniTile(std::string(szName));
	}

	void DrawAniTile(std::string strTileName,irr::core::position2d<irr::s32> posDes,int nFrame,irr::video::SColor color = irr::video::SColor(255,255,255,255) )
	{
		ggf::layer::S_AniTileLayer *pTile;
		ggf::layer::S_Texture *pTexDisc;
		irr::video::ITexture *pTex;

		//pTile = &(lm.m_vtAniTile[nIndex]);

		pTile = FindAniTile(strTileName);	


		pTexDisc = FindTexture(pTile->m_strTexName.c_str());

		pTex = m_pDriver->getTexture(pTexDisc->m_strName.c_str());	

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

		irr::core::rect<irr::s32> rect = pTile->m_rectFrame + irr::core::position2d<irr::s32>(fx,fy);

		//rect += irr::core::position2d<s32>(rect.getWidth()*n


		m_pDriver->draw2DImage(
			pTex,
			posDes,
			rect,
			0,
			color,
			true 
			);

	}

	/*
	nDanGan : 단위사이의간격(...억...만...원)(픽셀),-1이면 ,표시
	nDanDiv : 몇자리단위씩 나눌것인가?
	nGan : 글자사이기본간격

	*/
	void DrawNumFont(std::string strTileName,irr::core::position2d<irr::s32> pos,int nDanGan,int nDanDiv,int nGan,int num )
	{
		//CString
		char szBuf[256];
		sprintf_s(szBuf,"%d",num);

		std::string str = szBuf;	
		
		int i,j,k;
		j=(int)str.size();
		k=0;
		for(i=0;i<j;i++)
		{
			if(k >= nDanDiv)
			{
				k=0;
				str.insert(j-i,",");
			}
			k++;
		}

		for(i=(int)str.size()-1;i>=0;i--)
		{
			if(str[i] == ',')
			{
				if(nDanGan < 0)
				{
					DrawAniTile(strTileName,pos,10);
					pos -= irr::core::position2d<irr::s32>(nGan,0);
				}
				else
				{
					pos -= irr::core::position2d<irr::s32>(nDanGan,0);
				}

			}
			else
			{
				DrawAniTile(strTileName,pos,str[i]-'0');
				pos -= irr::core::position2d<irr::s32>(nGan,0);
			}
		}		
	}


	void DrawNumFont(std::string strTileName,irr::core::position2d<irr::s32> pos,int nDanGan,int nDanDiv,int nGan,__int64 num )
	{
		//CString
		char szBuf[256];
		//sprintf_s(szBuf,"%d",num);
		_i64toa_s(num,szBuf,256,10);
		std::string str = szBuf;	
		
		int i,j,k;
		j=(int)str.size();
		k=0;
		for(i=0;i<j;i++)
		{
			if(k >= nDanDiv)
			{
				k=0;
				str.insert(j-i,",");
			}
			k++;
		}

		for(i=(int)str.size()-1;i>=0;i--)
		{
			if(str[i] == ',')
			{
				if(nDanGan < 0)
				{
					DrawAniTile(strTileName,pos,10);
					pos -= irr::core::position2d<irr::s32>(nGan,0);
				}
				else
				{
					pos -= irr::core::position2d<irr::s32>(nDanGan,0);
				}

			}
			else
			{
				DrawAniTile(strTileName,pos,str[i]-'0');
				pos -= irr::core::position2d<irr::s32>(nGan,0);
			}
		}		
	}

	

};


}

}
