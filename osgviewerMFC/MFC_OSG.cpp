// MFC_OSG.cpp : implementation of the cOSG class
//
#include "stdafx.h"
#include "MFC_OSG.h"

#include "SkyBox.h"


cOSG::cOSG(HWND hWnd) :
   m_hWnd(hWnd) 
{
	// 使OSG可以识别中文
	setlocale(LC_ALL,".936");
}

cOSG::~cOSG()
{
	if(!bErrModel)
	{
		mViewer->setDone(true);
		Sleep(1000);
		mViewer->stopThreading();

		delete mViewer;
	}
}
bool cOSG::InitOSG(std::string modelname)
{
	bErrModel = false;
    // Store the name of the model to load
    m_ModelName = modelname;

    // Init different parts of OSG
    InitManipulators();
   if(!InitSceneGraph()) 
	   return false;
    InitCameraConfig();

	return true;
}

void cOSG::InitManipulators(void)
{
    // Create a trackball manipulator
    trackball = new osgGA::TrackballManipulator();

    // Create a Manipulator Switcher
    keyswitchManipulator = new osgGA::KeySwitchMatrixManipulator;

    // Add our trackball manipulator to the switcher
    keyswitchManipulator->addMatrixManipulator( '1', "Trackball", trackball.get());

    // Init the switcher to the first manipulator (in this case the only manipulator)
    keyswitchManipulator->selectMatrixManipulator(0);  // Zero based index Value
}

bool cOSG::InitSceneGraph(void)
{
    // Init the main Root Node/Group
    mRoot  = new osg::Group;

	osg::ref_ptr<osgDB::ReaderWriter::Options> options = new osgDB::ReaderWriter::Options;	

// 2013年7月3日
// 	if(osgDB::getLowerCaseFileExtension(m_ModelName.c_str())=="x")
// 	{
// 			options ->setOptionString("rightHanded noTexturesInIVEFile flipTexture creaseAngle");
// 	}
	setlocale(LC_ALL,".936");

    // Load the Model from the model name
     mModel = osgDB::readNodeFile(m_ModelName,options.get());
 	if(!mModel)
 	{
 		bErrModel = true;
 		return false;
 	}

 	OptTexture  t2d;
 	mModel->accept(t2d);
 
     // Optimize the model
     osgUtil::Optimizer optimizer;
     optimizer.optimize(mModel.get());
     optimizer.reset();

     //Add the model to the scene
    mRoot->addChild(mModel.get());

 	SkyBox &skbx = SkyBox::GetInstance();
 	osg::Node* pNode = skbx.createSkyBox("D:\\codes\\osgEarthDemo\\data\\Cubemap_snow");

	if(pNode)
	{
		mRoot->addChild(pNode);
	}

	return true;
}

