#pragma once

namespace ggf
{
namespace object
{

struct S_AnimatedMesh
{
	std::string m_strName;
	std::string m_strFile;
	irr::scene::IAnimatedMesh *m_pMesh;
};

struct S_Object_Action
{
	std::string m_strName;
	irr::s32 m_nStart;
	irr::s32 m_nEnd;	
	irr::u32 m_uTick;
};

struct S_DISC_Object
{	

	std::string m_strName;
	std::string m_strMeshName;
	std::string m_strTexture;
	std::string m_strType;
		

	irr::core::vector3df			m_v3Pos;	
	irr::core::vector3df			m_v3Rotation;	
	irr::core::vector3df			m_v3Scale;	
	std::vector<S_Object_Action>	m_vtAction;
};



class CObjectMng
{	
public:
		
	std::vector<ggf::object::S_DISC_Object> m_vtObjectDefine;	
	
	std::vector<ggf::object::S_AnimatedMesh> m_vt3dAniMesh;
	std::vector<ggf::object::S_AnimatedMesh> m_vt3dOcTreeMesh;
	std::vector<ggf::object::S_AnimatedMesh> m_vt3dTerrainMesh;


	CObjectMng()
	{	

	}
	~CObjectMng()
	{		
	}

	irr::scene::IAnimatedMesh *FindAniMesh(const char *szName)
	{
		std::vector<ggf::object::S_AnimatedMesh>::iterator it;

		for(it = m_vt3dAniMesh.begin();it != m_vt3dAniMesh.end();it++)
		{
			if(it->m_strName == szName)
			{
				return it->m_pMesh;
			}
		}
		
		return NULL;
	}	

	irr::scene::IAnimatedMesh *FindOctreeMesh(const char *szName)
	{
		std::vector<ggf::object::S_AnimatedMesh>::iterator it;

		for(it = m_vt3dOcTreeMesh.begin();it != m_vt3dOcTreeMesh.end();it++)
		{
			if(it->m_strName == szName)
			{
				return it->m_pMesh;
			}
		}
		
		return NULL;
	}	

	irr::scene::IAnimatedMesh *FindAniMeshByObjectName(const char *szName)
	{
		std::vector<ggf::object::S_DISC_Object>::iterator it;

		for(it = m_vtObjectDefine.begin();it != m_vtObjectDefine.end();it++)
		{
			if(it->m_strName == szName)
			{
				return FindAniMesh(it->m_strMeshName.c_str());				
			}
		}
		
		return NULL;
	}

	irr::scene::IAnimatedMesh *FindAniMeshByObjectName(const wchar_t *szName)
	{
		irr::core::stringc  strc = szName;
		return FindAniMeshByObjectName(strc.c_str());
	}

	std::vector<ggf::object::S_DISC_Object>::iterator FindObjectDisc(const char *szName)
	{
		std::vector<ggf::object::S_DISC_Object>::iterator it;
		
		for(it=m_vtObjectDefine.begin();it != m_vtObjectDefine.end();it++)
		{
			if(it->m_strName == szName)
				break;
		}

		return it;
	}	

	irr::s32 getFrameCount(const char *szObjectName)
	{
		irr::scene::IAnimatedMesh *pMesh = FindAniMeshByObjectName(szObjectName);
		if(pMesh)
			return pMesh->getFrameCount();
		return 0;

	}

	irr::s32 getFrameCount(const wchar_t *szObjectName)
	{
		irr::core::stringc  strc = szObjectName;

		return getFrameCount(strc.c_str());		
	}	

	bool LoadXml(irr::IrrlichtDevice *pDev,irr::scene::ISceneManager *pSmgr,irr::s32 *pnCounter=0,char *fn="../res/script/object.xml");
	
};

}
}