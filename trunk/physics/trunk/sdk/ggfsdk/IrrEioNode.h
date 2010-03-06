#pragma once

//멀티윈도우구현용 WndFrame 노드 추가
//위치업데이트 버그수정 7.14
//알파체널을 위한 그라데이션 효과 버그수정 2006.7.19
//알파체널출력시버그수정 (투명 컬러색 가지는 노드 출력시 구분하여 출력)2006.8.7
//애니메이션시 스케일값이 초기화되는버그수정 2006.8.17
//참조 포인터관련버그수정 2006.8.18
//2008 1.5 : 일리히트 1.4버전 지원

namespace irr
{
namespace scene
{
namespace scene_ggf
{
	
	class CEioSceneNode : public irr::scene::ISceneNode
	{
	protected:
		irr::video::S3DVertex Vertices[4];
		irr::video::SMaterial Material;
		//irr::core::dimension2d<irr::f32> m_ScreenSize;
		irr::core::aabbox3d<irr::f32> Box;

		irr::core::dimension2d<irr::f32> m_sizeTile;


		
	public:		

		CEioSceneNode(irr::scene::ISceneNode* parent, irr::scene::ISceneManager* mgr, irr::s32 id)
			: irr::scene::ISceneNode(parent, mgr, id)
		{ 
			Reset();		

		}		

		~CEioSceneNode()
		{

		}

		void Reset()
		{
			Material.Wireframe = false;
			Material.Lighting = false;			

			irr::core::vector3df normal = irr::core::vector3df(0.0f,0.0f,-1.0f);			

			Vertices[0] = irr::video::S3DVertex( irr::core::vector3df(.0f,.0f,0.0f),normal,irr::video::SColor(0,255,255,255),irr::core::vector2d<irr::f32>(0.f,0.f));
			Vertices[1] = irr::video::S3DVertex( irr::core::vector3df(1.0f,.0f,0.0f),normal,irr::video::SColor(0,255,255,255),irr::core::vector2d<irr::f32>(1.f,0.f)); 
			Vertices[2] = irr::video::S3DVertex( irr::core::vector3df(1.0f,-1.0f,0.0f),normal,irr::video::SColor(0,255,255,255),irr::core::vector2d<irr::f32>(1.f,1.f));
			Vertices[3] = irr::video::S3DVertex( irr::core::vector3df(0.0f,-1.0f,0.0f),normal,irr::video::SColor(0,255,255,255),irr::core::vector2d<irr::f32>(0.f,1.f));

			Box.reset(Vertices[0].Pos);
			for (irr::s32 i=1; i<4; ++i)
				Box.addInternalPoint(Vertices[i].Pos);

		}


		/*
		참조 포인터설정
		찾모포인터란?: 스프라이트를 관리하는데 기준이 되는점 출력,회전 확대축소시 기준으로 삼는다.
		*/
		void setReferancePoint(int rx,int ry)
		{
			irr::f32 fRx,fRy;

			fRx = ((irr::f32)rx) / m_sizeTile.Width;
			fRy = ((irr::f32)ry) / m_sizeTile.Height;

			Vertices[0].Pos = irr::core::vector3df(.0f,.0f,0.0f) + irr::core::vector3df(-fRx,fRy,0);
			Vertices[1].Pos = irr::core::vector3df(1.0f,.0f,0.0f) + irr::core::vector3df(-fRx,fRy,0);
			Vertices[2].Pos = irr::core::vector3df(1.0f,-1.0f,0.0f) + irr::core::vector3df(-fRx,fRy,0);
			Vertices[3].Pos = irr::core::vector3df(0.0f,-1.0f,0.0f) + irr::core::vector3df(-fRx,fRy,0);
			
			Box.reset(Vertices[0].Pos);
			for (irr::s32 i=1; i<4; ++i)
				Box.addInternalPoint(Vertices[i].Pos);
			
		}

		//중점을 리퍼런스포인터로설정한다
		void setReferancePoint_center()
		{
			setReferancePoint((irr::s32)m_sizeTile.Width/2,(irr::s32)m_sizeTile.Height/2);
		}

