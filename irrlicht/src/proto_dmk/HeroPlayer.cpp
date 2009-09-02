#include "CdmkApp.h"

#include "HeroPlayer.h"

/*

해결해야할문제점
1. 캐릭터 컨트롤러의 터널링문제 - 사양이 낮은 곳에서 터널링이발생할수있다.
해결방법은 이동방향으로 광선 검사를 수행해서 근접물체가 있으면 움직임을 멈추게 해야한다.

if( 캐릭컨트롤러의 이동방향 앞으로 물체가 근접했는가? == false)
	m_pChracterAnimator->controlStep_Walker(WalkVelocity);

*/


CHeroPlayer::CHeroPlayer()
{
	m_strTypeName = "CHeroPlayer";	
	int i;
	for(i=0;i<irr::KEY_KEY_CODES_COUNT;i++)
	{
		m_Key[i] = false;
	}	

	SetStatus(FSM_READY);
}

CHeroPlayer::~CHeroPlayer(void)
{
		
}

bool CHeroPlayer::Init(irr::scene::ISceneNode *pNode)
{

	if(CPlayer::Init(pNode))
	{
		//m_pChracterAnimator->setCCD(1/60,4.0f);	
		//m_pChracterAnimator->setCCD((1.f), .2f * (1.f));

		return true;
	}
	return false;
}



