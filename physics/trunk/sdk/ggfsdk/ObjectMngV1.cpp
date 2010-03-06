#define _CRT_SECURE_NO_DEPRECATE

/*
최종 수정 : 2008.9.10

다수의 애니메이션 메쉬노드로 이루어진 캐릭터에 대하여 애니메이션 콜백을 1번만 적용시킴
(주의 아이디값이 0인 노드에게만 콜백이 이루어짐)

*/

#include <windows.h>
#include <stdio.h>
#include <comutil.h>

#include <irrlicht.h>
#include <string>
#include <vector>
#include <map>
#include <memory>

#include "tinyxml.h"
#include "ggfsdk.h"


#include "ObjectMngV1.h"

#include "irr_utill.h"

namespace ggf
{
	namespace object
	{

const char *CObjectMngV1::OBJECT_NODENAME = "ObjectRoot";
const char *CObjectMngV1::CHRAROOT_NODENAME = "chara_root";

CObjectMngV1::CObjectMngV1(void)
{
}

CObjectMngV1::~CObjectMngV1(void)
{
}

bool CObjectMngV1::Load(char *fn)
{
	TiXmlDocument xml(fn);
	TiXmlNode *pRoot;
	TiXmlElement *pElement;

	bool loadOkay = xml.LoadFile();
	if ( !loadOkay )
	{
		printf( "Could not load test file %s. Error='%s'. Exiting.\n",fn, xml.ErrorDesc() );
		return false;
	}
	else
	{		

		pRoot =  xml.RootElement();

		
		pElement = pRoot->FirstChildElement("SceneList");
		pElement = pElement->FirstChildElement("scene");

		while(pElement)
		{
			std::tr1::shared_ptr<S_SceneElement> spElement(new S_SceneElement);

			spElement->m_strName = pElement->Attribute("name");
			spElement->m_strFileName = pElement->Attribute("file");

			m_mapScene[spElement->m_strName] = spElement;

			pElement = pElement->NextSiblingElement("scene");
		}	

		pElement = pRoot->FirstChildElement("ObjectDisc");
		pElement = pElement->FirstChildElement("Object");
		while(pElement)
		{
			std::tr1::shared_ptr<S_ObjectElement> spObj(new S_ObjectElement);
			spObj->m_strName = pElement->Attribute("name");
			spObj->m_strSceneName = pElement->Attribute("scene_name");

			//액션정보파싱
			TiXmlElement *pElement2;

			pElement2 = pElement->FirstChildElement("Action");
			while(pElement2)
			{
				std::tr1::shared_ptr<S_ActionElement> spAct(new S_ActionElement);
				spAct->m_strName =  pElement2->Attribute("name");
				ggf::parser::GetAttrib(pElement2->Attribute("start"), spAct->m_Start);
				ggf::parser::GetAttrib(pElement2->Attribute("end"), spAct->m_End);
				ggf::parser::GetAttrib(pElement2->Attribute("tick"), spAct->m_Tick);
				spObj->m_mapAction[spAct->m_strName] = spAct;
				pElement2 = pElement2->NextSiblingElement("Action");
			}

			m_mapObject[spObj->m_strName] = spObj;
			pElement = pElement->NextSiblingElement("Object");
		}


	}

	return true;
}

int CObjectMngV1::getCharacterList(irr::scene::ISceneManager *pSmgr,std::string strObjName,irr::core::list<irr::scene::ISceneNode *> &chr_list)
{
	//캐릭터루트얻기
	std::string strSelSceneObjName = m_mapObject[strObjName]->m_strSceneName;

	//오브잭트루트얻기
	irr::scene::ISceneNode *pObjectRootNode = pSmgr->getSceneNodeFromName(CObjectMngV1::OBJECT_NODENAME);

	irr::scene::ISceneNode *pNode_selectObj = pSmgr->getSceneNodeFromName(strSelSceneObjName.c_str(),pObjectRootNode);

	if(!pNode_selectObj)
	{
		ggf::irr_util::DebugOutputFmt(NULL,"can not found %s Node\n",strSelSceneObjName.c_str());		
		/*irr::core::stringw strw;		
		char szBuf[64];		
		sprintf_s(szBuf,"can not found %s Node\n",strSelSceneObjName.c_str());		

		strw = szBuf;
		OutputDebugString(strw.c_str());		*/
		return 0;
	}

	irr::scene::ISceneNode *pNode_chara_root = pSmgr->getSceneNodeFromName(CObjectMngV1::CHRAROOT_NODENAME,pNode_selectObj);

	chr_list = pNode_chara_root->getChildren();
	return pNode_chara_root->getChildren().getSize();

}

irr::scene::IAnimatedMeshSceneNode* CObjectMngV1::getCharacterNodeByID(irr::scene::ISceneManager *pSmgr,std::string strObjName,int id)
{
	//캐릭터루트얻기
	std::string strSelSceneObjName = m_mapObject[strObjName]->m_strSceneName;

	//오브잭트루트얻기
	irr::scene::ISceneNode *pObjectRootNode = pSmgr->getSceneNodeFromName(CObjectMngV1::OBJECT_NODENAME);

	irr::scene::ISceneNode *pNode_selectObj = pSmgr->getSceneNodeFromName(strSelSceneObjName.c_str(),pObjectRootNode);

	irr::scene::ISceneNode *pNode_chara_root = pSmgr->getSceneNodeFromName(CObjectMngV1::CHRAROOT_NODENAME,pNode_selectObj);

	irr::scene::ISceneNode *pNode = pSmgr->getSceneNodeFromId(id,pNode_chara_root);

	if(pNode->getType() == irr::scene::ESNT_ANIMATED_MESH && pNode)
		return (irr::scene::IAnimatedMeshSceneNode* )pNode;

	return NULL;
}

//인자설명
//pSmgr
//strObjName : 오브잭트 선택
//strActName : 애니메이션 선택
//bLoop : 루핑모드
//pEndCallBack : 아이디값이 0인 노드만 애니메이숀종료콜백(setAnimationEndCallback)을 한다.
void CObjectMngV1::Animation_Play(irr::scene::ISceneManager *pSmgr,std::string strObjName,std::string strActName,bool bLoop,irr::scene::IAnimationEndCallBack *pEndCallBack)
{
	S_ActionElement *pAct = m_mapObject[strObjName]->m_mapAction[strActName].get();
	//오브잭트루트얻기
	irr::scene::ISceneNode *pObjectRootNode = pSmgr->getSceneNodeFromName(CObjectMngV1::OBJECT_NODENAME);

	//캐릭터 몸체 리스트 얻기
	irr::core::list<irr::scene::ISceneNode *> listchildren;

	if(!getCharacterList(pSmgr,strObjName,listchildren))
		return;

	irr::core::list<irr::scene::ISceneNode *>::Iterator it;
	it = listchildren.begin();
	for(;it != listchildren.end();it++)
	{
		irr::scene::IAnimatedMeshSceneNode *pAniNode = (irr::scene::IAnimatedMeshSceneNode *)(*it);		
		pAniNode->setLoopMode(bLoop);
		pAniNode->setFrameLoop(pAct->m_Start,pAct->m_End);
		pAniNode->setAnimationSpeed((irr::f32)pAct->m_Tick);		

		if(pEndCallBack)
		{
			//아이디값이 0인 노드만 애니메이숀종료콜백(setAnimationEndCallback)을 한다.
			if(pAniNode->getID() == 0)
				pAniNode->setAnimationEndCallback(pEndCallBack);
		}
	}
}

//프레임별로로 정지화면보기 기능 
//lerp 0~1사이값 0은 시작 1끝
void CObjectMngV1::Animation_Snap(irr::scene::ISceneManager *pSmgr,std::string strObjName,std::string strActName,irr::f32 lerp)
{
	S_ActionElement *pAct = m_mapObject[strObjName]->m_mapAction[strActName].get();

	//오브잭트루트얻기
	irr::scene::ISceneNode *pObjectRootNode = pSmgr->getSceneNodeFromName(CObjectMngV1::OBJECT_NODENAME);

	//캐릭터 몸체 리스트 얻기
	irr::core::list<irr::scene::ISceneNode *> listchildren;
	getCharacterList(pSmgr,strObjName,listchildren);

	irr::core::list<irr::scene::ISceneNode *>::Iterator it;
	it = listchildren.begin();
	for(;it != listchildren.end();it++)
	{
		irr::scene::IAnimatedMeshSceneNode *pAniNode = (irr::scene::IAnimatedMeshSceneNode *)(*it);
		pAniNode->setLoopMode(false);

		irr::s32 nFrmLen = pAct->m_End - pAct->m_Start;
		irr::s32 nSnapFrm = pAct->m_Start + (irr::s32)((irr::f32)nFrmLen * lerp);

		pAniNode->setFrameLoop(nSnapFrm,nSnapFrm);
		pAniNode->setAnimationSpeed(0);
	}
}

//임의 구간 애니메이션
void CObjectMngV1::Animation_TotalPlayer(irr::scene::ISceneManager *pSmgr,std::string strObjName,irr::s32 start,irr::s32 end)
{
	//S_ActionElement *pAct = m_mapObject[strObjName]->m_mapAction[strActName].get();

	//오브잭트루트얻기
	irr::scene::ISceneNode *pObjectRootNode = pSmgr->getSceneNodeFromName(CObjectMngV1::OBJECT_NODENAME);

	//캐릭터 몸체 리스트 얻기
	irr::core::list<irr::scene::ISceneNode *> listchildren;// = 
	getCharacterList(pSmgr,strObjName,listchildren);

	irr::core::list<irr::scene::ISceneNode *>::Iterator it;
	it = listchildren.begin();
	for(;it != listchildren.end();it++)
	{
		irr::scene::IAnimatedMeshSceneNode *pAniNode = (irr::scene::IAnimatedMeshSceneNode *)(*it);
		pAniNode->setLoopMode(false);

		//irr::s32 nFrmLen = pAct->m_End - pAct->m_Start;
		//irr::s32 nSnapFrm = pAct->m_Start + (irr::s32)((irr::f32)nFrmLen * lerp);

		pAniNode->setFrameLoop(start,end);
		pAniNode->setAnimationSpeed(30);
	}
}
void CObjectMngV1::Animation_TotalSnap(irr::scene::ISceneManager *pSmgr,std::string strObjName,irr::f32 frame)
{
	//S_ActionElement *pAct = m_mapObject[strObjName]->m_mapAction[strActName].get();

	//오브잭트루트얻기
	irr::scene::ISceneNode *pObjectRootNode = pSmgr->getSceneNodeFromName(CObjectMngV1::OBJECT_NODENAME);

	//캐릭터 몸체 리스트 얻기
	irr::core::list<irr::scene::ISceneNode *> listchildren;// = 
	getCharacterList(pSmgr,strObjName,listchildren);

	irr::core::list<irr::scene::ISceneNode *>::Iterator it;
	it = listchildren.begin();
	for(;it != listchildren.end();it++)
	{
		irr::scene::IAnimatedMeshSceneNode *pAniNode = (irr::scene::IAnimatedMeshSceneNode *)(*it);
		pAniNode->setLoopMode(false);

		//irr::s32 nFrmLen = pAct->m_End - pAct->m_Start;
		//irr::s32 nSnapFrm = pAct->m_Start + (irr::s32)((irr::f32)nFrmLen * lerp);

		pAniNode->setFrameLoop((irr::s32)frame,(irr::s32)frame);
		pAniNode->setAnimationSpeed(0);
	}
}

//0프레임으로 정지상태로세팅
void CObjectMngV1::Animation_Reset(irr::scene::ISceneManager *pSmgr,std::string strObjName)
{
	S_ActionElement *pAct;// = m_mapObject[strObjName]->m_mapAction[strActName].get();
	//첫번째 애니메이션 얻기
	pAct = m_mapObject.begin()->second.get()->m_mapAction.begin()->second.get();

	//오브잭트루트얻기
	irr::scene::ISceneNode *pObjectRootNode = pSmgr->getSceneNodeFromName(CObjectMngV1::OBJECT_NODENAME);

	//캐릭터 몸체 리스트 얻기
	irr::core::list<irr::scene::ISceneNode *> listchildren;// = 
	getCharacterList(pSmgr,strObjName,listchildren);

	irr::core::list<irr::scene::ISceneNode *>::Iterator it;
	it = listchildren.begin();
	for(;it != listchildren.end();it++)
	{
		irr::scene::IAnimatedMeshSceneNode *pAniNode = (irr::scene::IAnimatedMeshSceneNode *)(*it);
		pAniNode->setLoopMode(false);

		//irr::s32 nFrmLen = pAct->m_End - pAct->m_Start;
		//irr::s32 nSnapFrm = pAct->m_Start + (irr::s32)((irr::f32)nFrmLen * lerp);
		pAniNode->setFrameLoop(pAct->m_Start,pAct->m_Start);
		pAniNode->setAnimationSpeed(0);
	}
}



irr::scene::ISceneNode *CObjectMngV1::Clone(irr::scene::ISceneManager *pSmgr, std::string strOriObjName, std::string strCloneObjName)
{
	CObjectMngV1 *pObjMng = this;

	irr::scene::ISceneNode *pObjectRootNode = pSmgr->getSceneNodeFromName(CObjectMngV1::OBJECT_NODENAME);
	if(!pObjectRootNode)
	{
		pObjectRootNode = pSmgr->addEmptySceneNode();		
		pObjectRootNode->setName(CObjectMngV1::OBJECT_NODENAME);
	}

	//찾으려는 씬오브잭트가 존재하는지...
	if(pObjMng->m_mapScene.find(pObjMng->m_mapObject[strOriObjName]->m_strSceneName) != pObjMng->m_mapScene.end())
	{
		pSmgr->loadScene(pObjMng->m_mapScene[pObjMng->m_mapObject[strOriObjName]->m_strSceneName]->m_strFileName.c_str());
		irr::scene::ISceneNode *pNode = pSmgr->getSceneNodeFromName("ggf_atpt");

		if(pNode)
		{
			//정해진 이름으로 바꾼뒤 캐릭터 노드에 모은다.			

			std::tr1::shared_ptr<S_SceneElement> spElement(new S_SceneElement);	
			*(spElement.get()) = *(pObjMng->m_mapScene[pObjMng->m_mapObject[strOriObjName]->m_strSceneName].get());			
			spElement->m_strName = strCloneObjName + "_scn";

			pNode->setName(spElement->m_strName.c_str());
			pNode->setVisible(false);
			pNode->setParent(pObjectRootNode);

			m_mapScene[spElement->m_strName.c_str()] = spElement;

			m_mapObject[strCloneObjName] = std::tr1::shared_ptr<S_ObjectElement>(new S_ObjectElement);
			*(m_mapObject[strCloneObjName].get()) = *(m_mapObject[strOriObjName].get());
			m_mapObject[strCloneObjName]->m_strName = strCloneObjName;
			m_mapObject[strCloneObjName]->m_strSceneName = spElement->m_strName;			

			ggf::irr_util::DebugOutputFmt(NULL,"succes clonning %s current object num: %d\n",strCloneObjName.c_str()
				,pObjectRootNode->getChildren().getSize()
				);			

			return pNode;//pObjectRootNode->getChildren().getSize();
		}
		else
		{
			ggf::irr_util::DebugOutputFmt(NULL,"atpt node not found\n");			
		}	

	}
	return 0;
}


irr::scene::ISceneNode *CObjectMngV1::ConstructSceneNode(irr::scene::ISceneManager *pSmgr)
//오브잭트 씬리스트 로딩
{
	//irr::scene::ISceneManager *pSmgr = m_System.m_pSmgr;
	CObjectMngV1 *pObjMng = this;

	irr::scene::ISceneNode *pObjectRootNode = pSmgr->getSceneNodeFromName(CObjectMngV1::OBJECT_NODENAME);
	if(!pObjectRootNode)
	{
		pObjectRootNode = pSmgr->addEmptySceneNode();		
		pObjectRootNode->setName(CObjectMngV1::OBJECT_NODENAME);
	}

	std::map<std::string,std::tr1::shared_ptr<S_SceneElement> >::iterator it;
	it = pObjMng->m_mapScene.begin();

	for(;it != pObjMng->m_mapScene.end();it++)
	{
		
		pSmgr->loadScene(it->second->m_strFileName.c_str());

		irr::scene::ISceneNode *pNode = pSmgr->getSceneNodeFromName("ggf_atpt");

		if(pNode)
		{
			//정해진 이름으로 바꾼뒤 캐릭터 노드에 모은다.
			pNode->setName(it->second->m_strName.c_str());
			pNode->setVisible(false);
			pNode->setParent(pObjectRootNode);

			ggf::irr_util::DebugOutputFmt(NULL,"succes load %s\n",it->second->m_strName.c_str());
			//printf("succes load %s\n",it->second->m_strName.c_str());
		}
		else
		{
			ggf::irr_util::DebugOutputFmt(NULL,"atpt node not found\n");
			//printf("atpt node not found\n");
		}			
	}

	ggf::irr_util::DebugOutputFmt(NULL,"%d of object registed.. \n",pObjectRootNode->getChildren().getSize());	

	return pObjectRootNode;
}

void CObjectMngV1::Animation_Tick(irr::scene::ISceneManager *pSmgr,std::string strObjName,std::string strActName,bool bPrv)
{
	S_ActionElement *pAct;// = m_mapObject[strObjName]->m_mapAction[strActName].get();
	//첫번째 애니메이션 얻기
	pAct = m_mapObject.begin()->second.get()->m_mapAction.begin()->second.get();

	//오브잭트루트얻기
	irr::scene::ISceneNode *pObjectRootNode = pSmgr->getSceneNodeFromName(CObjectMngV1::OBJECT_NODENAME);

	//캐릭터 몸체 리스트 얻기
	irr::core::list<irr::scene::ISceneNode *> listchildren;// = 
	getCharacterList(pSmgr,strObjName,listchildren);

	irr::core::list<irr::scene::ISceneNode *>::Iterator it;
	it = listchildren.begin();
	for(;it != listchildren.end();it++)
	{
		irr::scene::IAnimatedMeshSceneNode *pAniNode = (irr::scene::IAnimatedMeshSceneNode *)(*it);
		pAniNode->setLoopMode(false);

		irr::s32 nFrm = (irr::s32)pAniNode->getFrameNr();
		
		if(bPrv)
			pAniNode->setFrameLoop(nFrm-1,nFrm-1);
		else
			pAniNode->setFrameLoop(nFrm+1,nFrm+1);

		pAniNode->setAnimationSpeed(0);
	}

}

irr::scene::IAnimatedMeshSceneNode *CObjectMngV1::Char_GetNode(irr::scene::ISceneManager *pSmgr,std::string strObjName,irr::s32 id)
{
	//캐릭터루트얻기
	std::string strSelSceneObjName = m_mapObject[strObjName]->m_strSceneName;

	//오브잭트루트얻기
	irr::scene::ISceneNode *pObjectRootNode = pSmgr->getSceneNodeFromName(CObjectMngV1::OBJECT_NODENAME);

	irr::scene::ISceneNode *pNode_selectObj = pSmgr->getSceneNodeFromName(strSelSceneObjName.c_str(),pObjectRootNode);

	irr::scene::ISceneNode *pNode_chara_root = pSmgr->getSceneNodeFromName(CObjectMngV1::CHRAROOT_NODENAME,pNode_selectObj);


	return (irr::scene::IAnimatedMeshSceneNode *)pSmgr->getSceneNodeFromId(id, pNode_chara_root);


}

irr::scene::ISceneNode *CObjectMngV1::Root_GetNode(irr::scene::ISceneManager *pSmgr,std::string strObjName)
{
	//캐릭터루트얻기
	std::string strSelSceneObjName = m_mapObject[strObjName]->m_strSceneName;
	//오브잭트루트얻기
	irr::scene::ISceneNode *pObjectRootNode = pSmgr->getSceneNodeFromName(CObjectMngV1::OBJECT_NODENAME);

	irr::scene::ISceneNode *pNode_selectObj = pSmgr->getSceneNodeFromName(strSelSceneObjName.c_str(),pObjectRootNode);

	return pNode_selectObj;
}

}
}