		virtual void setTextureCoord(int tx,int ty,int tw,int th,irr::video::ITexture *pTex = NULL)
		{
			float _tx,_ty,_th,_tw;

			if(pTex == NULL)
			{
				//pTex = Material.Textures[0];			
				pTex = getMaterial(0).getTexture(0);

			}

			if(pTex)
			{
				_tx = (irr::f32)((irr::f32)tx / (irr::f32)pTex->getSize().Width);
				_ty = (irr::f32)((irr::f32)ty / (irr::f32)pTex->getSize().Height);
				_tw = (irr::f32)((irr::f32)tw / (irr::f32)pTex->getSize().Width);
				_th = (irr::f32)((irr::f32)th / (irr::f32)pTex->getSize().Height);

				Vertices[0].TCoords = irr::core::vector2d<irr::f32>(_tx,_ty);
				Vertices[1].TCoords = irr::core::vector2d<irr::f32>(_tx + _tw,_ty);
				Vertices[2].TCoords = irr::core::vector2d<irr::f32>(_tx + _tw,_ty + _th);
				Vertices[3].TCoords = irr::core::vector2d<irr::f32>(_tx,_ty + _th);

				m_sizeTile = irr::core::dimension2d<irr::f32>((irr::f32)tw,(irr::f32)th);
				//set2DScale(1.f,1.f);
			}
		}

		void setTextureCoord(int tx,int ty,int tw,int th,irr::c8 *szTexName)
		{
			setTextureCoord(tx,ty,tw,th,SceneManager->getVideoDriver()->getTexture(szTexName));
		}		

		void Init(int tx,int ty,int tw,int th,irr::c8 *szTexName,irr::video::E_MATERIAL_TYPE mtype = irr::video::EMT_TRANSPARENT_ALPHA_CHANNEL)
		{
			irr::video::ITexture *pTex = SceneManager->getVideoDriver()->getTexture(szTexName);
			Init(tx,ty,tw,th,pTex,mtype);
		}

		virtual void Init(int tx,int ty,int tw,int th,irr::video::ITexture *pTex,irr::video::E_MATERIAL_TYPE mtype = irr::video::EMT_TRANSPARENT_ALPHA_CHANNEL)
		{
			Reset();
			setMaterialTexture(0,pTex);
			setMaterialType(mtype);		

			getMaterial(0).MaterialTypeParam = 0.01f; //알파체널에 더해지는값을 아주 작은값으로설정

			setTextureCoord(tx,ty,tw,th,pTex);		

			setScale(irr::core::vector3df((irr::f32)tw,(irr::f32)th,0.f));
			
		}

		
		void Init(ggf::layer::S_TileLayer *pLayer,irr::video::E_MATERIAL_TYPE mtype = irr::video::EMT_TRANSPARENT_ALPHA_CHANNEL)
		{
			Init(
				pLayer->m_CutRect[0],pLayer->m_CutRect[1],pLayer->m_CutRect[2],pLayer->m_CutRect[3],
				(irr::c8 *)pLayer->m_strTexName.c_str(),
				mtype
				);
		}
		

		/*
		3디 좌표계를 2디 스크린좌표계로변환	
		*/
		irr::core::vector3df ScreenToWorld(irr::f32 x,irr::f32 y,irr::f32 z)
		{			
			irr::core::dimension2d<irr::s32> dmScreen = SceneManager->getVideoDriver()->getScreenSize();
			return irr::core::vector3df((irr::f32)(x - (dmScreen.Width/2)),(irr::f32)((dmScreen.Height/2) - y ),z);
		}

		void set2DScreenPosition(irr::core::position2d<irr::s32> pos,irr::f32 ZDepth)
		{
			irr::core::vector3df vecScreen = ScreenToWorld((irr::f32)pos.X,(irr::f32)pos.Y,(irr::f32)ZDepth);
			setPosition(vecScreen);
			updateAbsolutePosition();		
		}			

		void set2DScreenPosition(irr::s32 x,irr::s32 y,irr::f32 zDep = 0.f)
		{
			set2DScreenPosition(irr::core::position2d<irr::s32>(x,y),zDep);			
		}

