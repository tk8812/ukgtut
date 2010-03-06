
#include "irrlicht.h"
#include <vector>
#include <string>

#include "ggfsdk.h"
#include "ObjectMng.h"
/*
#include "ggf_def.h"
#include "ggf_xml.h"

*/


namespace ggf
{
namespace object
{
	bool CObjectMng::LoadXml(irr::IrrlichtDevice *pDev, irr::scene::ISceneManager *pSmgr, irr::s32 *pnCounter,char *fn)
	{
		ggf::parser::GGFXml xml;
		BYTE *pBuf;
		DWORD nSize; 
		DWORD hfile;

		bool bTemp;

		ggf::parser::VectorInt vi;
		ggf::parser::VectorFloat vf;
		ggf::parser::VectorString vs;
		std::string strTemp;

		hfile = GGFFile_Open(fn,GGFFMODE_READ);
		nSize = GGFFile_GetFileSize(hfile);

		irr::core::stringc strwTemp;
		irr::video::IVideoDriver *pDriver = pDev->getVideoDriver();		

		if(nSize > 0)
		{		
			pBuf = new BYTE[nSize+1];
			GGFFile_Read(hfile,pBuf,nSize);
			pBuf[nSize] = 0x00;
			::GGFFile_Close(hfile);

			if(xml.SetDoc((ggf::parser::LPCTSTR)pBuf) ==false)
			{
				printf("%s\n",xml.GetError().c_str());

			}	

			delete[] pBuf;

			xml.FindElem();
			xml.IntoElem();		

			{
				ggf::object::S_AnimatedMesh ani_mesh;
				xml.FindElem("OcTreeMeshDisc");				
				xml.IntoElem();	

				while(1)
				{
					xml.FindElem("Mesh");					

					ani_mesh.m_strName = xml.GetAttrib("name");
					if(ani_mesh.m_strName == "###")
						break;

					ani_mesh.m_strFile	= xml.GetAttrib("file");
					ani_mesh.m_pMesh	= pSmgr->getMesh(ani_mesh.m_strFile.c_str());									

					m_vt3dOcTreeMesh.push_back(ani_mesh);
				}

				xml.OutOfElem();

				if(pnCounter)
				{
					*pnCounter++;
				}
			}

			{
				ggf::object::S_AnimatedMesh ani_mesh;
				xml.FindElem("AniMeshDisc");				
				xml.IntoElem();	

				while(1)
				{
					xml.FindElem("AniMesh");


					ani_mesh.m_strName = xml.GetAttrib("name");
					if(ani_mesh.m_strName == "###")
						break;

					ani_mesh.m_strFile	= xml.GetAttrib("file");
					ani_mesh.m_pMesh	= pSmgr->getMesh(ani_mesh.m_strFile.c_str());					

					m_vt3dAniMesh.push_back(ani_mesh);

				}

				xml.OutOfElem();
				if(pnCounter)
				{
					*pnCounter++;
				}
			}

			{
				xml.FindElem("ObjectDisc");
				xml.IntoElem();		

				while(1)
				{
					irr::scene::IAnimatedMeshSceneNode *pNode;
					S_DISC_Object obj;
					xml.FindElem("Object");
					strTemp = xml.GetAttrib("name");					

					if(strTemp == "###")
						break;					
					strwTemp = strTemp.c_str();
					obj.m_strName = strTemp;				

					strTemp = xml.GetAttrib("type");
					obj.m_strType = strTemp;

					{
						obj.m_strTexture = xml.GetAttrib("texture");
						obj.m_strMeshName = xml.GetAttrib("obj");

						vf.clear();
						xml.GetAttrib("scale",vf);
						obj.m_v3Scale.set(vf[0],vf[1],vf[2]);
						obj.m_v3Pos.set(0,0,0);
						obj.m_v3Rotation.set(0,0,0);

						pNode = pSmgr->addAnimatedMeshSceneNode(
							FindAniMesh(obj.m_strMeshName.c_str()),
							0);

						pNode->setName(strwTemp.c_str());
						pNode->setMaterialTexture(0,pDriver->getTexture(obj.m_strTexture.c_str()));		
						pNode->setScale(obj.m_v3Scale);
						pNode->updateAbsolutePosition();
						pNode->setVisible(false);

						xml.IntoElem();

						while(1)
						{
							S_Object_Action action;

							xml.FindElem("Action");

							action.m_strName = xml.GetAttrib("name");
							if(action.m_strName == "###")
								break;

							vi.clear();
							xml.GetAttrib("start",vi);
							action.m_nStart = vi[0];

							vi.clear();
							xml.GetAttrib("end",vi);
							action.m_nEnd = vi[0];

							vi.clear();
							xml.GetAttrib("tick",vi);
							action.m_uTick = vi[0];

							obj.m_vtAction.push_back(action);						
						}
						xml.OutOfElem();

						m_vtObjectDefine.push_back(obj);

						if(pnCounter)
						{
							*pnCounter++;
						}
					}

				}
				xml.OutOfElem();

				xml.FindElem("WorldObjDisc");
				xml.IntoElem();	
				{
					irr::scene::ISceneNode *pRoot;					
					irr::scene::ISceneNode *pNode;					

					while(1)
					{
						xml.FindElem("WorldObj");
						strTemp = xml.GetAttrib("name");

						if(strTemp == "###")
							break;
						else 
						{
							pRoot = pSmgr->addEmptySceneNode();
							pRoot->setVisible(true);
							strwTemp = strTemp.c_str();
							pRoot->setName(strwTemp.c_str());
							xml.IntoElem();

							//환경멥
							xml.FindElem("Sky");
							strwTemp = xml.GetAttrib("type").c_str();
							if(strwTemp == "box")
							{								
								vs.clear();
								xml.GetAttrib("textures",vs);
								pNode = pSmgr->addSkyBoxSceneNode(
									pDriver->getTexture(vs[0].c_str()),
									pDriver->getTexture(vs[1].c_str()),
									pDriver->getTexture(vs[2].c_str()),
									pDriver->getTexture(vs[3].c_str()),
									pDriver->getTexture(vs[4].c_str()),
									pDriver->getTexture(vs[5].c_str()),
									pRoot
									);							
								pNode->setName(xml.GetAttrib("name").c_str());
							}
							else if(strwTemp == "dom")
							{
								//xml.FindElem("SkyDom");
								//strwTemp = xml.GetAttrib("name").c_str();
								//if(strwTemp != "###")							
								{
									irr::u32 hRes,vRes;
									irr::f32 texPercentage,spherePercentage;
									vs.clear();
									xml.GetAttrib("texture",vs);
									vi.clear();
									xml.GetAttrib("hRes",vi);
									xml.GetAttrib("vRes",vi);
									vf.clear();
									xml.GetAttrib("texPercentage",vf);
									xml.GetAttrib("spherePercentage",vf);
									hRes = vi[0];
									vRes = vi[1];
									texPercentage = vf[0];
									spherePercentage = vf[1];

									pNode = pSmgr->addSkyDomeSceneNode(
										pDriver->getTexture(vs[0].c_str()),
										hRes,vRes,texPercentage,spherePercentage,																		
										pRoot
										);							
									pNode->setName(xml.GetAttrib("name").c_str());
								}
							}


							xml.FindElem("SkyDom");

							if(pnCounter)
							{
								*pnCounter++;
							}


							//터래인
							while(1)
							{
								xml.FindElem("Terrain");
								strwTemp = xml.GetAttrib("name").c_str();

								if(strwTemp == "###")
									break;

								strTemp = xml.GetAttrib("file");
								pNode = pSmgr->addTerrainSceneNode(strTemp.c_str(),pRoot);
								pNode->setName(strwTemp.c_str());							

								xml.GetAttrib("light",bTemp);
								pNode->setMaterialFlag(irr::video::EMF_LIGHTING, bTemp);

								vs.clear();
								xml.GetAttrib("texture",vs);


								pNode->setMaterialTexture(0,pDriver->getTexture(vs[0].c_str()));															
								if(vs.size() < 2 )
								{

									pNode->setMaterialType(irr::video::EMT_SOLID);
								}
								else
								{
									pNode->setMaterialTexture(1,pDriver->getTexture(vs[1].c_str()));								
									pNode->setMaterialType(irr::video::EMT_DETAIL_MAP);
								}

								xml.GetAttrib("light",bTemp);
								pNode->setMaterialFlag(irr::video::EMF_LIGHTING, bTemp);

								vf.clear();
								xml.GetAttrib("pos",vf);
								pNode->setPosition(irr::core::vector3df(vf[0],vf[1],vf[2]));

								vf.clear();
								xml.GetAttrib("scale",vf);
								pNode->setScale(irr::core::vector3df(vf[0],vf[1],vf[2]));

								vf.clear();
								xml.GetAttrib("rotation",vf);
								pNode->setRotation(irr::core::vector3df(vf[0],vf[1],vf[2]));

								irr::video::SMaterial mat= pNode->getMaterial(0);									

								pNode->updateAbsolutePosition();

								if(pnCounter)
								{
									*pnCounter +=5;
								}
							}

							while(1)
							{
								xml.FindElem("Object");
								strwTemp = xml.GetAttrib("name").c_str();

								if(strwTemp == "###")
								{
									break;
								}
								else
								{									
									pNode = pSmgr->addOctTreeSceneNode(FindOctreeMesh(xml.GetAttrib("obj").c_str()),pRoot);
									pNode->setName(strwTemp.c_str());
									vf.clear();
									xml.GetAttrib("pos",vf);
									pNode->setPosition(irr::core::vector3df(vf[0],vf[1],vf[2]));
									vf.clear();
									xml.GetAttrib("scale",vf);
									pNode->setScale(irr::core::vector3df(vf[0],vf[1],vf[2]));
									vf.clear();
									xml.GetAttrib("rotation",vf);
									pNode->setRotation(irr::core::vector3df(vf[0],vf[1],vf[2]));

									xml.IntoElem();
									xml.FindElem("Material");									
									xml.GetAttrib("light",bTemp);
									pNode->setMaterialFlag(irr::video::EMF_LIGHTING, bTemp);
									strTemp = xml.GetAttrib("type");

									if(strTemp == "EMT_TRANSPARENT_ALPHA_CHANNEL")
									{										
										pNode->setMaterialType(irr::video::EMT_TRANSPARENT_ALPHA_CHANNEL);
										//mat= pNode->getMaterial(0);
									}
									else
									{									
										pNode->setMaterialType(irr::video::EMT_LIGHTMAP);
									}
									xml.OutOfElem();

									

									pNode->updateAbsolutePosition();
								}

								if(pnCounter)
								{
									*pnCounter+=5;
								}
							}

							//라이트
							while(1)
							{
								xml.FindElem("light");
								strwTemp = xml.GetAttrib("name").c_str();

								if(strwTemp == "###")
								{
									break;
								}
								else
								{		
									irr::core::vector3df pos;
									irr::video::SColorf color;
									irr::f32 fRadius;

									vf.clear();
									xml.GetAttrib("radius",vf);
									fRadius = vf[0];

									vf.clear();
									xml.GetAttrib("pos",vf);
									pos.set(vf[0],vf[1],vf[2]);									

									vf.clear();
									xml.GetAttrib("color",vf);
									color.set(vf[0],vf[1],vf[2]);


									pNode = pSmgr->addLightSceneNode(pRoot,pos,color,fRadius);									
									pNode->setName(strwTemp.c_str());

									xml.IntoElem();
									xml.FindElem("attr");
									irr::video::SLight light = ((irr::scene::ILightSceneNode*)pNode)->getLightData();
									
									strTemp = xml.GetAttrib("type");
									if(strTemp == "ELT_DIRECTIONAL")
									{
										light.Type = irr::video::ELT_DIRECTIONAL;
										vf.clear();
										xml.GetAttrib("pos",vf);
										pos.set(vf[0],vf[1],vf[2]);
										light.Position = pos;
									}
									else
									{
										light.Type = irr::video::ELT_POINT;
									}

									xml.OutOfElem();

									

								}

							}


							xml.OutOfElem();
						}

					}

				}
				xml.OutOfElem();
			}

			xml.OutOfElem();			

		}	
		return true;
	}



}
}