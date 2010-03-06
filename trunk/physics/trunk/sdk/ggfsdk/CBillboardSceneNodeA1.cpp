//작성자 도플광어
//일리히트엔진용 애니메이션 빌보드
//최초작성 2006.8.21

/*
예제

irr::scene::scene_ggf::CBillboardSceneNodeA1 *pBillNodeA1;
pBillNodeA1 = new irr::scene::scene_ggf::CBillboardSceneNodeA1(
	씬메니져->getRootSceneNode(),
	씬메니져,
	-1,
	irr::core::vector3df(0,0,-200),
	irr::core::dimension2df(10,10)
	);
pBillNodeA1->Init(CkogG5App::m_System.m_LayerMng,"ani_reelicon");		
pBillNodeA1->setMaterialFlag(irr::video::EMF_LIGHTING,false);		
pBillNodeA1->playLoopAnimation(11000);

pBillNodeA1->drop();
*/

#include <string>
#include <vector>
#include <irrlicht.h>
#include "ggfsdk.h"
#include "CBillboardSceneNodeA1.h"



namespace irr
{
namespace scene
{
namespace scene_ggf
{

//! constructor
CBillboardSceneNodeA1::CBillboardSceneNodeA1(ISceneNode* parent, ISceneManager* mgr, s32 id,
const core::vector3df& position, const core::dimension2d<f32>& size)
: IBillboardSceneNode(parent, mgr, id, position)
{
	#ifdef _DEBUG
	setDebugName("CBillboardSceneNodeA1");
	#endif

	setSize(size);

	indices[0] = 0;
	indices[1] = 2;
	indices[2] = 1;
	indices[3] = 0;
	indices[4] = 3;
	indices[5] = 2;

	vertices[0].TCoords.set(1.0f, 1.0f);
	vertices[0].Color.set(0xffffffff);

	vertices[1].TCoords.set(1.0f, 0.0f);
	vertices[1].Color.set(0xffffffff);

	vertices[2].TCoords.set(0.0f, 0.0f);
	vertices[2].Color.set(0xffffffff);

	vertices[3].TCoords.set(0.0f, 1.0f);
	vertices[3].Color.set(0xffffffff);
}



CBillboardSceneNodeA1::~CBillboardSceneNodeA1()
{
}



//! pre render event
//void CBillboardSceneNodeA1::OnPreRender()
void CBillboardSceneNodeA1::OnRegisterSceneNode()
{
	if (IsVisible)
	{
		SceneManager->registerNodeForRendering(this);
		ISceneNode::OnRegisterSceneNode();
	}
}


//! render
void CBillboardSceneNodeA1::render()
{
	if(IsVisible)
	{
		video::IVideoDriver* driver = SceneManager->getVideoDriver();
		ICameraSceneNode* camera = SceneManager->getActiveCamera();

		if (!camera || !driver)
			return;

		// make billboard look to camera

		core::vector3df pos = getAbsolutePosition();

		core::vector3df campos = camera->getAbsolutePosition();
		core::vector3df target = camera->getTarget();
		core::vector3df up = camera->getUpVector();
		core::vector3df view = target - campos;
		view.normalize();

		core::vector3df horizontal = up.crossProduct(view);
		horizontal.normalize();

		if ( horizontal.getLength() == 0 )
		{
			horizontal.set(up.Y,up.X,up.Z);
			horizontal.normalize();
		}

		core::vector3df vertical = horizontal.crossProduct(view);

		view *= -1.0f;

		for (s32 i=0; i<4; ++i)
			vertices[i].Normal = view;

		horizontal *= 0.5f * Size.Width;
		vertical.normalize();
		vertical *= 0.5f * Size.Height;

		vertices[0].Pos = pos + horizontal + vertical;
		vertices[1].Pos = pos + horizontal - vertical;
		vertices[2].Pos = pos - horizontal - vertical;
		vertices[3].Pos = pos - horizontal + vertical;

		// draw

		if (DebugDataVisible)
		{
			driver->setTransform(video::ETS_WORLD, AbsoluteTransformation);
			video::SMaterial m;
			m.Lighting = false;
			driver->setMaterial(m);
			driver->draw3DBox(BBox, video::SColor(0,208,195,152));
		}

		core::matrix4 mat;
		driver->setTransform(video::ETS_WORLD, mat);

		driver->setMaterial(Material);

		driver->drawIndexedTriangleList(vertices, 4, indices, 2);
	}
}


//! returns the axis aligned bounding box of this node
const core::aabbox3d<f32>& CBillboardSceneNodeA1::getBoundingBox() const
{
	return BBox;
}


//! sets the size of the billboard
void CBillboardSceneNodeA1::setSize(const core::dimension2d<f32>& size)
{
	Size = size;

	if (Size.Width == 0.0f)
		Size.Width = 1.0f;

	if (Size.Height == 0.0f )
		Size.Height = 1.0f;

	f32 avg = (size.Width + size.Height)/6;
	BBox.MinEdge.set(-avg,-avg,-avg);
	BBox.MaxEdge.set(avg,avg,avg);
}


video::SMaterial& CBillboardSceneNodeA1::getMaterial(s32 i)
{
	return Material;
}


//! returns amount of materials used by this scene node.
irr::u32 CBillboardSceneNodeA1::getMaterialCount()
{
	return 1;
}


//! gets the size of the billboard
const core::dimension2d<f32>& CBillboardSceneNodeA1::getSize()
{
	return Size;
}


//! Writes attributes of the scene node.
void CBillboardSceneNodeA1::serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options)
{
	IBillboardSceneNode::serializeAttributes(out, options);

	out->addFloat("Width", Size.Width);
	out->addFloat("Height", Size.Height);
}

//! Reads attributes of the scene node.
void CBillboardSceneNodeA1::deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options)
{
	IBillboardSceneNode::deserializeAttributes(in, options);

	Size.Width = in->getAttributeAsFloat("Width");
	Size.Height = in->getAttributeAsFloat("Height");

	setSize(Size);
}



void CBillboardSceneNodeA1::setTextureCoord(int tx, int ty, int tw, int th)
{	
	float _tx,_ty,_th,_tw;

	irr::video::ITexture *pTex = this->getMaterial(0).Textures[0];		

	if(pTex)
	{
		_tx = (irr::f32)((irr::f32)tx / (irr::f32)pTex->getSize().Width);
		_ty = (irr::f32)((irr::f32)ty / (irr::f32)pTex->getSize().Height);
		_tw = (irr::f32)((irr::f32)tw / (irr::f32)pTex->getSize().Width);
		_th = (irr::f32)((irr::f32)th / (irr::f32)pTex->getSize().Height);

		vertices[2].TCoords.set(irr::core::vector2d<irr::f32>(_tx,_ty));
		vertices[1].TCoords.set(irr::core::vector2d<irr::f32>(_tx + _tw,_ty));
		vertices[0].TCoords.set(irr::core::vector2d<irr::f32>(_tx + _tw,_ty + _th));
		vertices[3].TCoords.set(irr::core::vector2d<irr::f32>(_tx,_ty + _th));		
	}
}

void CBillboardSceneNodeA1::Init(int nTotalFrame, irr::core::rect<irr::s32> rtCut, irr::core::dimension2di sizeCut, irr::c8 *szTexName, irr::video::E_MATERIAL_TYPE mtype)
{
	m_sizeCut	= sizeCut;
	m_nFrame	= 0;
	m_nTotalFrame = nTotalFrame;			
	m_rtCut		= rtCut;

	getMaterial(0).MaterialTypeParam = 0.01f;
	setMaterialType(mtype);
	setTextureCoord(rtCut.UpperLeftCorner.X,rtCut.UpperLeftCorner.Y,sizeCut.Width,sizeCut.Height);

	setMaterialTexture(0,SceneManager->getVideoDriver()->getTexture(szTexName));
	setAniMode(PLAY_STOP);
	setAniTotalTick(1000);
	setAniFrame(0);
}

void CBillboardSceneNodeA1::Init(ggf::layer::CLayerMng &lm, irr::c8 *szAniTile)
{
	ggf::layer::S_AniTileLayer *pAniTile = lm.FindAniTile(szAniTile);

	Init(pAniTile->m_nTotalFrame,pAniTile->m_rectCut,
		irr::core::dimension2di(pAniTile->m_rectFrame.getWidth(),pAniTile->m_rectFrame.getHeight()),
		(irr::c8 *)pAniTile->m_strTexName.c_str()
	);
}

void CBillboardSceneNodeA1::setAniFramePos(int nFrame, irr::core::dimension2d<irr::s32> sizeCut, irr::core::rect<irr::s32> rtCut)
{
	m_nFrame = nFrame;

	int fx;
	int fy;
	int a1;//가로넘침 계산용

	a1 = sizeCut.Width * nFrame;
	
	if(a1 >= rtCut.getWidth() ) //가로 넘침이 발생하면
	{
		int nTemp;
		int nGridY;		
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
}

void CBillboardSceneNodeA1::OnPostRender(irr::u32 timeMS)
{

	if(IsVisible)
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

	//ISceneNode::OnPostRender(timeMS);//
}



} // end namespace scene
} // end namespace irr
}