		void set2DScreenPosition(irr::core::vector2di v2Pos,irr::f32 zDep = 0.f)
		{
			set2DScreenPosition(irr::core::position2d<irr::s32>(v2Pos.X,v2Pos.Y),zDep);	

		}

		void set2DScale(irr::f32 sclx,irr::f32 scly)
		{
			//irr::core::vector3df oriScale = m_sizeTile;
			setScale(irr::core::vector3df(m_sizeTile.Width*sclx,m_sizeTile.Height*scly,0.f));
		}

		irr::core::position2d<irr::s32> get2DScreenPosition()
		{
			irr::core::dimension2d<irr::s32> dmScreen = SceneManager->getVideoDriver()->getScreenSize();
			irr::core::vector3df pos = getPosition();
			
			return irr::core::position2d<irr::s32>((irr::s32)(pos.X + (dmScreen.Width/2)),(irr::s32)((dmScreen.Height/2) - pos.Y ));
		}

		void get2DScreenPosition(irr::core::vector3df &v3pos)
		{
			irr::core::vector3df pos = getPosition();
			v3pos = irr::core::vector3df((irr::f32)get2DScreenPosition().X,(irr::f32)get2DScreenPosition().Y,pos.Z);
		}

		void get2DScreenPosition(irr::core::vector3di &v3pos)
		{
			irr::core::vector3df pos = getPosition();
			v3pos = irr::core::vector3di(get2DScreenPosition().X,get2DScreenPosition().Y,(irr::s32)pos.Z);
		}

		void get2DScreenPosition(irr::core::vector2di &v2pos)
		{
			//irr::core::vector3df pos = getPosition();
			v2pos = irr::core::vector2di(get2DScreenPosition().X,get2DScreenPosition().Y);
		}

		void get2DScreenPosition(irr::core::vector2df &v2pos)
		{
			//irr::core::vector3df pos = getPosition();
			v2pos = irr::core::vector2df((irr::f32)get2DScreenPosition().X,(irr::f32)get2DScreenPosition().Y);
		}

		
		/*
		irr::core::rect<irr::f32> getBoundRect()
		{
			irr::core::rect<irr::f32> boundRect = irr::core::rect<irr::f32>(Box.MinEdge.X,Box.MinEdge.Y,Box.MaxEdge.X,Box.MaxEdge.Y);

			boundRect.repair();
			return boundRect;
			//return irr::core::rect<irr::f32>(Box.MinEdge,Box.MaxEdge);
		}
		*/


		//virtual void OnPreRender()
		virtual void OnRegisterSceneNode()
		{  
			if (IsVisible)
			{
				//왜 이렇게 하냐하면..투명기능이있는 면과 그렇지않은면이있을때
				//이런식으로 구분시키지않으면 알파값이 이상하게나올수있음
				// because this node supports rendering of mixed mode meshes consisting of 
				// transparent and solid material at the same time, we need to go through all 
				// materials, check of what type they are and register this node for the right
				// render pass according to that.

				irr::video::IVideoDriver* driver = SceneManager->getVideoDriver();

				irr::video::IMaterialRenderer* rnd = 
						driver->getMaterialRenderer(Material.MaterialType);
				if(rnd && rnd->isTransparent())
				{
					SceneManager->registerNodeForRendering(this, scene::ESNRP_TRANSPARENT);
				}
				else
				{
					SceneManager->registerNodeForRendering(this, scene::ESNRP_SOLID);
				}
			}

			//ISceneNode::OnPreRender();
			ISceneNode::OnRegisterSceneNode();
	/*
			if (IsVisible)    
			{
				SceneManager->registerNodeForRendering(this);
			}

			//printf("prerender\n");

			ISceneNode::OnPreRender();
			*/
		}

		