//创建表盘
osg::Node * cOSG::CreateClockFace ()
{
	//申请根结点，做为返回值
	osg::Group * root = new osg::Group () ;
	//设置线宽
	osg::ref_ptr <osg::LineWidth> LineSize = new osg::LineWidth;
	LineSize ->setWidth (4.0) ;
	//打开线宽属性
	root->getOrCreateStateSet()->setAttributeAndModes(LineSize.get(),osg::StateAttribute::ON);
	//表盘几何结点
	osg::Geode* clockGeode = new osg::Geode();
	//表盘圈
	osg::ref_ptr <osg::Geometry > clockGeometry = new osg::Geometry();
	//数字
	osg::ref_ptr <osg::Geometry > NumPoint = new osg::Geometry () ;
	//加入到Geode中去
	clockGeode->addDrawable(clockGeometry);
	clockGeode->addDrawable (NumPoint) ;
	//加入表盘结点
	root->addChild(clockGeode);
	//申请数字结点顶点
	osg::ref_ptr <osg::Vec3Array > Num = new osg::Vec3Array ;
	//存放经过插值得到的所有点，而后画直线连成表盘
	osg::ref_ptr <osg::Vec3Array > allPoints = new osg::Vec3Array ;
	//表盘颜色
	osg::Vec4Array* colors = new osg::Vec4Array;
	//表针颜色
	osg::Vec4Array* colors2 = new osg::Vec4Array ;
	//加入颜色，得到十二个数字的顶点
	for (double i = 0.0; i <6.28 ; i+=0.52)
	{
		colors2 ->push_back (osg::Vec4f (sin(i), cos(i), 0.5, 1.0)) ;
		colors2 ->push_back (osg::Vec4f (cos(i), sin(i), 0.5, 1.0)) ;
		if(i == 0.0)
		{
			//保证每个顶点一种颜色，在12：00这个数上多画两道予以区分
			colors2 ->push_back (osg::Vec4f (sin(i), cos(i), 0.5, 1.0)) ;
			colors2 ->push_back (osg::Vec4f (cos(i), sin(i), 0.5, 1.0)) ;
			Num ->push_back (osg::Vec3 (45*sin(i)-4,-0.0 , 45*cos(i))) ;
			Num ->push_back (osg::Vec3 (40*sin(i)-4,-0.0 , 40*cos(i))) ;
			Num ->push_back (osg::Vec3 (45*sin(i)+4,-0.0 , 45*cos(i))) ;
			Num ->push_back (osg::Vec3 (40*sin(i)+4,-0.0 , 40*cos(i))) ;
		}
		else
		{
			Num ->push_back (osg::Vec3 (45*sin(i),-0.0 , 45*cos(i))) ;
			Num ->push_back (osg::Vec3 (40*sin(i),-0.0 , 40*cos(i))) ;
		} 
	}
	//得到半径是50的钟表盘面外圈的所有的顶点， a=0.02,在外圈共压入314个点，形成314条线
	for (double i = 0.0; i <6.28 ; i+=0.02)
	{
		colors ->push_back (osg::Vec4f (sin(i), cos(i), 0.5, 1.0)) ;
		allPoints ->push_back (osg::Vec3 (50*sin(i),-0.0 , 50*cos(i))) ;
	}

	//得到所有顶点
	clockGeometry->setVertexArray( allPoints.get () );
	//画线
	osg::DrawElementsUInt * clockBase = new osg::DrawElementsUInt(osg::PrimitiveSet::LINE_LOOP ,0);
	for (int i =0 ; i<(int)(allPoints ->size ()) ; i++)
	{
		clockBase->push_back(i);
	} ;
	clockGeometry->addPrimitiveSet(clockBase);
	//得到数字顶点
	NumPoint ->setVertexArray (Num.get ()) ;
	osg::DrawElementsUInt * PointBase = new osg::DrawElementsUInt(osg::PrimitiveSet ::LINES , 0) ;
	for (int i = 0 ; i<(int)(Num ->size ()); i ++)
	{
		//画线
		PointBase ->push_back (i) ;
	}
	NumPoint ->addPrimitiveSet (PointBase) ;
	//使用颜色索引
	osg::TemplateIndexArray	<unsigned int, osg::Array::UIntArrayType,4,4> *colorIndexArray;
	colorIndexArray =
		new osg::TemplateIndexArray<unsigned int, osg::Array::UIntArrayType,4,4>;
	for (int i =0 ; i<(int)(allPoints ->size ()) ; i++)
	{
		colorIndexArray->push_back(i);
	} ;
	osg::TemplateIndexArray <unsigned int, osg::Array::UIntArrayType,4,4> *colorIndexArrayNum;
	colorIndexArrayNum = new osg::TemplateIndexArray<unsigned int, osg::Array::UIntArrayType,4,4>;
	for (int i =0 ; i<(int)(Num ->size ()) ; i++)
	{
		colorIndexArrayNum->push_back(i);
	} ;

	clockGeometry->setColorArray(colors);
	//clockGeometry->setColorIndices(colorIndexArray);
	clockGeometry->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
	NumPoint ->setColorArray(colors2);
	//NumPoint->setColorIndices(colorIndexArrayNum);
	NumPoint->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
	return (osg::Node *) root ;
}


