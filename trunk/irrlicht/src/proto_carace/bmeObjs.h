#pragma		once

//로컬플레이어 바이시클
class CObj_ThisBycle : public CEsenObject,public irr::scene::ISceneNodeAnimator
{
private:
	CEsenFrame *m_pFrame;

	
	//CBycleSceneNode *m_pBycleNode;
	irr::scene::ISceneNode *m_pRootSceneNode;
	//irr::scene::ISceneNode *m_pWorkSceneNode;	
	
	irr::scene::ICameraSceneNode *m_pCamNode;	
	/*irr::scene::ISceneNode *m_pCamControlNode;
	irr::scene::ISceneNode *m_pCamControlNode_pos;
	irr::scene::ISceneNode *m_pCamControlNode_rot;*/
	irr::scene::ISceneNode *m_pCamLookatNode;
	irr::scene::ISceneNodeAnimator *m_pCamAnimator;
	irr::core::quaternion m_qtCamRotation;
	
	/*
	irr::scene::ISceneNode *m_pBycleNode;		
	irr::scene::ISceneNode *m_pBycleFedalNode;		
	irr::scene::ISceneNode *m_pBycleHandleNode;		
	irr::scene::ISceneNode *m_pBycleFWheel;
	irr::scene::ISceneNode *m_pBycleBWheel;
	*/

	irr::scene::ISceneNode *m_pStartSignal[2];

	

	/*
	irr::core::vector3df m_v3Pos;
	irr::f32 m_fTurn; //회전각도
	irr::f32 m_fHeading; //방향각
	irr::core::vector3df m_v3Heading;//방향벡터
	irr::f32 m_fVelocity; //속도
	irr::f32 m_fCamYaw; //카메라 위치
	*/	

	irr::u32 m_u32StartTick;
	irr::u32 m_u32EndTick;
	irr::u32 m_u32AccTick;
	irr::s32 m_nCountDown;	

	//물리차량용
	irr::scene::ISceneNode *m_pVehicleNode;		
	irr::scene::ISceneNode *m_pWheelFR;		
	irr::scene::ISceneNode *m_pWheelFL;		
	irr::scene::ISceneNode *m_pWheelRR;		
	irr::scene::ISceneNode *m_pWheelRL;	

	DB_VehiclePhysics m_dbVehiclePhysics;//자동차물리 데이터블럭

	std::tr1::shared_ptr<btRigidBody> m_carChassis;
	btRaycastVehicle::btVehicleTuning	m_tuning;
	std::tr1::shared_ptr<btVehicleRaycaster>	m_vehicleRayCaster;
	std::tr1::shared_ptr<btRaycastVehicle>	m_vehicle;

	std::tr1::shared_ptr<btDefaultMotionState> m_spCarMotionState;
	std::tr1::shared_ptr<btCollisionShape> m_spChassisShape;
	std::tr1::shared_ptr<btCompoundShape> m_spCompound;	

	float	m_fMaxSpeed;
	float	m_EngineForce;// = 0.f;
	float	m_BreakingForce;// = 0.f;
	float	m_VehicleSteering;// = 0.f;

public:
	enum {
		ST_SLEEP,
		ST_READY,		
		ST_RACE,
		ST_RESULT,
		ST_PAUSE
	};	

	CObj_ThisBycle()
	{
	}

	~CObj_ThisBycle()
	{	
		//m_pRootSceneNode->removeAnimators();		
	}

	void Release()
	{		
	}

	float getSpeedKmh()
	{
		return m_vehicle->getCurrentSpeedKmHour();
	}

	void InitVechilePhysics();	

	void DoBreake()
	{
		m_EngineForce = 0.f;
		m_BreakingForce = m_dbVehiclePhysics.m_BreakeForce;
		//m_BreakingForce = 100.f;
		//m_EngineForce = 0;
	}

	void ChangeSpeed(irr::f32 speed)
	{	
		if(speed > 0)
		{
			m_EngineForce = m_dbVehiclePhysics.m_EngineForce;
			m_BreakingForce = 0.f;		
		}
		else if(speed < 0)
		{
			m_EngineForce = -(m_dbVehiclePhysics.m_EngineForce/3.f);
			m_BreakingForce = 0.f;
		}
		else
		{
			m_EngineForce = 0.f;
			m_BreakingForce = m_dbVehiclePhysics.m_ResistForce;//바퀴저항값
		}
	}

	void IncreaseTurn(irr::f32 fTurn)
	{	
		if( g_Aixes_X < 1000 && g_Aixes_X > -1000)
			g_Aixes_X += (LONG)(fTurn * 1000.f);		

		if( g_Aixes_X >= 1000)
			g_Aixes_X = 999;
		if( g_Aixes_X <= -1000)
			g_Aixes_X = -999;

	}
	
	bool IsSleep()
	{
		if(GetStatus() == CObj_ThisBycle::ST_SLEEP)
		{
			return true;
		}

		return false;

	}

	// 결과저장
	irr::f32 getRecord()
	{
		irr::f32 fRecord = ((float)(m_u32EndTick - m_u32StartTick)) / 1000.0f;		
		return fRecord;		
	}

	void HideVehicle()
	{
		m_pVehicleNode->setVisible(false);
		m_pWheelFR->setVisible(false);
		m_pWheelFL->setVisible(false);
		m_pWheelRL->setVisible(false);
		m_pWheelRR->setVisible(false);
	}

	void ShowVehicle()
	{
		m_pVehicleNode->setVisible(true);
		m_pWheelFR->setVisible(true);
		m_pWheelFL->setVisible(true);
		m_pWheelRL->setVisible(true);
		m_pWheelRR->setVisible(true);
	}

	irr::core::vector3df getPosition();

	bool IsMissionComplete();

	void CompleteRace()
	{
		SetStatus(CObj_ThisBycle::ST_RESULT);
	}

	void PauseRace()
	{
		SetStatus(CObj_ThisBycle::ST_PAUSE);
	}
	
	void Init(CEsenFrame *pFrame);	
	void Apply(irr::u32 timeMS);	
	void Render();

	virtual void animateNode(irr::scene::ISceneNode* node, u32 timeMs);
	virtual irr::scene::ISceneNodeAnimator* createClone(irr::scene::ISceneNode* node, irr::scene::ISceneManager* newManager=0)
	{
		return NULL;
	}
	//void Apply_Transform(irr::u32 timeMS);
	
};