		virtual void render()
		{  	
			if (IsVisible == false) return;
			
			//printf("render %d \n",getID());

			irr::u16 indices[] = { 0,1,2,3,0,2 };
			irr::video::IVideoDriver* driver = SceneManager->getVideoDriver();

			//SceneManager->setActiveCamera((irr::scene::ICameraSceneNode *) SceneManager->getSceneNodeFromId(0));
			//driver->draw2DLine(irr::core::position2d<irr::s32>(0,0),irr::core::position2d<irr::s32>(100,0));			
			//driver->draw3DLine(irr::core::vector3df(0.,0.,0.),irr::core::vector3df(100.,100.,0.));			
			
			

			driver->setMaterial(Material);			
			
			irr::core::matrix4 _TempTrans;
			_TempTrans = driver->getTransform(irr::video::ETS_WORLD);
			
			driver->setTransform(irr::video::ETS_WORLD, AbsoluteTransformation);			
			driver->drawIndexedTriangleList(&Vertices[0], 4, &indices[0], 2);

			//driver->draw3DLine(Vertices[0].Pos,Vertices[1].Pos);
			
			if(DebugDataVisible)
			{//디버깅정보출력
				irr::video::SMaterial _Material;


				_Material.Lighting = false;
				_Material.Wireframe = false;				

				driver->setMaterial(_Material);			
				irr::core::aabbox3df box1 = getBoundingBox();
				//irr::core::aabbox3df box2 = getTransformedBoundingBox();			
				driver->draw3DBox(box1);				
				//driver->draw3DBox(box2);
			}
			driver->setTransform(irr::video::ETS_WORLD,_TempTrans);		//월드변환복귀	
			//SceneManager->setActiveCamera((irr::scene::ICameraSceneNode *) SceneManager->getSceneNodeFromId(1));

		}

		/*
		virtual void OnPostRender(irr::u32 timeMS)
		{
			//printf("post render %d\n",getID());
			//printf("%d \n",timeMS);
			//
			ISceneNode::OnPostRender(timeMS);
		}
		*/

		virtual const irr::core::aabbox3d<irr::f32>& getBoundingBox() const  
		{    
			return Box;  
		}
		virtual irr::u32 getMaterialCount() const		
		//virtual irr::u32 getMaterialCount()
		{
			return 1;
		}
		virtual irr::video::SMaterial& getMaterial(irr::u32 i)
		{
			return Material;
		}
		



		irr::video::ITexture *getMatrialTexture()
		{
			//return Material.Textures[0];
			return getMaterial(0).getTexture(0);
		}

	};

	/*
	 텍스춰전체영역을 주어진크기로 일정하게 나누어서 프래임을구분하는 애니매이션클래스
	 첫번째프레임은 위치 텍스춰 상에서 0,0에서 정해진크기만큼잘라낸 이미지이다.
	*/
	class CEioSceneNodeA1 : public CEioSceneNode
	{
		int m_nFrame;
		int m_nTotalFrame;
		irr::u32 m_uTotalAniTick;
		irr::u32 m_uTickSave;
		irr::u32 m_uDeltaTick;

		irr::core::dimension2d<irr::s32> m_sizeCut;
		irr::core::rect<irr::s32> m_rtCut; //애니메이션이미지를잘라낼영역지정
		int m_nPlayMode;		
		int m_nWork;

		std::vector<int>	m_vtAniTable;

	public:	

		enum {
			PLAY_STOP = 0,
			PLAY_ONESTOP,
			PLAY_ONESTOPHIDE,
			PLAY_LOOP
		};


		CEioSceneNodeA1(irr::scene::ISceneNode* parent, irr::scene::ISceneManager* mgr, irr::s32 id) :CEioSceneNode(parent,mgr,id)
		{
		}
		~CEioSceneNodeA1()
		{
		}

		/*
		rtCut 애니 작업 유효이미지 영역
		sizeCut 프레임 이미지 크기
		*/

		void Init(int nTotalFrame,irr::core::rect<irr::s32> rtCut,irr::core::dimension2d<irr::s32> sizeCut,irr::c8 *szTexName,irr::video::E_MATERIAL_TYPE mtype = irr::video::EMT_TRANSPARENT_ALPHA_CHANNEL)
		{
			m_sizeCut	= sizeCut;
			m_nFrame	= 0;
			m_nTotalFrame = nTotalFrame;			
			m_rtCut		= rtCut;

			CEioSceneNode::Init(
				rtCut.UpperLeftCorner.X,
				rtCut.UpperLeftCorner.Y,
				sizeCut.Width,sizeCut.Height,szTexName,mtype);//첫번째프레임에위치시킴
			setAniMode(PLAY_STOP);
			setAniTotalTick(1000);

		}

