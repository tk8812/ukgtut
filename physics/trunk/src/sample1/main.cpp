
#include <irrlicht.h>
#include <iostream>


#include "btBulletDynamicsCommon.h"


void QuaternionToEuler(const btQuaternion &TQuat, btVector3 &TEuler) {
	btScalar W = TQuat.getW();
	btScalar X = TQuat.getX();
	btScalar Y = TQuat.getY();
	btScalar Z = TQuat.getZ();
	float WSquared = W * W;
	float XSquared = X * X;
	float YSquared = Y * Y;
	float ZSquared = Z * Z;

	TEuler.setX(atan2f(2.0f * (Y * Z + X * W), -XSquared - YSquared + ZSquared + WSquared));
	TEuler.setY(asinf(-2.0f * (X * Z - Y * W)));
	TEuler.setZ(atan2f(2.0f * (X * Y + Z * W), XSquared - YSquared - ZSquared + WSquared));
	TEuler *= irr::core::RADTODEG;
}

//일리히트 모션 스테이트
class IrrMotionState : public btDefaultMotionState
{
	irr::scene::ISceneNode* m_irrNode;
public:

	IrrMotionState(irr::scene::ISceneNode* irrNode)
		:m_irrNode(irrNode)
	{
	}
	///synchronizes world transform from user to physics
	virtual void getWorldTransform(btTransform& centerOfMassWorldTrans ) const 
	{
		centerOfMassWorldTrans = m_graphicsWorldTrans;
	}

	///synchronizes world transform from physics to user
	///Bullet only calls the update of worldtransform for active objects
	virtual void setWorldTransform(const btTransform& centerOfMassWorldTrans)
	{ 
		m_startWorldTrans = centerOfMassWorldTrans;
		m_graphicsWorldTrans = centerOfMassWorldTrans;

		const btVector3& Point = centerOfMassWorldTrans.getOrigin();
		m_irrNode->setPosition(irr::core::vector3df((irr::f32)Point[0], (irr::f32)Point[1], (irr::f32)Point[2]));

		// Set rotation
		btVector3 EulerRotation;
		QuaternionToEuler(centerOfMassWorldTrans.getRotation(), EulerRotation);
		m_irrNode->setRotation(irr::core::vector3df(EulerRotation[0], EulerRotation[1], EulerRotation[2]));
	}

};




