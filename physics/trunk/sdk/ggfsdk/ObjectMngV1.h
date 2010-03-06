#pragma once


namespace ggf
{
	namespace object
	{

struct S_SceneElement
{
	std::string m_strName;
	std::string m_strFileName;
};

struct S_ActionElement
{
	std::string m_strName;
	int m_Start;
	int m_End;
	int m_Tick;
};

struct S_ObjectElement
{
	std::string m_strName;
	std::string m_strSceneName;
	std::map<std::string,std::tr1::shared_ptr<S_ActionElement> > m_mapAction;
};



class CObjectMngV1
{
public:
	CObjectMngV1(void);
	virtual ~CObjectMngV1(void);

	std::map<std::string,std::tr1::shared_ptr<S_SceneElement> > m_mapScene;
	std::map<std::string,std::tr1::shared_ptr<S_ObjectElement> > m_mapObject;	
	
	static const char *OBJECT_NODENAME;// = "ObjectRoot";
	static const char *CHRAROOT_NODENAME;

	bool Load(char *fn);

	int getCharacterList(irr::scene::ISceneManager *pSmgr,std::string strObjName,irr::core::list<irr::scene::ISceneNode *> &chr_list);
	irr::scene::IAnimatedMeshSceneNode* getCharacterNodeByID(irr::scene::ISceneManager *pSmgr,std::string strObjName,int id);
	

	//도우미 함수들
	//전구간플래이
	void Animation_Play(irr::scene::ISceneManager *pSmgr,std::string strObjName,std::string strActName,bool bLoop = true,irr::scene::IAnimationEndCallBack *pEndCallBack=NULL);
	
	//프레임별로로 정지화면보기 기능 
	//lerp 0~1사이값 0은 시작 1끝
	void Animation_Snap(irr::scene::ISceneManager *pSmgr,std::string strObjName,std::string strActName,irr::f32 lerp); 

	//한칸씩움직이기
	//bPrv가 참이면 뒤로 거짓이면 앞으로...
	void Animation_Tick(irr::scene::ISceneManager *pSmgr,std::string strObjName,std::string strActName,bool bPrv); 
	

	//임의 구간 애니메이션
	void Animation_TotalPlayer(irr::scene::ISceneManager *pSmgr,std::string strObjName,irr::s32 start,irr::s32 end); 
	void Animation_TotalSnap(irr::scene::ISceneManager *pSmgr,std::string strObjName,irr::f32 frame); 

	//0프레임으로 정지상태로세팅
	void Animation_Reset(irr::scene::ISceneManager *pSmgr,std::string strObjName);

	//애니메이션 캐릭터노드얻기
	irr::scene::IAnimatedMeshSceneNode *Char_GetNode(irr::scene::ISceneManager *pSmgr,std::string strObjName,irr::s32 id);
	//이름이 strObjName인 오브잭트 루트노드얻기
	irr::scene::ISceneNode *Root_GetNode(irr::scene::ISceneManager *pSmgr,std::string strObjName);

	//오브잭트들를 씬노드트리에 생성시킨다.
	irr::scene::ISceneNode *ConstructSceneNode(irr::scene::ISceneManager *pSmgr);

	//오브잭트복재
	//strOriObjName 원본이름
	//strCloneObjName 복사본이름
	irr::scene::ISceneNode *Clone(irr::scene::ISceneManager *pSmgr,std::string strOriObjName,std::string strCloneObjName);

};


	}
}