		void Init(int nTotalFrame,irr::core::dimension2d<irr::s32> sizeCut,irr::c8 *szTexName,irr::video::E_MATERIAL_TYPE mtype = irr::video::EMT_TRANSPARENT_ALPHA_CHANNEL)
		{
			m_sizeCut = sizeCut;
			m_nFrame = 0;
			m_nTotalFrame = nTotalFrame;			

			CEioSceneNode::Init(0,0,sizeCut.Width,sizeCut.Height,szTexName,mtype);//첫번째프레임에위치시킴			

			irr::video::ITexture *pTex = getMatrialTexture();
			m_rtCut = irr::core::rect<irr::s32>(0,0,pTex->getSize().Width-1,pTex->getSize().Height-1);
			setAniMode(PLAY_STOP);
			setAniTotalTick(1000);
		}		

		void Init(ggf::layer::S_AniTileLayer *pAniTile,irr::video::E_MATERIAL_TYPE mtype = irr::video::EMT_TRANSPARENT_ALPHA_CHANNEL)
		{

			Init(
				pAniTile->m_nTotalFrame,
				pAniTile->m_rectCut,
				irr::core::dimension2d<irr::s32>(pAniTile->m_rectFrame.getWidth(),pAniTile->m_rectFrame.getHeight()),
				(irr::c8 *)pAniTile->m_strTexName.c_str(),
				mtype
				);
		}

		irr::s32 getTotalFrame()
		{
			if(m_vtAniTable.size())
			{
				return (int)m_vtAniTable.size();
			}
			return m_nTotalFrame;
		}

		irr::s32 getCurrentFrame()
		{
			return m_nFrame;
		}

		//애니메이숀이끝났는가?
		bool isEndAnimation()
		{
			if((m_nTotalFrame-1) == m_nFrame)
				return true;
			else
				return false;
		}

		

		void setAniFramePos(int nFrame,irr::core::dimension2d<irr::s32> sizeCut,irr::core::rect<irr::s32> rtCut)
		{
			m_nFrame = nFrame;
			//irr::video::ITexture *pTex;
			//pTex = getMatrialTexture();
			
			int fx;// = 
			int fy;// = pTile->m_rectCut.UpperLeftCorner.Y;
			int a1;//가로넘침 계산용

			a1 = sizeCut.Width * nFrame;

			//if( a1 >= pTile->m_rectCut.getWidth()) //세로 위치밖으로벗어나있으면
			//if(a1 >= pTex->getSize().Width ) //가로 넘침이 발생하면
			if(a1 >= rtCut.getWidth() ) //가로 넘침이 발생하면
			{
				int nTemp;
				int nGridY;
				//nTemp	= a1 % pTex->getSize().Width;
				//nGridY	= a1 / pTex->getSize().Width;
				nTemp	= a1 % rtCut.getWidth();
				nGridY	= a1 / rtCut.getWidth();

				fx = nTemp + rtCut.UpperLeftCorner.X;
				fy = (nGridY * sizeCut.Height) + rtCut.UpperLeftCorner.Y;
			}
			else
			{
				fx = a1 + rtCut.UpperLeftCorner.X;
				fy = 0  + rtCut.UpperLeftCorner.Y;		
			}

			setTextureCoord(fx,fy,sizeCut.Width,sizeCut.Height);

			//irr::core::rect<s32> rect = pTile->m_rectFrame + irr::core::position2d<s32>(fx,fy);
		}

		void setAniFrame(int nFrame)
		{			
			irr::s32 nSize = (irr::s32)m_vtAniTable.size();//애니 테이블싸이즈

			if(nSize)
			{
				if(nFrame > nSize )
				{
					nFrame = nSize-1;
				}			

				nFrame = m_vtAniTable[nFrame]; //인덱싱변환
				//printf("%d \n",nFrame);
			}
			
			setAniFramePos(nFrame,m_sizeCut,m_rtCut);
		}