//创建表针从res到des
osg::Node* cOSG::CreateHandler(osg::Vec3 &res, osg::Vec3 &des)
{
	osg::Group * root = new osg::Group () ;
	//设置线宽为4
	osg::ref_ptr <osg::LineWidth> LineSize = new osg::LineWidth;
	LineSize ->setWidth (4.0) ;
	//打开线宽
	root->getOrCreateStateSet()->setAttributeAndModes(LineSize.get(),osg::StateAttribute::ON);
	//申请针结点
	osg::Geode* handleGeode = new osg::Geode();
	osg::Geometry* handleGeometry = new osg::Geometry();
	handleGeode->addDrawable(handleGeometry);
	root->addChild(handleGeode);
	osg::ref_ptr <osg::Vec3Array > allPoints = new osg::Vec3Array ;
	osg::Vec4Array* colors = new osg::Vec4Array;
	colors ->push_back (osg::Vec4f (1.0, 0.0, 0.5, 1.0)) ;
	colors ->push_back (osg::Vec4f (0.0, 1.0, 0.5, 1.0)) ;
	//把起点与终点压入
	allPoints ->push_back (res) ;
	allPoints ->push_back (des) ;
	//设置要画的顶点
	handleGeometry->setVertexArray( allPoints.get () );
	osg::DrawElementsUInt * handleBase = new
		osg::DrawElementsUInt(osg::PrimitiveSet::LINE_LOOP ,0);
	for (int i =0 ; i<(int)(allPoints ->size ()) ; i++)
	{
		handleBase->push_back(i);
	} ;
	handleGeometry->addPrimitiveSet(handleBase);
	//设置颜色
	osg::TemplateIndexArray
		<unsigned int, osg::Array::UIntArrayType,4,4> *colorIndexArray;
	colorIndexArray =	new osg::TemplateIndexArray<unsigned int, osg::Array::UIntArrayType,4,4>;
	for (int i =0 ; i<(int)(allPoints ->size ()) ; i++)
	{
		colorIndexArray->push_back(i);
	} ;
	handleGeometry->setColorArray(colors);
	//handleGeometry->setColorIndices(colorIndexArray);
	handleGeometry->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
	return (osg::Node *) root ;
}
//创建表针移动路径
osg::AnimationPath* cOSG::createAnimationPath(const osg::Vec3& center, float radius, double looptime, float angle)
{ //创建path
	osg::AnimationPath* animationPath = new osg::AnimationPath;
	//设置循环
	animationPath->setLoopMode(osg::AnimationPath::LOOP);
	//设置总共关键点数
	int numSamples = 40;
	float yaw = 0.0f;
	//步长
	float yaw_delta = 2.0f*osg::PI/((float)numSamples-1.0f);
	float roll = osg::inDegrees(30.0f);
	//设置循环时间
	double time=0.0f;
	double time_delta = looptime/(double)numSamples;
	for(int i=0;i<numSamples;++i)
	{
		osg::Vec3 position(center+osg::Vec3(sinf(yaw)*radius,cosf(yaw)*radius,0.0f));
		osg::Quat rotation(osg::Quat(roll,osg::Vec3(0.0,1.0,0.0))*osg::Quat((yaw+osg::inDegrees(angle)),osg::Vec3(0.0,1.0,0.0)));
		//把关键点与时间压入形成路径
		animationPath->insert(time,osg::AnimationPath::ControlPoint(position,rotation));
		yaw += yaw_delta;
		time += time_delta;
	}
	return animationPath;
}


