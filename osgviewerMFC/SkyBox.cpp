#include "SkyBox.h"


osg::TextureCubeMap* SkyBox::readCubeMap(const std::string &strImagePath)
{
	osg::TextureCubeMap* cubemap = new osg::TextureCubeMap;
	//#define CUBEMAP_FILENAME(face) "nvlobby_" #face ".png"
	//#define CUBEMAP_FILENAME(face) "Cubemap_axis/" #face ".png"
//#define CUBEMAP_FILENAME(face) "res/" #face ".bmp"

	std::string strImagePosX = strImagePath + "\\posx.jpg";
	std::string strImageNegX = strImagePath + "\\negx.jpg";
	std::string strImagePosY = strImagePath + "\\posy.jpg";
	std::string strImageNegY = strImagePath + "\\negy.jpg";
	std::string strImagePosZ= strImagePath + "\\posz.jpg";
	std::string strImageNegZ = strImagePath + "\\negz.jpg";

	osg::Image* imagePosX = osgDB::readImageFile(strImagePosX);

	osg::Image* imageNegX = osgDB::readImageFile(strImageNegX);
	osg::Image* imagePosY = osgDB::readImageFile(strImagePosY);
	osg::Image* imageNegY = osgDB::readImageFile(strImageNegY);
	osg::Image* imagePosZ = osgDB::readImageFile(strImagePosZ);
	osg::Image* imageNegZ = osgDB::readImageFile(strImageNegZ);

	if (imagePosX && imageNegX && imagePosY /*&& imageNegY && imagePosZ && imageNegZ*/)
	{
		cubemap->setImage(osg::TextureCubeMap::POSITIVE_X, imagePosX);
		cubemap->setImage(osg::TextureCubeMap::NEGATIVE_X, imageNegX);
		cubemap->setImage(osg::TextureCubeMap::POSITIVE_Y, imagePosY);
		cubemap->setImage(osg::TextureCubeMap::NEGATIVE_Y, imageNegY);
		cubemap->setImage(osg::TextureCubeMap::POSITIVE_Z, imagePosZ);
		cubemap->setImage(osg::TextureCubeMap::NEGATIVE_Z, imageNegZ);

		cubemap->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_EDGE);
		cubemap->setWrap(osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_EDGE);
		cubemap->setWrap(osg::Texture::WRAP_R, osg::Texture::CLAMP_TO_EDGE);

		cubemap->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR_MIPMAP_LINEAR);
		cubemap->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);
	}

	return cubemap;
}


osg::Node* SkyBox::createSkyBox(const std::string &strImagePath)
{
	osg::ref_ptr<osg::StateSet> stateset = new osg::StateSet();

	osg::ref_ptr<osg::TexEnv> te = new osg::TexEnv;
	te->setMode(osg::TexEnv::REPLACE);
	stateset->setTextureAttributeAndModes(0, te, osg::StateAttribute::ON);

	osg::ref_ptr<osg::TexGen> tg = new osg::TexGen;
	tg->setMode(osg::TexGen::NORMAL_MAP);
	stateset->setTextureAttributeAndModes(0, tg, osg::StateAttribute::ON);

	osg::ref_ptr<osg::TexMat> tm = new osg::TexMat;
	stateset->setTextureAttribute(0, tm);

	osg::TextureCubeMap* skymap = readCubeMap(strImagePath);

	stateset->setTextureAttributeAndModes(0, skymap, osg::StateAttribute::ON);

	stateset->setMode( GL_LIGHTING, osg::StateAttribute::OFF );
	stateset->setMode( GL_CULL_FACE, osg::StateAttribute::OFF );

	// clear the depth to the far plane.
	osg::ref_ptr<osg::Depth> depth = new osg::Depth;
	depth->setFunction(osg::Depth::ALWAYS);
	depth->setRange(1.0,1.0);
	stateset->setAttributeAndModes(depth, osg::StateAttribute::ON );

	stateset->setRenderBinDetails(-1,"RenderBin");

	osg::ref_ptr<osg::Drawable> drawable = new osg::ShapeDrawable(new osg::Sphere(osg::Vec3(0.0f,0.0f,0.0f),100));

	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	geode->setCullingActive(false);
	geode->setStateSet( stateset );
	geode->addDrawable(drawable);

	osg::ref_ptr<osg::Transform>transform = new MoveEarthySkyWithEyePointTransform;
	transform->setCullingActive(false);
	transform->addChild(geode);

	osg::ref_ptr<osg::ClearNode> clearNode = new osg::ClearNode;
	//   clearNode->setRequiresClear(false);
	clearNode->setCullCallback(new TexMatCallback(*tm));
	clearNode->addChild(transform);

	return clearNode.release();
}