		void setAniFrame(irr::u32 uDeltaTick)
		{
			irr::f32 fDiv;

			if(m_uTotalAniTick > 0)
				fDiv = (irr::f32)((irr::f32)uDeltaTick / (irr::f32)m_uTotalAniTick);
			else
				fDiv = 0;


			if( fDiv > 1.0)
			{
				fDiv = 1.0;
			}			
			
			setAniFrame((int)(fDiv*(getTotalFrame() - 1)));
			//setAniFrame((int)(fDiv*(m_nTotalFrame-1)));			
		}

		void setAniMode(int nPlayMode)
		{
			setVisible(true);
			m_nWork = 0;
			m_nPlayMode = nPlayMode;
		}

		void setAniTotalTick(irr::u32 uTick)
		{
			m_uTotalAniTick = uTick;
		}		

		virtual void OnAnimate(irr::u32 timeMS)		
		//virtual void OnPostRender(irr::u32 timeMS)
		{
			//printf("post render %d\n",getID());

			if(IsVisible )
			{
				switch(m_nPlayMode)
				{
				case PLAY_STOP:
					break;
				case PLAY_ONESTOP:
					switch(m_nWork)
					{
					case 0:
						m_uTickSave = timeMS;					
						m_nWork = 10;
						break;
					case 10:
						m_uDeltaTick = timeMS - m_uTickSave;					
						setAniFrame(m_uDeltaTick);
						if(m_uDeltaTick >= m_uTotalAniTick)
							m_nWork = 100;
						break;
					case 100:
						break;
					}
					break;
				case PLAY_ONESTOPHIDE:
					//printf("%d,%d \n",m_nFrame,m_nWork);
					switch(m_nWork)
					{
					case 0:
						m_uTickSave = timeMS;					
						m_nWork = 10;
						break;
					case 10:
						//printf("%d\n",getID());
						m_uDeltaTick = timeMS - m_uTickSave;
						setAniFrame(m_uDeltaTick);
						if(m_uDeltaTick >= m_uTotalAniTick)
							m_nWork = 100;
						break;
					case 100:
						setVisible(false);
						break;
					}			
					break;
				case PLAY_LOOP:
					switch(m_nWork)
					{
					case 0:
						m_uTickSave = timeMS;					
						m_nWork = 10;
						break;
					case 10:
						m_uDeltaTick = timeMS - m_uTickSave;
						//m_uTickSave = timeMS;		
						setAniFrame(m_uDeltaTick);
						if(m_uDeltaTick >= m_uTotalAniTick)
						{
							m_uTickSave = timeMS;								
							m_nWork = 20;

						}
						break;
					case 20:
						m_uDeltaTick = timeMS - m_uTickSave;
						if(m_uDeltaTick > (m_uTotalAniTick/m_nTotalFrame) )
						{
							m_nWork = 0;
						}					
						break;

					}
					break;				
				}
			}



			{
				//printf("post render %d\n",getID());

				if(IsVisible )
				{
					switch(m_nPlayMode)
					{
					case PLAY_STOP:
						break;
					case PLAY_ONESTOP:
						switch(m_nWork)
						{
						case 0:
							m_uTickSave = timeMS;					
							m_nWork = 10;
							break;
						case 10:
							m_uDeltaTick = timeMS - m_uTickSave;					
							setAniFrame(m_uDeltaTick);
							if(m_uDeltaTick >= m_uTotalAniTick)
								m_nWork = 100;
							break;
						case 100:
							break;
						}
						break;
					case PLAY_ONESTOPHIDE:
						//printf("%d,%d \n",m_nFrame,m_nWork);
						switch(m_nWork)
						{
						case 0:
							m_uTickSave = timeMS;					
							m_nWork = 10;
							break;
						case 10:
							//printf("%d\n",getID());
							m_uDeltaTick = timeMS - m_uTickSave;
							setAniFrame(m_uDeltaTick);
							if(m_uDeltaTick >= m_uTotalAniTick)
								m_nWork = 100;
							break;
						case 100:
							setVisible(false);
							break;
						}			
						break;
					case PLAY_LOOP:
						switch(m_nWork)
						{
						case 0:
							m_uTickSave = timeMS;					
							m_nWork = 10;
							break;
						case 10:
							m_uDeltaTick = timeMS - m_uTickSave;
							//m_uTickSave = timeMS;		
							setAniFrame(m_uDeltaTick);
							if(m_uDeltaTick >= m_uTotalAniTick)
							{
								m_uTickSave = timeMS;								
								m_nWork = 20;

							}
							break;
						case 20:
							m_uDeltaTick = timeMS - m_uTickSave;
							if(m_uDeltaTick > (m_uTotalAniTick/m_nTotalFrame) )
							{
								m_nWork = 0;
							}					
							break;

						}
						break;				
					}
				}

				//ISceneNode::OnPostRender(timeMS);
			}


			irr::scene::ISceneNode::OnAnimate(timeMS);
			//ISceneNode::OnPostRender(timeMS);
		}