//创建移动表针
osg::Node* cOSG::CreateMoveHandler(void)
{
	//得到当前的时间
	SYSTEMTIME st ;
	::GetLocalTime (&st) ;
	//创建秒针，分针，时针
	osg::Node * second = CreateHandler(osg::Vec3 (-10.0,-0.0 , 0.0), osg::Vec3(35,-0.0 ,0.0)) ;
	osg::Node * minute = CreateHandler(osg::Vec3 (-8.0, -0.0, 0.0), osg::Vec3 (25,	-0.0, 0.0)) ;
	osg::Node * hour = CreateHandler(osg::Vec3 (-6.0, -0.0, 0.0), osg::Vec3 (15,	-0.0, 0.0)) ;
	//利用当前系统时间初始化角度
	float animationLength = 60.0F;
	osg::Vec3 center(0.0, 0.0, 0.0);
	float radius = 0.0 ;
	float angle0 , angle1, angle2 ;
	angle0 = st.wSecond *6.0 ;
	angle1 = st.wMinute *6.0 + angle0 * (1/60.0) ;
	angle2 = st.wHour * 30.0 + angle1 * (1/60) ;
	//周期越来越长，秒针周期为60S
	osg::AnimationPath* secondPath =createAnimationPath(center,radius,animationLength, -120.0+angle0) ;//+ angle2);
	animationLength *= 60.0f ;
	//分针为60^2
	osg::AnimationPath* minutePath = createAnimationPath (center, radius, animationLength, -120.0+angle1); //+angle1);
	animationLength *=60.0f ;
	//时针为60^3
	osg::AnimationPath* hourPath = createAnimationPath (center, radius, animationLength, -120.0+angle2) ;//+angle0);
	osg::Group* model = new osg::Group;
	osg::MatrixTransform* positioned1 = new osg::MatrixTransform;
	positioned1->setDataVariance(osg::Object::STATIC);
	positioned1->addChild(second);
	osg::MatrixTransform* positioned2 = new osg::MatrixTransform;
	positioned2->setDataVariance(osg::Object::STATIC);
	positioned2->addChild (minute);
	osg::MatrixTransform* positioned3 = new osg::MatrixTransform;
	positioned3->setDataVariance(osg::Object::STATIC);
	positioned3->addChild (hour);
	osg::PositionAttitudeTransform* xform1 = new osg::PositionAttitudeTransform;
	xform1->setUpdateCallback(new osg::AnimationPathCallback(secondPath,0.0,1.0));
	xform1->addChild(positioned1);
	osg::PositionAttitudeTransform* xform2 = new osg::PositionAttitudeTransform;
	xform2->setUpdateCallback(new osg::AnimationPathCallback(minutePath,0.0,1.0));
	xform2->addChild(positioned2);
	osg::PositionAttitudeTransform* xform3 = new osg::PositionAttitudeTransform;
	xform3->setUpdateCallback(new osg::AnimationPathCallback(hourPath,0.0,1.0));
	xform3->addChild(positioned3);
	model->addChild(xform1);
	model->addChild(xform2);
	model->addChild(xform3);
	return model;
}


