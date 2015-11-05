#pragma once

#include "common.h"
#include <TravelManipulator.h>

class cOSG
{
public:
    cOSG(HWND hWnd);
    ~cOSG();

  //  void InitOSG(std::string filename);
	bool InitOSG(std::string modelname);
	void InitManipulators(void);
   // void InitSceneGraph(void);
	bool InitSceneGraph(void);
	void InitCameraConfig(void);
    void SetupWindow(void);
    void SetupCamera(void);
    void PreFrameUpdate(void);
    void PostFrameUpdate(void);
    void Done(bool value) { mDone = value; }
    bool Done(void) { return mDone; }
    static void Render(void* ptr);

    osgViewer::Viewer* getViewer() { return mViewer; }
	osg::Node * CreateClockFace ();
	osg::Node* CreateHandler(osg::Vec3 &res, osg::Vec3 &des);
	osg::AnimationPath* createAnimationPath(const osg::Vec3& center, float radius, double looptime, float angle);
	osg::Node* CreateMoveHandler(void);
	bool bErrModel;

private:
	
    bool mDone;
    std::string m_ModelName;
    HWND m_hWnd;
    osgViewer::Viewer* mViewer;
    osg::ref_ptr<osg::Group> mRoot;
    osg::ref_ptr<osg::Node> mModel;
    osg::ref_ptr<osgGA::TrackballManipulator> trackball;
	osg::ref_ptr<TravelManipulator> travelManipulator;

    osg::ref_ptr<osgGA::KeySwitchMatrixManipulator> keyswitchManipulator;

	
};



class OptTexture:public osg::NodeVisitor{
public:
	OptTexture();
	~OptTexture();

	virtual void apply(osg::Node& node);
	virtual void apply(osg::Geode& geode);

};