		void playOneStopAnimation(irr::u32 uTotalTick)
		{
			setAniMode(PLAY_ONESTOP);
			setAniTotalTick(uTotalTick);
		}

		void playOneStopHideAnimation(irr::u32 uTotalTick)
		{
			setAniMode(PLAY_ONESTOPHIDE);
			setAniTotalTick(uTotalTick);
		}

		void playLoopAnimation(irr::u32 uTotalTick)
		{
			setAniMode(PLAY_LOOP);
			setAniTotalTick(uTotalTick);
		}

		void stopAnimation()
		{
			setAniMode(PLAY_STOP);
		}

		//! 인덱싱모드 애니메 추가
		void addFrameIndex(int nIndex)
		{
			m_vtAniTable.push_back(nIndex);
		}

		void resetFrameIndex()
		{
			m_vtAniTable.clear();
		}


	};

	/*
	텍스춰에 랜더링하여 화면에보여줄경우 사용하는 2.5디노드
	*/
	class CEioNodeWndFrame : public CEioSceneNode
	{
	public:
		CEioNodeWndFrame(irr::scene::ISceneNode* parent, irr::scene::ISceneManager* mgr, irr::s32 id) :CEioSceneNode(parent,mgr,id)
		{
		}
		~CEioNodeWndFrame()
		{
		}

		void setTextureCoord(int tx,int ty,int tw,int th,irr::video::ITexture *pTex = NULL)
		{
			float _tx,_ty,_th,_tw;

			if(pTex == NULL)
			{
				pTex = getMaterial(0).getTexture(0);
				//pTex = Material.Textures[0];
			}

			if(pTex)
			{
				_tx = (irr::f32)((irr::f32)tx / (irr::f32)pTex->getSize().Width);
				_ty = (irr::f32)((irr::f32)ty / (irr::f32)pTex->getSize().Height);
				_tw = (irr::f32)((irr::f32)tw / (irr::f32)pTex->getSize().Width);
				_th = (irr::f32)((irr::f32)th / (irr::f32)pTex->getSize().Height);
				
				
				
				Vertices[3].TCoords = irr::core::vector2d<irr::f32>(_tx,_ty);
				Vertices[2].TCoords = irr::core::vector2d<irr::f32>(_tx + _tw,_ty);
				Vertices[1].TCoords = irr::core::vector2d<irr::f32>(_tx + _tw,_ty + _th);
				Vertices[0].TCoords = irr::core::vector2d<irr::f32>(_tx,_ty + _th);

				m_sizeTile = irr::core::dimension2d<irr::f32>((irr::f32)tw,(irr::f32)th);
				set2DScale(1.f,1.f);
			}
		}

		void Init(int tx,int ty,int tw,int th,irr::video::ITexture *pTex,irr::video::E_MATERIAL_TYPE mtype = irr::video::EMT_TRANSPARENT_ALPHA_CHANNEL)
		{
			setMaterialTexture(0,pTex);
			setMaterialType(mtype);			

			setTextureCoord(tx,ty,tw,th,pTex);		

			setScale(irr::core::vector3df((irr::f32)tw,(irr::f32)th,0.f));
			
		}


	};

}
}
}