void cOSG::InitCameraConfig(void)
{
    // Local Variable to hold window size data
    RECT rect;

    // Create the viewer for this window
    mViewer = new osgViewer::Viewer();

    // Add a Stats Handler to the viewer
    mViewer->addEventHandler(new osgViewer::StatsHandler);
	mViewer->addEventHandler(new  osgViewer::ThreadingHandler);
	mViewer->addEventHandler(new  osgViewer::WindowSizeHandler);
	mViewer->addEventHandler(new  osgViewer::RecordCameraPathHandler);
	mViewer->addEventHandler(new  osgViewer::LODScaleHandler);
	mViewer->addEventHandler(new  osgViewer::ScreenCaptureHandler);
	mViewer->addEventHandler( new osgGA::StateSetManipulator(mViewer->getCamera()->getOrCreateStateSet()) );
    
    // Get the current window size
    ::GetWindowRect(m_hWnd, &rect);

    // Init the GraphicsContext Traits
    osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits;

    // Init the Windata Variable that holds the handle for the Window to display OSG in.
    osg::ref_ptr<osg::Referenced> windata = new osgViewer::GraphicsWindowWin32::WindowData(m_hWnd);

    // Setup the traits parameters
    traits->x = 0;
    traits->y = 0;
    traits->width = rect.right - rect.left;
    traits->height = rect.bottom - rect.top;
    traits->windowDecoration = false;
    traits->doubleBuffer = true;
    traits->sharedContext = 0;
    traits->setInheritedWindowPixelFormat = true;
    traits->inheritedWindowData = windata;

    // Create the Graphics Context
    osg::GraphicsContext* gc = osg::GraphicsContext::createGraphicsContext(traits.get());

    // Init a new Camera (Master for this View)
    osg::ref_ptr<osg::Camera> camera = new osg::Camera;

    // Assign Graphics Context to the Camera
    camera->setGraphicsContext(gc);

    // Set the viewport for the Camera
    camera->setViewport(new osg::Viewport(traits->x, traits->y, traits->width, traits->height));

    // Set projection matrix and camera attribtues
    camera->setClearMask(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    camera->setClearColor(osg::Vec4f(0.2f, 0.2f, 0.4f, 1.0f));
    camera->setProjectionMatrixAsPerspective(30.0f, static_cast<double>(traits->width)/static_cast<double>(traits->height), 1.0, 1000.0);

    // Add the Camera to the Viewer
    //mViewer->addSlave(camera.get());
    mViewer->setCamera(camera.get());

    // Add the Camera Manipulator to the Viewer
    mViewer->setCameraManipulator(keyswitchManipulator.get());
	//mViewer->setCameraManipulator(travelManipulator->TravelScene(mViewer));

	osg::ref_ptr<osg::MatrixTransform> pMat = new osg::MatrixTransform;
	//pMat->setMatrix(osg::Matrix::translate(osg::Vec3(1300, 300, 0)));

// 	pMat->addChild (CreateClockFace ()) ;
// 	pMat->addChild (CreateMoveHandler());

	mRoot->addChild(pMat);

	// Set the Scene Data
    mViewer->setSceneData(mRoot.get());

    // Realize the Viewer
    mViewer->realize();

    // Correct aspect ratio
    /*double fovy,aspectRatio,z1,z2;
    mViewer->getCamera()->getProjectionMatrixAsPerspective(fovy,aspectRatio,z1,z2);
    aspectRatio=double(traits->width)/double(traits->height);
    mViewer->getCamera()->setProjectionMatrixAsPerspective(fovy,aspectRatio,z1,z2);*/
}

void cOSG::PreFrameUpdate()
{
    // Due any preframe updates in this routine
}

void cOSG::PostFrameUpdate()
{
    // Due any postframe updates in this routine
}

void cOSG::Render(void* ptr)
{
    cOSG* osg = (cOSG*)ptr;

    osgViewer::Viewer* viewer = osg->getViewer();

    // You have two options for the main viewer loop
    //      viewer->run()   or
    //      while(!viewer->done()) { viewer->frame(); }

    //viewer->run();
    while(!viewer->done())
    {
        osg->PreFrameUpdate();
        viewer->frame();
        osg->PostFrameUpdate();
        //Sleep(10);         // Use this command if you need to allow other processes to have cpu time
    }

    // For some reason this has to be here to avoid issue: 
    // if you have multiple OSG windows up 
    // and you exit one then all stop rendering
  //  AfxMessageBox("Exit Rendering Thread");

    _endthread();
}






//OptTexture 类函数
OptTexture::OptTexture()
:osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ACTIVE_CHILDREN)//初始化
{
	// 使OSG可以识别中文
	setlocale(LC_ALL,".936");
}

OptTexture::~OptTexture(void)
{
}
void OptTexture::apply(osg::Node& node)
{
// 	if(node.getStateSet())
// 	{
// 		apply(node.getStateSet());
// 	}
	traverse(node);
}
void OptTexture::apply(osg::Geode& geode)
{
// 	if(geode.getStateSet())
// 	{
// 		apply(geode.getStateSet());
// 	}
	unsigned int cnt = geode.getNumDrawables();
	for(unsigned int i = 0; i < cnt; i++)
	{
		//20130915 MYF模型深度测试不正确
		osg::StateSet *stateset = geode.getDrawable(i)->getOrCreateStateSet();
		stateset->setMode(GL_BLEND,osg::StateAttribute::ON);  //取消深度测试   
		stateset->setMode( GL_DEPTH_TEST, osg::StateAttribute::ON  );  
	//	stateset->setMode( GL_LIGHTING, osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED );  
		//stateset->setRenderBinDetails(0, "RenderBin");
		//设置渲染优先级------级别理论上来讲 比你背景的node靠后就行，没设置过的是-1. 

		geode.getDrawable(i)->setStateSet(stateset);


		//geode.getDrawable(i)->getStateSet()->setRenderBinMode(osg::StateSet::RenderBinMode::INHERIT_RENDERBIN_DETAILS);
		//	apply(geode.getDrawable(i)->getStateSet());
	}
	traverse(geode);
}


