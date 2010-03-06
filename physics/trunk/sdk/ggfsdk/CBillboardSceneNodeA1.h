// Copyright (C) 2002-2006 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

//#ifndef __C_BILLBOARD_SCENE_NODE_H_INCLUDED__
//#define __C_BILLBOARD_SCENE_NODE_H_INCLUDED__
#pragma once

#include "IBillboardSceneNode.h"
#include "S3DVertex.h"

namespace irr
{
namespace scene
{
	namespace scene_ggf
{

//! Scene node which is a billboard. A billboard is like a 3d sprite: A 2d element,
//! which always looks to the camera. 
class CBillboardSceneNodeA1 : public IBillboardSceneNode
{
public:

	//! constructor
	CBillboardSceneNodeA1(ISceneNode* parent, ISceneManager* mgr, s32 id,	
		const core::vector3df& position, const core::dimension2d<f32>& size);

	virtual ~CBillboardSceneNodeA1();

	//! pre render event
	//virtual void OnPreRender();
	virtual void OnRegisterSceneNode();

	//! render
	virtual void render();

	//! returns the axis aligned bounding box of this node
	virtual const core::aabbox3d<f32>& getBoundingBox() const;

	//! sets the size of the billboard
	virtual void setSize(const core::dimension2d<f32>& size);

	//! gets the size of the billboard
	virtual const core::dimension2d<f32>& getSize();

	virtual video::SMaterial& getMaterial(s32 i);
	
	//! returns amount of materials used by this scene node.
	virtual irr::u32 getMaterialCount();
	
	//! Writes attributes of the scene node.
	virtual void serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options=0);

	//! Reads attributes of the scene node.
	virtual void deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options=0);

	//! Returns type of the scene node
	virtual ESCENE_NODE_TYPE getType() { return ESNT_BILLBOARD; }

	//도플광어가 추가된내용

	//! 텍스춰좌표설정
	void setTextureCoord(int tx,int ty,int tw,int th);	

	void Init(
		int nTotalFrame,
		irr::core::rect<irr::s32> rtCut,
		irr::core::dimension2di sizeCut,
		irr::c8 *szTexName,
		irr::video::E_MATERIAL_TYPE mtype = irr::video::EMT_TRANSPARENT_ALPHA_CHANNEL
		);

	void Init(ggf::layer::CLayerMng &lm,irr::c8 *szAniTile);
	

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
		if((getTotalFrame()-1) == getCurrentFrame() )
			return true;
		else
			return false;
	}

	void setAniFramePos(int nFrame,irr::core::dimension2d<irr::s32> sizeCut,irr::core::rect<irr::s32> rtCut);
	
	//! 프레임 직접입력
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
		}

		setAniFramePos(nFrame,m_sizeCut,m_rtCut);
	}


	//! 델타틱에대한 프레임계산
	void setAniFrame(irr::u32 uDeltaTick)
	{		
		irr::f32 fDiv;

		fDiv = (irr::f32)((irr::f32)uDeltaTick / (irr::f32)m_uTotalAniTick);

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

	virtual void OnPostRender(irr::u32 timeMS);
	

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


private:	
	core::dimension2d<f32> Size;
	core::aabbox3d<f32> BBox;
	video::SMaterial Material;

	video::S3DVertex vertices[4];
	u16 indices[6];

	//추가내용:doflegangwar
	enum {
		PLAY_STOP = 0,
		PLAY_ONESTOP,
		PLAY_ONESTOPHIDE,
		PLAY_LOOP
	};

	int			m_nFrame;
	int			m_nTotalFrame;
	irr::u32	m_uTotalAniTick;
	irr::u32	m_uTickSave;
	irr::u32	m_uDeltaTick;

	irr::core::dimension2d<irr::s32>	m_sizeCut;
	irr::core::rect<irr::s32>			m_rtCut; //애니메이션이미지를잘라낼영역지정
	int									m_nPlayMode;		
	int									m_nWork;
	
	std::vector<int>	m_vtAniTable;

};


} // end namespace scene
} // end namespace irr
}




