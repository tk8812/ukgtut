#include "irrlicht.h"
#include <vector>
#include <string>
#include "tinyxml.h"
#include "ggfsdk.h"
#include "LayerMng.h"


#include <windows.h>
#include <stdio.h>
#include <stdarg.h>

#include "irr_utill.h"

namespace ggf
{
namespace layer 
{

CLayerMng::CLayerMng(void)
{
}

CLayerMng::~CLayerMng(void)
{
}

S_Texture *CLayerMng::FindTexture(std::string strName)
{
	std::vector<S_Texture>::iterator it;
	for(it = m_vtTexture.begin();it != m_vtTexture.end();it++)
	{
		if(it->m_strName == strName)
			return &(*it);
	}
	return NULL;
}
S_TileLayer *CLayerMng::FindTile(std::string strName)
{
	std::vector<S_TileLayer>::iterator it;
	for(it = m_vtTile.begin();it != m_vtTile.end();it++)
	{
		if(it->m_strName == strName)
			return &(*it);
	}

	return NULL;
}
S_SpriteLayer *CLayerMng::FindSprite(std::string strName)
{
	std::vector<S_SpriteLayer>::iterator it;
	for(it = m_vtSprite.begin();it != m_vtSprite.end();it++)
	{
		if(it->m_strName == strName)
			return &(*it);
	}

	return NULL;
}

S_AniTileLayer *CLayerMng::FindAniTile(std::string strName)
{
	/*
	std::vector<S_AniTileLayer>::iterator it;
	for(it = m_vtAniTile.begin();it != m_vtAniTile.end();it++)
	{
		if(it->m_strName == strName)
			return &(*it);
	}
	*/

	std::map<std::string, S_AniTileLayer>::iterator it;

	it = m_AniTiles.find(strName);

	if(it != m_AniTiles.end() )
		return &(it->second);

	return NULL;
}


void CLayerMng::Load(irr::IrrlichtDevice *pdevice,char *szfile)
{
	{//씬스크립트로드		

		{//씬스크립트로드

			TiXmlDocument xml(szfile);
			bool loadOkay = xml.LoadFile();
			if ( !loadOkay )
			{
				
			}
			else
			{
				Load(xml);
			}

		}

		//디바이스가 주어지지않앗으면 그냥 나가기
		if(pdevice == NULL)
		{
			return;
		}
		else
		{
			irr::video::IVideoDriver *pDriver = pdevice->getVideoDriver();	

			{//텍스춰 로드
				irr::video::IImage *pImg;
				irr::core::stringc str;
				irr::u32 i;

				for(i=0;i < m_vtTexture.size();i++)
				{
					if(m_vtTexture[i].m_strName == "###")
						break;

					str = m_strBasePath.c_str();
					str += m_vtTexture[i].m_strFile.c_str();
					pImg = pDriver->createImageFromFile(str.c_str());
					if(pImg)
					{
						pDriver->addTexture(m_vtTexture[i].m_strName.c_str(),pImg);
						ggf::irr_util::DebugOutputFmt(NULL,"load success....%s \n",str.c_str());
						pImg->drop();
					}
					else
					{
						ggf::irr_util::DebugOutputFmt(NULL,"load failed....%s \n",str.c_str());
					}
				}
			}
		}
	}
}

void CLayerMng::Load(TiXmlDocument &xml)
{
	std::string str;
	ggf::parser::VectorInt viTemp;

	m_vtTexture.clear();

	TiXmlNode *pRoot;
	TiXmlElement *pElement;

	pRoot =  xml.RootElement();

	//배이스 패스설정
	pElement = pRoot->FirstChildElement("Info");
	if(pElement)
		m_strBasePath = pElement->Attribute("Base_Path");
	else
		m_strBasePath = "../res/ggfTestWork/texture/";


	//텍스춰 정보얻기
	pElement = pRoot->FirstChildElement("TexDisc");
	pElement = pElement->FirstChildElement("Texture");

	while(pElement)
	{
		S_Texture tex;

		tex.m_strFile = pElement->Attribute("file");
		tex.m_strName = pElement->Attribute("name");

		m_vtTexture.push_back(tex);

		pElement = pElement->NextSiblingElement("Texture");

	}

	//정적타일
	pElement = pRoot->FirstChildElement("TileDisc");
	pElement = pElement->FirstChildElement("TileLayer");

	while(pElement)
	{
		S_TileLayer tile;

		tile.m_strName = pElement->Attribute("name");

		pElement = pElement->FirstChildElement("data");

		tile.m_strTexName = pElement->Attribute("texname");

		ggf::parser::GetAttrib(pElement->Attribute("cut_rect"),tile.m_CutRect);
		tile.m_CutRect2 = irr::core::rect<irr::s32>(tile.m_CutRect[0],tile.m_CutRect[1],tile.m_CutRect[2]+tile.m_CutRect[0],tile.m_CutRect[3]+tile.m_CutRect[1]);
		tile.m_colTrans = 0;
		m_vtTile.push_back(tile);

		pElement = pElement->Parent()->ToElement();
		pElement = pElement->NextSiblingElement("TileLayer");
	}

	//동적타일
	pElement = pRoot->FirstChildElement("AniTileDisc");
	pElement = pElement->FirstChildElement("TileLayer");
	
	while(pElement)
	{
		S_AniTileLayer tile;
		ggf::parser::VectorInt viTemp;

		tile.m_strName = pElement->Attribute("name");

		pElement = pElement->FirstChildElement("data");

		tile.m_strTexName = pElement->Attribute("texname");

		ggf::parser::GetAttrib(pElement->Attribute("cut_rect"),viTemp);
		tile.m_rectCut = irr::core::rect<irr::s32>(viTemp[0],viTemp[1],viTemp[2]+viTemp[0],viTemp[3]+viTemp[1]);
		tile.m_colTrans = 0;

		viTemp.clear();

		ggf::parser::GetAttrib(pElement->Attribute("frame_rect"),viTemp);
		
		tile.m_rectFrame = irr::core::rect<irr::s32>(0,0,viTemp[0],viTemp[1]);

		pElement->Attribute("frame_num",&tile.m_nTotalFrame );

		m_AniTiles[tile.m_strName] = tile;

		pElement = pElement->Parent()->ToElement();
		pElement = pElement->NextSiblingElement("TileLayer");
	}

	//애니메이션
	pElement = pRoot->FirstChildElement("SpriteLayerDisc");

	if(!pElement)
	{
		ggf::irr_util::DebugOutputFmt(NULL,"there is no Sprite Disc\n");
	}
	else
	{
		pElement = pElement->FirstChildElement("SpriteLayer");
		while(pElement)
		{
			S_SpriteLayer sprLayer;
			ggf::parser::VectorInt viTemp;

			sprLayer.m_strName = pElement->Attribute("name");
			pElement = pElement->FirstChildElement("data");

			sprLayer.m_stAniTileName = pElement->Attribute("AniTilename");
			pElement->Attribute("time",(int *)&(sprLayer.m_uTotalTime));


			ggf::parser::GetAttrib(pElement->Attribute("rpixel"),viTemp);
			sprLayer.m_RefPixel = irr::core::position2d<irr::s32>(viTemp[0],viTemp[1]);		

			viTemp.clear();
			ggf::parser::GetAttrib(pElement->Attribute("seq"),viTemp);
			sprLayer.m_vtIndex = viTemp;

			m_vtSprite.push_back(sprLayer);

			pElement = pElement->Parent()->ToElement();
			pElement = pElement->NextSiblingElement("SpriteLayer");

		}
	}

	if(!pElement)
	{
		ggf::irr_util::DebugOutputFmt(NULL,"there is no MiXed sCene Disc\n");
	}
	else
	{

		//조합씬
		pElement = pRoot->FirstChildElement("LayerXceneDisc");
		pElement = pElement->FirstChildElement("LayerXean");

		while(pElement)
		{
			S_Xcene xcene;

			xcene.m_strName = pElement->Attribute("name");
			xcene.m_vsSpriteLayer.clear();
			ggf::parser::GetAttrib(pElement->Attribute("data"),xcene.m_vsSpriteLayer);
			m_vtXcene.push_back(xcene);

			pElement = pElement->Parent()->ToElement();
			pElement = pElement->NextSiblingElement("LayerXean");
		}
	}
}

void CLayerMng::Save(TiXmlDocument &xml)
{
	TiXmlNode *pRoot;
	//TiXmlElement *pElement;
	char szBuf[1024];

	pRoot =  xml.RootElement();

	{
		TiXmlElement item("Info");
		item.SetAttribute("Base_Path",m_strBasePath.c_str());
		pRoot->InsertEndChild(item);
		
	}

	{
		TiXmlElement item("TexDisc");
		//item.SetAttribute("Base_Path",m_strBasePath.c_str());
		//pRoot->InsertEndChild(item);

		std::vector<ggf::layer::S_Texture>::iterator it;		

		for(it=m_vtTexture.begin();it != m_vtTexture.end();it++)
		{
			TiXmlElement texture_item("Texture");

			texture_item.SetAttribute("name",it->m_strName.c_str());
			texture_item.SetAttribute("file",it->m_strFile.c_str());		

			item.InsertEndChild(texture_item);
		}		
		pRoot->InsertEndChild(item);
	}

	{//타일저장
		TiXmlElement item("TileDisc");
		
		std::vector<ggf::layer::S_TileLayer>::iterator it;

		for(it=m_vtTile.begin();it != m_vtTile.end();it++)
		{
			TiXmlElement layer_item("TileLayer");
			layer_item.SetAttribute("name",it->m_strName.c_str());

			TiXmlElement layerdata_item("data");
			layerdata_item.SetAttribute("texname",it->m_strTexName.c_str());
			
			sprintf_s(szBuf,"%d,%d,%d,%d",it->m_CutRect2.UpperLeftCorner.X
				,it->m_CutRect2.UpperLeftCorner.Y
				,it->m_CutRect2.getWidth()
				,it->m_CutRect2.getHeight());
			layerdata_item.SetAttribute("cut_rect",szBuf);

			layer_item.InsertEndChild(layerdata_item);
			item.InsertEndChild(layer_item);
		}		
		pRoot->InsertEndChild(item);
	}

	{//타일저장
		TiXmlElement item("AniTileDisc");			
		

		std::map<std::string,ggf::layer::S_AniTileLayer>::iterator	it;	

		it = m_AniTiles.begin();

		for(;it != m_AniTiles.end();it++)			
		{
			TiXmlElement layer_item("TileLayer");
			layer_item.SetAttribute("name",it->first.c_str());		

			TiXmlElement layerdata_item("data");
			layerdata_item.SetAttribute("texname",it->second.m_strTexName.c_str());

			sprintf_s(szBuf,"%d,%d,%d,%d",
				it->second.m_rectCut.UpperLeftCorner.X
				,it->second.m_rectCut.UpperLeftCorner.Y
				,it->second.m_rectCut.getWidth()
				,it->second.m_rectCut.getHeight());
			layerdata_item.SetAttribute("cut_rect",szBuf);

			sprintf_s(szBuf,"%d,%d"				
				,it->second.m_rectFrame.getWidth()
				,it->second.m_rectFrame.getHeight());
			layerdata_item.SetAttribute("frame_rect",szBuf);

			sprintf_s(szBuf,"%d",it->second.m_nTotalFrame);
			layerdata_item.SetAttribute("frame_num",szBuf);
			layer_item.InsertEndChild(layerdata_item);
			item.InsertEndChild(layer_item);

		}
		pRoot->InsertEndChild(item);

	}



	xml.Print();
	xml.SaveFile();



	/*
	char szBuf[1024];

	xml.AddElem("root");
	xml.IntoElem();
	{//택스춰저장
		xml.AddElem("TexDisc");
		xml.IntoElem();

		std::vector<ggf::layer::S_Texture>::iterator it;		

		for(it=m_vtTexture.begin();it != m_vtTexture.end();it++)
		{
			xml.AddElem("Texture");
			xml.AddAttrib("name",it->m_strName.c_str());
			xml.AddAttrib("file",it->m_strFile.c_str());
		}
		xml.AddElem("Texture");
		xml.AddAttrib("name","###");

		xml.OutOfElem();
	}

	{//타일저장
		xml.AddElem("TileDisc");
		xml.IntoElem();

		std::vector<ggf::layer::S_TileLayer>::iterator it;

		for(it=m_vtTile.begin();it != m_vtTile.end();it++)
		{
			xml.AddElem("TileLayer");
			xml.AddAttrib("name",it->m_strName.c_str());
			xml.IntoElem();

			xml.AddElem("data");
			xml.AddAttrib("texname",it->m_strTexName.c_str());
			sprintf_s(szBuf,"%d,%d,%d,%d",it->m_CutRect2.UpperLeftCorner.X
				,it->m_CutRect2.UpperLeftCorner.Y
				,it->m_CutRect2.getWidth()
				,it->m_CutRect2.getHeight());
			xml.AddAttrib("cut_rect",szBuf);
			xml.OutOfElem();
			
		}

		xml.AddElem("TileLayer");
		xml.AddAttrib("name","###");		

	
		xml.OutOfElem();
	}

	{ //애니타일
		xml.AddElem("AniTileDisc");				
		xml.IntoElem();

		std::vector<ggf::layer::S_AniTileLayer>::iterator it;

		for(it=m_vtAniTile.begin();it != m_vtAniTile.end();it++)
		{
			xml.AddElem("TileLayer");
			xml.AddAttrib("name",it->m_strName.c_str());
			xml.IntoElem();

			xml.AddElem("data");
			xml.AddAttrib("texname",it->m_strTexName.c_str());
			sprintf_s(szBuf,"%d,%d,%d,%d",
				it->m_rectCut.UpperLeftCorner.X
				,it->m_rectCut.UpperLeftCorner.Y
				,it->m_rectCut.getWidth()
				,it->m_rectCut.getHeight());
			xml.AddAttrib("cut_rect",szBuf);

			sprintf_s(szBuf,"%d,%d"				
				,it->m_rectFrame.getWidth()
				,it->m_rectFrame.getHeight());
			xml.AddAttrib("frame_rect",szBuf);

			sprintf_s(szBuf,"%d",it->m_nTotalFrame);
			xml.AddAttrib("frame_num",szBuf);
			
			xml.OutOfElem();
		}
		xml.AddElem("TileLayer");
		xml.AddAttrib("name","###");		

		xml.OutOfElem();
	}

	{//스프라이트 레이어
		xml.AddElem("SpriteLayerDisc");
		xml.IntoElem();

		std::vector<ggf::layer::S_SpriteLayer>::iterator it;
		for(it=m_vtSprite.begin();it != m_vtSprite.end();it++)
		{
			xml.AddElem("SpriteLayer");
			xml.AddAttrib("name",it->m_strName.c_str());
			xml.IntoElem();

			xml.AddElem("data");
			xml.AddAttrib("AnitileName",it->m_stAniTileName.c_str());			
			sprintf_s(szBuf,"%d",it->m_uTotalTime);
			xml.AddAttrib("time",szBuf);			
			sprintf_s(szBuf,"%d,%d",it->m_RefPixel.X,it->m_RefPixel.Y);
			xml.AddAttrib("rpixel",szBuf);

			xml.OutOfElem();
		}

		xml.AddElem("SpriteLayer");
		xml.AddAttrib("name","###");

		xml.OutOfElem();
	}

	{//Xcene 레이어
		xml.AddElem("LayerXceneDisc");
		xml.IntoElem();		

		std::vector<ggf::layer::S_Xcene>::iterator it;
		for(it=m_vtXcene.begin();it != m_vtXcene.end();it++)
		{
			xml.AddElem("LayerXean");
			xml.AddAttrib("name",it->m_strName.c_str());
		
			ggf::parser::VectorString::iterator itvs;

			std::string strSprList;
			itvs = it->m_vsSpriteLayer.begin();
			
			if(itvs != it->m_vsSpriteLayer.end())
			{
				strSprList = *itvs;
				itvs++;
			}

			for(;itvs != it->m_vsSpriteLayer.end();itvs++)
			{
				strSprList.append(",");
				strSprList.append(*itvs);
			}

			xml.AddAttrib("data",strSprList.c_str());
			//xml.OutOfElem();
		}

		xml.AddElem("LayerXean");
		xml.AddAttrib("name","###");

		xml.OutOfElem();
		
	}
	*/


}

void CLayerMng::Save(std::string &strXml)
{	
	const char* demoStart =
		"<root>"
		"</root>"
		;
	TiXmlDocument xml(strXml.c_str());
	xml.Parse(demoStart);
	Save(xml);
	

	
}

}
}