void CHeroPlayer::Update(irr::f32 fDelta)
{
	
	irr::scene::ISceneManager *pSmgr = CdmkApp::GetPtr()->m_pSmgr;
	irr::IrrlichtDevice *pDevice = CdmkApp::GetPtr()->m_pDevice;
	irr::scene::CBulletAnimatorManager* pBulletPhysicsFactory = CdmkApp::GetPtr()->m_pBulletPhysicsFactory; 
	irr::scene::CBulletWorldAnimator* pWorldAnimator = CdmkApp::GetPtr()->m_pWorldAnimator;
	//irr::scene::CBulletChracterAnimator *pChrAim = m_pChracterAnimator;

	switch(GetStatus())
	{
	case CHeroPlayer::FSM_READY:
		{
			//시작위치 지정
			m_pNode->setVisible(true);			
			irr::core::vector3df pos_Spwan = pSmgr->getSceneNodeFromName("start",m_pTrigerNode)->getAbsolutePosition();
			m_pChracterAnimator->setPosition(pos_Spwan);
			m_pChracterAnimator->zeroForces();

			SetStatus(FSM_STAND);
		}
		break;
	case CHeroPlayer::FSM_STAND:
		if(GetStep() == 0)
		{
			m_pNode->changeAction("stand");			
			
			IncStep();
		}
		else if(GetStep() == 1)
		{
			if( m_Key[irr::KEY_UP] ||
				m_Key[irr::KEY_DOWN] ||
				m_Key[irr::KEY_LEFT] ||
				m_Key[irr::KEY_RIGHT] 
			)
			{
				SetStatus(FSM_WALK);
			}
			if(m_Key[irr::KEY_KEY_A])
			{
				SetStatus(FSM_KICK);
			}
		}
		break;
	case CHeroPlayer::FSM_WALK:
		if(GetStep() == 0)
		{
			m_pNode->changeAction("walk");

			IncStep();
		}
		else if(GetStep() == 1)
		{
			btVector3 WalkVelocity(0,0,0);
			btScalar speed = 0;
			btScalar angle = 0;		

			speed = btScalar(1.1) * 1800.0f * fDelta;

			if( m_Key[irr::KEY_UP] ) {
				angle = 0;
				//speed = 2.0f;
				WalkVelocity = btVector3(0,0,1.f) * speed;
			}
			if(m_Key[irr::KEY_DOWN]) {
				angle = 180.f;
				//speed = 2.0f;
				WalkVelocity = btVector3(0,0,-1.f) * speed;
			}
			if(m_Key[irr::KEY_LEFT])  {
				angle = 90.f;
				//speed = 2.0f;
				WalkVelocity = btVector3(-1.0,0.0f,0.f) * speed;
			}
			if(m_Key[irr::KEY_RIGHT] ) {
				angle = 270;
				//speed = 2.0f;
				WalkVelocity = btVector3(1.0,0.0f,0.f) * speed;;
			}

			if(WalkVelocity.length() == 0)
			{
				angle = m_pChracterAnimator->getBodyFrontAngle();// * irr::core::RADTODEG;//.getHorizontalAngle().Y;			
				SetStatus(FSM_STAND);
			}			
			
			m_pChracterAnimator->controlStep_Walker(WalkVelocity);
		}
		break;
	case CHeroPlayer::FSM_KICK:
		if(GetStep() == 0)
		{
			m_pNode->changeAction("kick_1",false);
			IncStep();
		}
		else if(GetStep() == 1)
		{
			if(m_pNode->IsActionFinished()) //공격점 도달
			{
				//충돌처리

				btVoronoiSimplexSolver sGjkSimplexSolver; //최단점 찾기
				btGjkEpaPenetrationDepthSolver epaSolver; //파고들어간정도 구하기
				btPointCollector gjkOutput; 

				irr::scene::jz3d::CCollusionMngNode *pCollider = m_pCollMngNode_Kick;//때리는놈
				btCollisionShape *shape1 =  pCollider->getCollisionShape();
				btTransform shape_trans1;
				shape_trans1.setFromOpenGLMatrix(pCollider->getCollisionShapeTransform().pointer());

				irr::scene::jz3d::CCollusionMngNode *pCollidee = CdmkApp::GetPtr()->m_spZombie1->getBodyCollusionObject(); //맞을놈
				btCollisionShape *shape2 =  pCollidee->getCollisionShape();
				btTransform shape_trans2;
				shape_trans2.setFromOpenGLMatrix(pCollidee->getCollisionShapeTransform().pointer());
				

				//최단거리구하기
				{					
					btGjkPairDetector convexConvex(
						static_cast<const btConvexShape *>(shape1),
						static_cast<const btConvexShape *>(shape2),						
						&sGjkSimplexSolver,
						&epaSolver); 

					btGjkPairDetector::ClosestPointInput input; 
					input.m_transformA = shape_trans1;
					input.m_transformB = shape_trans2;

					convexConvex.getClosestPoints(input, gjkOutput, 0);					
					
					std::cout << gjkOutput.m_distance  << std::endl;					
				}

				if(gjkOutput.m_hasResult && gjkOutput.m_distance < 0.f)
				{
					CdmkApp::GetPtr()->m_spZombie1->Signal(std::string("kicked"),this);										
				}

				IncStep();
			}
		}
		else if(GetStep() == 2)
		{
			m_pNode->changeAction("kick_2",false); //리코일 액션
			IncStep();
		}
		else if(GetStep() == 3)
		{
			if(m_pNode->IsActionFinished())
			{
				SetStatus(FSM_STAND);
				
			}			
		}

		break;
	}


	


	/*if((::GetAsyncKeyState(VK_SPACE) &0x8001) == 0x8001)
	{						
		pChrAim->applyImpulse(irr::core::vector3df(0,70.0f*5.f,0));	

	}*/

	
}

void CHeroPlayer::Signal(std::string strSignal, void *pParam)
{

}

bool CHeroPlayer::OnEvent(const irr::SEvent &event)
{
	switch(event.EventType)
	{
	case irr::EET_GUI_EVENT:
		{

		}
		break;
	case irr::EET_KEY_INPUT_EVENT:
		{
			if(event.KeyInput.PressedDown)
			{
				m_Key[event.KeyInput.Key] = true;
			}
			else
			{
				m_Key[event.KeyInput.Key] = false;
			}
		}
		break;
	case irr::EET_MOUSE_INPUT_EVENT:
		{

		}
		break;
	case irr::EET_USER_EVENT:
		break;
	}
	return false;
}