void main()
{
	//메모리릭 탐지
#ifdef _IRR_WINDOWS_
	::_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif


	irr::IrrlichtDevice *pDevice = irr::createDevice(					
		irr::video::EDT_DIRECT3D9,
#if (IRRLICHT_VERSION_MAJOR >= 1 && IRRLICHT_VERSION_MINOR >= 6)
		irr::core::dimension2du(640,480), 
#else
		irr::core::dimension2di(640,480), 
#endif
		32,
		false, false, true,
		NULL
		);

	pDevice->setWindowCaption(L"bullet physic Type-A1");

	irr::video::IVideoDriver *pVideo = pDevice->getVideoDriver();
	irr::scene::ISceneManager *pSmgr = pDevice->getSceneManager();
	irr::gui::IGUIEnvironment *pGuiEnv = pDevice->getGUIEnvironment();


	pSmgr->addCameraSceneNode(0,irr::core::vector3df(0,6,-15),irr::core::vector3df(0,0,0));

	///setup the main Bullet datastructures
	//충돌 모형 만들기	
	btCollisionShape* pColShape = new  btBoxShape(btVector3(.5f,.5f,.5f)); //박스
	btCollisionShape* pGroundColShape = new  btBoxShape(btVector3(5.f,.5f,5.f)); //바닥

	//월드만들기 
	btDiscreteDynamicsWorld* gDynamicsWorld=0;			
	{		
		btDefaultCollisionConfiguration *collisionConfiguration = new btDefaultCollisionConfiguration();
		collisionConfiguration->setConvexConvexMultipointIterations();

		btDispatcher*				dispatcher = new btCollisionDispatcher(collisionConfiguration);
		btBroadphaseInterface*		Broadphase = new btDbvtBroadphase();
		btConstraintSolver*			constraintSolver = new btSequentialImpulseConstraintSolver();

		
		///create the main Bullet dynamics world
		gDynamicsWorld  = new btDiscreteDynamicsWorld(dispatcher,Broadphase,constraintSolver,collisionConfiguration);
		gDynamicsWorld->setGravity(btVector3(0,-9.8f,0));
	}


	//정적 바닥 강체
	{	
		btRigidBody* pRigidBody;	

		irr::scene::ISceneNode *pNode = pSmgr->addCubeSceneNode(1);
		pNode->setMaterialFlag(irr::video::EMF_LIGHTING,false);
		pNode->setMaterialFlag(irr::video::EMF_WIREFRAME,true);
		pNode->setScale(irr::core::vector3df(10,1,10));


		btDefaultMotionState *pMotionState;		
		btScalar	mass(0.0f);
		btVector3 localInertia(0,0,0);								
		pGroundColShape->calculateLocalInertia(mass,localInertia);

		btTransform startTransform;
		startTransform.setIdentity();
		startTransform.setOrigin(btVector3(0,-2.f,0));
		
		pMotionState = new IrrMotionState(pNode);		
		pMotionState->setWorldTransform(startTransform);

		//강체생성
		btRigidBody::btRigidBodyConstructionInfo rbInfo(mass,pMotionState,pGroundColShape,localInertia);
		pRigidBody = new btRigidBody(rbInfo);				

		gDynamicsWorld->addRigidBody(pRigidBody);
	}

	//동적 강체
	{	
		btRigidBody* pRigidBody;	

		irr::scene::ISceneNode *pNode = pSmgr->addCubeSceneNode(1);
		pNode->setMaterialFlag(irr::video::EMF_LIGHTING,false);
		pNode->setMaterialFlag(irr::video::EMF_WIREFRAME,true);
		//pNode->setScale(irr::core::vector3df(1,1,10));


		btDefaultMotionState *pMotionState;		
		btScalar	mass(0.1f);
		btVector3 localInertia(0,0,0);								
		pColShape->calculateLocalInertia(mass,localInertia);

		btTransform startTransform;
		startTransform.setIdentity();
		startTransform.setOrigin(btVector3(0,5.f,0));
		
		pMotionState = new IrrMotionState(pNode);		
		pMotionState->setWorldTransform(startTransform);
		

		//강체생성
		btRigidBody::btRigidBodyConstructionInfo rbInfo(mass,pMotionState,pColShape,localInertia);
		pRigidBody = new btRigidBody(rbInfo);				

		gDynamicsWorld->addRigidBody(pRigidBody);
	}




	//프레임 레이트 표시용 유아이
	irr::gui::IGUIStaticText *pstextFPS = 
		pGuiEnv->addStaticText(L"Frame rate",irr::core::rect<irr::s32>(0,0,100,20),true,true,0,100,true);

	//타이머 재초기화
	pDevice->getTimer()->setTime(0);

	while(pDevice->run())
	{	
		static irr::u32 uLastTick=0;
		irr::u32 uTick = pDevice->getTimer()->getTime();						
		irr::f32 fDelta = ((float)(uTick - uLastTick)) / 1000.f; //델타값 구하기
		uLastTick = uTick;

		//프레임레이트 갱신, 삼각형수 표시
		{
			wchar_t wszbuf[256];
			swprintf(wszbuf,L"Frame rate : %d\n TriAngle: %d",pVideo->getFPS(),pVideo->getPrimitiveCountDrawn());
			pstextFPS->setText(wszbuf);
		}	

		gDynamicsWorld->stepSimulation(fDelta);

		pVideo->beginScene(true, true, irr::video::SColor(255,100,101,140));

		pSmgr->drawAll();

		pSmgr->drawAll();
		pGuiEnv->drawAll();		

		pVideo->endScene();

	}

	//월드 객체 해제하기
	{
		//충돌객체배열 받기		
		btCollisionObjectArray coa = gDynamicsWorld->getCollisionObjectArray();
		//추출기받기
		btCollisionDispatcher* dispatcher = (btCollisionDispatcher*)gDynamicsWorld->getDispatcher();		
		//구속계산기 받기
		btConstraintSolver *pConstraintSolver = gDynamicsWorld->getConstraintSolver();
		//위상관리자 받기
		btBroadphaseInterface *pBroadphase = gDynamicsWorld->getBroadphase();	

		//btCollisionConfiguration
		
		delete gDynamicsWorld;

		delete dispatcher->getCollisionConfiguration();
		delete dispatcher;
		delete pConstraintSolver;	
		delete pBroadphase;

		int i;
		for(i=0;i < coa.size();i++)
		{
			if(coa[i]->getInternalType() == btCollisionObject::CO_RIGID_BODY)
			{
				delete ((btRigidBody *)(coa[i]))->getMotionState();
			}
			delete coa[i];
		}
	}

	delete pColShape;	
	delete pGroundColShape;	

	pDevice->drop();
}
