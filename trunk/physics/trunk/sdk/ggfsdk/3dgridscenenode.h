#pragma once

namespace irr
{
namespace scene
{
namespace scene_ggf
{
	

class C3DGridSceneNode : public irr::scene::ISceneNode
{

	/*
	First, we declare some member variables. Some space to
	hold data for our tetraeder: The bounding box, 4 vertices, and
	the material of the tetraeder.
	*/
	irr::core::aabbox3d<irr::f32> Box;
	irr::video::S3DVertex Vertices[4];
	irr::video::SMaterial Material;


	/*
	The parameters of the constructor specify the parent of the scene node,
	a pointer to the scene manager, and an id of the scene node.
	In the constructor itself, we call the parent classes constructor,
	set some properties of the material we use to draw the scene nodem and 
	create the 4 vertices of the tetraeder we will draw later.
	*/

public:

	C3DGridSceneNode(irr::scene::ISceneNode* parent, irr::scene::ISceneManager* mgr, irr::s32 id)
		: irr::scene::ISceneNode(parent, mgr, id)
	{
		Material.Wireframe = false;
		Material.Lighting = false;

		Vertices[0] = irr::video::S3DVertex(0,0,10, 1,1,0, irr::video::SColor(255,0,255,255), 0, 1);
		Vertices[1] = irr::video::S3DVertex(10,0,-10, 1,0,0, irr::video::SColor(255,255,0,255), 1, 1);
		Vertices[2] = irr::video::S3DVertex(0,20,0, 0,1,1, irr::video::SColor(255,255,255,0), 1, 0);
		Vertices[3] = irr::video::S3DVertex(-10,0,-10, 0,0,1, irr::video::SColor(255,0,255,0), 0, 0);

	/*
	The Irrlicht Engine needs to know the bounding box of your scene node. 
	It will use it for doing automatic culling and other things. Hence we
	need to create a bounding box from the 4 vertices we use. 
	If you do not want the engine to use the box for automatic culling, 
	and/or don't want to create the box, you could also write
	AutomaticCullingEnabled = false;.
	*/
		Box.reset(Vertices[0].Pos);
		for (irr::s32 i=1; i<4; ++i)
			Box.addInternalPoint(Vertices[i].Pos);
	}


	/*
	Before it is drawn, the OnPreRender() method of every scene node in the scene 
	is called by the scene manager. If the scene node wishes to draw itself,
	it may register itself in the scene manager to be drawn. This is necessary to
	tell the scene manager when it should call the ::render method. For example
	normal scene nodes render their content one after another, while
	stencil buffer shadows would like to be drawn after all other scene nodes. And
	camera or light scene nodes need to be rendered before all other scene 
	nodes (if at all).
	So here we simply register the scene node to get render normally. If we would like
	to let it be rendered like cameras or light, we would have to call
	SceneManager->registerNodeForRendering(this, SNRT_LIGHT_AND_CAMERA);
	After this, we call the OnPreRender-method of the base class ISceneNode,
	which simply lets also all the child scene nodes of this node register themselves.
	*/
	virtual void OnRegisterSceneNode()
	//virtual void OnPreRender()
	{
		if (IsVisible)
			SceneManager->registerNodeForRendering(this);

		ISceneNode::OnRegisterSceneNode();
		//ISceneNode::OnPreRender();
	}

	/*
	In the render() method most of the interesting stuff happenes: The
	Scene node renders itself. We override this method and draw the
	tetraeder.
	*/
	virtual void render()
	{
		irr::u16 indices[] = {	0,2,3, 2,1,3, 1,0,3, 2,0,1	};
		irr::video::IVideoDriver* driver = SceneManager->getVideoDriver();

		driver->setMaterial(Material);
		driver->setTransform(irr::video::ETS_WORLD, AbsoluteTransformation);
		//driver->drawIndexedTriangleList(&Vertices[0], 4, &indices[0], 4);		
		//driver->draw3DLine(Vertices[0].Pos,Vertices[1].Pos,Vertices[0].Color);

		int i;
		irr::f32 fTemp;

		fTemp = 0;
		for(i=-500;i<= 500;i+=50)
		{
			driver->draw3DLine(irr::core::vector3df(fTemp+i,0,500),irr::core::vector3df(fTemp +i,0,-500),irr::video::SColor(255,255,255,255));
		}

		fTemp = 0;
		for(i=-500;i<= 500;i+=50)
		{
			driver->draw3DLine(irr::core::vector3df(500,0,fTemp+i),irr::core::vector3df(-500,0,fTemp +i),irr::video::SColor(255,255,255,255));
		}

		driver->draw3DLine(irr::core::vector3df(0,0,0),irr::core::vector3df(0,500,0),irr::video::SColor(255,255,0,0));

		driver->draw3DLine(irr::core::vector3df(0,0,0),irr::core::vector3df(10,50,0),irr::video::SColor(255,255,0,0));
		driver->draw3DLine(irr::core::vector3df(0,0,0),irr::core::vector3df(100,100,0),irr::video::SColor(255,255,0,0));
		driver->draw3DLine(irr::core::vector3df(-500,0,500),irr::core::vector3df(-500,500,500),irr::video::SColor(255,0,255,0));

	}

	/*
	At least, we create three small additional methods.
	GetBoundingBox() returns the bounding box of this scene node, 
	GetMaterialCount() returns the amount of materials in this scene node
	(our tetraeder only has one material), and getMaterial() returns the
	material at an index. Because we have only one material here, we can
	return the only one meterial, assuming that no one ever calls getMaterial()
	with an index greater than 0.
	*/
	virtual const irr::core::aabbox3d<irr::f32>& getBoundingBox() const
	{
		return Box;
	}

	virtual irr::s32 getMaterialCount()
	{
		return 1;
	}

	virtual irr::video::SMaterial& getMaterial(irr::s32 i)
	{
		return Material;
	}	
};

}
}
}




