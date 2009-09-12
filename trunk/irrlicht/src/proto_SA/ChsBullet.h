#pragma once
#include "FSMObject.h"
//탄 오브잭트
class ChsBullet : public IFSMObject
{
public:

	struct Params
	{
		irr::core::vector3df vInitPos; // 시작점
		irr::f32 Energy;		//탄도 에너지
		irr::core::vector3df vAim; //총구 방향
		irr::f32 mass;//질량		
	};

	enum {
		FSM_READY,
		FSM_MOVE,
		FSM_HIT,
		FSM_DIE,		
		FSM_COUNT
	};

	ChsBullet(void);
	virtual ~ChsBullet(void);

	irr::scene::ISceneNode *m_pNode;
	irr::scene::CBulletObjectAnimator *m_pAnim;

	//! 리턴값 0: 원인모를 에러, 1: 성공
	virtual int Init(void *pParam);

	inline virtual irr::core::vector3df getPosition() {return m_pNode->getPosition();}
	inline virtual bool isDie() {if(GetStatus() == FSM_DIE)return true;else return false;}

	virtual void Signal(std::string strSignal,void *pParam);// {}	//상태변환등을처리한다.
	virtual void Update(irr::f32 fTick); //게임오브잭트로직처리한다.
	
};
