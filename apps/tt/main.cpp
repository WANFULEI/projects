
#include <QtGui/QApplication>
#include "dem/dem.h"
#include "QDebug"
#include <iostream>
using namespace std;
#include <QFile>
#include "QtGui/QPixmap"
#include "QThread"
#include <windows.h>
#include "osg/Group"
#include "osgViewer/Viewer"
#include "osg/ShapeDrawable"
#include "osg/Image"
#include "osg/Texture2D"
#include "osg/StateSet"
#include "osgDB/ReadFile"
#include "osg/TexEnv"
#include "osgText/Text"
#include "osg/PositionAttitudeTransform"
#include "osg/MatrixTransform"
#include "osgSim/MultiSwitch"
#include "osgSim/DOFTransform"

using namespace osg;
using namespace osgViewer;

int main(int argc, char *argv[])
{
	osg::Group* root = NULL; 
	osg::Node* tankNode = NULL;     
	osg::Node* terrainNode = NULL;     
	osg::PositionAttitudeTransform* tankXform; 

	//HUD 几何体的叶节点      
	osg::Geode* HUDGeode = new osg::Geode();    
	//  用于作 HUD 显示的 Text 实例 
	osgText::Text* textOne = new osgText::Text(); 

	// 这个文字实例将跟随坦克显示        
	osgText::Text* tankLabel = new osgText::Text();        
	// 投影节点用于定义 HUD 的视景体（view frustrum）      
	osg::Projection* HUDProjectionMatrix = new osg::Projection; 
	//从文件中读取模型，并设置场景图形的结构，参见前一章教程。        
	// 初始化根节点       
	root = new osg::Group(); 

	osgDB::FilePathList pathList = osgDB::Registry::instance()->getDataFilePathList(); 
	pathList.push_back("C:\\Projects\\OpenSceneGraph\\OpenSceneGraph-Data\\NPSData\\Models\\T72-Tank\\");       
	pathList.push_back("C:\\Projects\\OpenSceneGraph\\OpenSceneGraph-Data\\NPSData\\Models\\JoeDirt\\");      
	pathList.push_back("C:\\Projects\\OpenSceneGraph\\OpenSceneGraph-Data\\NPSData\\Textures\\");    
	osgDB::Registry::instance()->setDataFilePathList(pathList);

	// 读入模型并关联到节点        
	tankNode = osgDB::readNodeFile("glsl_confetti.osgt");      
	terrainNode = osgDB::readNodeFile("lz.osgt"); 

	// 初始化位置变换节点，用于放置坦克模型      
	tankXform = new osg::PositionAttitudeTransform();      
	tankXform->setPosition( osg::Vec3d(5,5,8) ); 

	// 构件场景。将地形节点和位置变换节点关联到根节点     
	root->addChild(terrainNode);        
	root->addChild(tankXform);    
	tankXform->addChild(tankNode); 

	// 将投影矩阵添加到根节点。投影矩阵的水平和垂直范围与屏幕的尺寸   
	// 相同。因此该节点子树中的位置坐标将等同于像素坐标    
	HUDProjectionMatrix->setMatrix(osg::Matrix::ortho2D(0,1024,0,768));             
	// HUD 模型观察矩阵应使用单位矩阵       
	osg::MatrixTransform* HUDModelViewMatrix = new osg::MatrixTransform;     
	HUDModelViewMatrix->setMatrix(osg::Matrix::identity()); 

	// 确保模型观察矩阵不会被场景图形的位置变换影响       
	HUDModelViewMatrix->setReferenceFrame(osg::Transform::ABSOLUTE_RF); 

	// 添加 HUD 投影矩阵到根节点，添加 HUD 模型观察矩阵到 HUD 投影矩阵。 
	// 模型观察矩阵节点的所有子节点都可以使用该投影矩阵进行视景       
	// 浏览，并使用模型观察矩阵来安置位置    
	root->addChild(HUDProjectionMatrix);     
	HUDProjectionMatrix->addChild(HUDModelViewMatrix);
	//现在我们设置几何体。我们创建一个与屏幕坐标对齐的四边形，并设置其颜色和纹理参数。

	// 将包含 HUD 几何体的 Geode 节点作为 HUD 模型观察矩阵的子节点   
	HUDModelViewMatrix->addChild( HUDGeode ); 

	//  设置 HUD 的四边形背景，并添加到 Geode 节点     
	osg::Geometry* HUDBackgroundGeometry = new osg::Geometry(); 

	osg::Vec3Array* HUDBackgroundVertices = new osg::Vec3Array;    
	HUDBackgroundVertices->push_back( osg::Vec3( 0,    0,-1) );      
	HUDBackgroundVertices->push_back( osg::Vec3(1024,  0,-1) );       
	HUDBackgroundVertices->push_back( osg::Vec3(1024,200,-1) );   
	HUDBackgroundVertices->push_back( osg::Vec3(   0,200,-1) ); 

	osg::DrawElementsUInt* HUDBackgroundIndices = new osg::DrawElementsUInt(osg::PrimitiveSet::POLYGON, 0);     
	HUDBackgroundIndices->push_back(0);      
	HUDBackgroundIndices->push_back(1);  
	HUDBackgroundIndices->push_back(2);    
	HUDBackgroundIndices->push_back(3); 

	osg::Vec4Array* HUDcolors = new osg::Vec4Array;  
	HUDcolors->push_back(osg::Vec4(0.8f,0.8f,0.8f,0.8f)); 

	osg::Vec2Array* texcoords = new osg::Vec2Array(4);     
	(*texcoords)[0].set(0.0f,0.0f);       
	(*texcoords)[1].set(1.0f,0.0f);      
	(*texcoords)[2].set(1.0f,1.0f);      
	(*texcoords)[3].set(0.0f,1.0f); 

	HUDBackgroundGeometry->setTexCoordArray(0,texcoords);  
	osg::Texture2D* HUDTexture = new osg::Texture2D;       
	HUDTexture->setDataVariance(osg::Object::DYNAMIC);     
	osg::Image* hudImage;      
	hudImage = osgDB::readImageFile("HUDBack2.tga");   
	HUDTexture->setImage(hudImage);     
	osg::Vec3Array* HUDnormals = new osg::Vec3Array;      
	HUDnormals->push_back(osg::Vec3(0.0f,0.0f,1.0f));    
	HUDBackgroundGeometry->setNormalArray(HUDnormals);    
	HUDBackgroundGeometry->setNormalBinding(osg::Geometry::BIND_OVERALL);
	HUDBackgroundGeometry->addPrimitiveSet(HUDBackgroundIndices);    
	HUDBackgroundGeometry->setVertexArray(HUDBackgroundVertices);     
	HUDBackgroundGeometry->setColorArray(HUDcolors);    
	HUDBackgroundGeometry->setColorBinding(osg::Geometry::BIND_OVERALL); 

	HUDGeode->addDrawable(HUDBackgroundGeometry);

	//设置渲染状态，使用上面定义的纹理        
	osg::StateSet* HUDStateSet = new osg::StateSet();     
	HUDGeode->setStateSet(HUDStateSet);       
	HUDStateSet->setTextureAttributeAndModes(0,HUDTexture,osg::StateAttribute::ON); 

	//打开 GL_BLEND 混合模式（以保证 Alpha 纹理正确） 
	HUDStateSet->setMode(GL_BLEND,osg::StateAttribute::ON); 

	// 禁止深度测试，因此几何体将忽略已绘制物体的深度值，直接进行绘制  
	HUDStateSet->setMode(GL_DEPTH_TEST,osg::StateAttribute::OFF);     
	HUDStateSet->setRenderingHint( osg::StateSet::TRANSPARENT_BIN ); 

	// 确认该几何题在最后进行绘制。渲染元（RenderBin）按照数字顺序执行渲染，因此我们设置一个较大 的数字值   
	HUDStateSet->setRenderBinDetails( 11, "RenderBin"); 
	//最后我们设置文字。由于 osg::Text 继承自 osg::Drawable，其实例可以作为 osg::Geode 实例的子节点添加到场景 中。  
	// 添加文字到 Geode 叶节点中    
	HUDGeode->addDrawable( textOne ); 

	// 设置 HUD 文字的参数 
	textOne->setCharacterSize(25);      
	textOne->setFont("C:/WINDOWS/Fonts/impact.ttf");  
	textOne->setText("Not so good");      
	textOne->setAxisAlignment(osgText::Text::SCREEN);     
	textOne->setPosition( osg::Vec3(360,165,-1.5) );      
	textOne->setColor( osg::Vec4(199, 77, 15, 1) ); 

	// 声明一个叶节点来保存坦克的标签文字      
	osg::Geode* tankLabelGeode = new osg::Geode(); 

	// 将坦克标签添加到场景中      
	tankLabelGeode->addDrawable(tankLabel);   
	tankXform->addChild(tankLabelGeode); 

	// 设置坦克标签文字的参数，与坦克的坐标对齐，    
	// 这里使用 XZ_PLANE 保证文字与坦克的 XZ 平面对齐  
	tankLabel->setCharacterSize(5);      
	tankLabel->setFont("/fonts/arial.ttf");    
	tankLabel->setText("Tank #1");    
	tankLabel->setAxisAlignment(osgText::Text::XZ_PLANE); 

	// 设置文字渲染时包括一个对齐点和包围矩形    
	tankLabel->setDrawMode(osgText::Text::TEXT | osgText::Text::ALIGNMENT | osgText::Text::BOUNDINGBOX); 

	tankLabel->setAlignment(osgText::Text::CENTER_TOP);  
	tankLabel->setPosition( osg::Vec3(0,0,8) );      
	tankLabel->setColor( osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f) ); 
	//最后，设置视景类并进入仿真循环。        
	osgViewer::Viewer viewer; 

	viewer.setSceneData( root );

	osgSim::MultiSwitch

	return viewer.run(); 

}
