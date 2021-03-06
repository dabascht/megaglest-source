#include "main.h"
#include "glwidget.h"
#include <iostream>
#include <QFile>
#include <QWheelEvent>
#include "xml_parser.h"
#include "properties.h"

#if QT_VERSION >= 0x050400
#include <QOpenGLContext>
#endif

//~ //~
//~ //~
namespace Shared{ namespace G3dViewer{

#ifdef _WIN32
const char *folderDelimiter = "\\";
#else
const char *folderDelimiter = "/";
#endif

GLWidget::GLWidget( /*const GLFormat& format,*/ QWidget* parent):
#if QT_VERSION >= 0x050400
QOpenGLWidget( parent ),
#else
QGLWidget( parent ),
#endif
        renderer(Renderer::getInstance()),rotX(50),rotY(50),zoom(1),model(NULL),playerColor(0xFF,0x00,0x00){
            #if QT_VERSION >= 0x050400
            QSurfaceFormat glFormat;
            #else
            QGLFormat glFormat;
            #endif
            glFormat.setVersion( 1, 3 );
            glFormat.setProfile( glFormat.NoProfile ); // Requires >=Qt-4.8.0
            //glFormat.setSampleBuffers( true );//why did I do that at all?
            setFormat(glFormat);
}

void GLWidget::loadModel(QString path) {
    try{
        if(path != "" && QFile(path).exists()) {
            this->modelPathList.push_back(path.toStdString());
            std::cout << "Adding model " << path.toStdString() << " list size " << this->modelPathList.size() << std::endl;
        }

        //~ string titlestring=winHeader;
        for(unsigned int idx =0; idx < this->modelPathList.size(); idx++) {
            string modelPath = this->modelPathList[idx];
//~
            //~ //printf("Loading model [%s] %u of " MG_SIZE_T_SPECIFIER "\n",modelPath.c_str(),idx, this->modelPathList.size());
//~
            //~ if(timer) timer->Stop();
            delete model;
            model = renderer? renderer->newModel(rsGlobal, modelPath): NULL;
//~
            //~ statusbarText = getModelInfo();
            //~ string statusTextValue = statusbarText + " animation speed: " + floatToStr(speed * 1000.0) + " anim value: " + floatToStr(anim) + " zoom: " + floatToStr(zoom) + " rotX: " + floatToStr(rotX) + " rotY: " + floatToStr(rotY);
            //~ GetStatusBar()->SetStatusText(ToUnicode(statusTextValue.c_str()));
            //~ if(timer) timer->Start(100);
            //~ titlestring =  extractFileFromDirectoryPath(modelPath) + " - "+ titlestring;
        }
        updateGL();
        //~ SetTitle(ToUnicode(titlestring));*
    }
    catch(Shared::Platform::megaglest_runtime_error &e) {
        std::cout << e.what() << std::endl;
        //wxMessageDialog(NULL, ToUnicode(e.what()), ToUnicode("Error"), wxOK | wxICON_ERROR).ShowModal();
        ((MainWindow*)parentWidget())->showRuntimeError("Loading G3D File",e);
    }
}

//stuff commented out becauso I'm still trying out hw to make it best
void GLWidget::loadParticle(QString path) {
    if(path != "" && QFile(path).exists() ) {
        //ends animation stuff?
        renderer->end();
        unitParticleSystems.clear();
        unitParticleSystemTypes.clear();

        //~ if(isControlKeyPressed == true) {
            //~ // std::cout << "Adding to list..." << std::endl;
            //~ this->particlePathList.push_back(path);
        //~ }
        //~ else {
            // std::cout << "Clearing list..." << std::endl;
            this->particlePathList.clear();
            this->particlePathList.push_back(path.toStdString());
        //~ }
    }
    try{if(this->particlePathList.empty() == false) {
        for(unsigned int idx = 0; idx < this->particlePathList.size(); idx++) {
            string particlePath = this->particlePathList[idx];

            QStringList dir = path.split(folderDelimiter);
            dir.pop_back();

            //get unitpath
            std::string unitXML = dir.join(folderDelimiter).toStdString();
            if(!dir.isEmpty()){
                unitXML += folderDelimiter + dir.last().toStdString() + ".xml";
            }

            int size   = 1;
            int height = 1;

            //get the size and height of the unit in this folder
            if(!dir.isEmpty() && fileExists(unitXML) == true) {
                using namespace Shared::Xml;
                //~ if(SystemFlags::VERBOSE_MODE_ENABLED) printf("In [%s::%s Line: %d] loading [%s] idx = %u\n",extractFileFromDirectoryPath(__FILE__).c_str(),__FUNCTION__,__LINE__,unitXML.c_str(),idx);

                XmlTree xmlTree;
                xmlTree.load(unitXML,Shared::Util::Properties::getTagReplacementValues() );
                const XmlNode *unitNode= xmlTree.getRootNode();
                const XmlNode *parametersNode= unitNode->getChild("parameters");
                //size
                size= parametersNode->getChild("size")->getAttribute("value")->getIntValue();
                //height
                height= parametersNode->getChild("height")->getAttribute("value")->getIntValue();
            }

            std::cout << "size: " << size << "; height: " << height << std::endl;

            std::map<string,vector<pair<string, string> > > loadedFileList;
            UnitParticleSystemType *unitParticleSystemType = new UnitParticleSystemType();
            unitParticleSystemType->load(NULL, dir.join(folderDelimiter).toStdString(), path.toStdString(), //### if we knew which particle it was, we could be more accurate
                    renderer,loadedFileList,"g3dviewer","");
            unitParticleSystemTypes.push_back(unitParticleSystemType);


            //I have no Idea what I’m doing here
            for(std::vector<UnitParticleSystemType *>::const_iterator it= unitParticleSystemTypes.begin(); it != unitParticleSystemTypes.end(); ++it) {
                std::cout << "ups!" << std::endl;
                UnitParticleSystem *ups= new UnitParticleSystem(200);
                (*it)->setValues(ups);
                //resize particles
                if(size > 0) {
                    Vec3f vec = Vec3f(0.f, height / 2.f, 0.f);
                    ups->setPos(vec);
                }
                ups->setFactionColor(Vec3f(playerColor.redF(), playerColor.greenF(), playerColor.blueF()));
                unitParticleSystems.push_back(ups);
                renderer->manageParticleSystem(ups);

                ups->setVisible(true);
            }
            //TODO: remove when animation got implemented
            for(int i=0; i< 100; ++i) {
                //does a animation step or something like that
                renderer->updateParticleManager();
            }

            if(path != "" && QFile(path).exists()){//otherwise nothing changed
                //~ renderer->initModelManager();
                renderer->initTextureManager();
            }
        }
        updateGL();
    }}
    catch(Shared::Platform::megaglest_runtime_error &e) {
        std::cout << e.what() << std::endl;
        //wxMessageDialog(NULL, ToUnicode(e.what()), ToUnicode("Error"), wxOK | wxICON_ERROR).ShowModal();
        ((MainWindow*)parentWidget())->showRuntimeError("Not a Mega-Glest particle XML file, or broken",e);
    }
}

void GLWidget::loadSplashParticle(QString path) {
}

void GLWidget::loadProjectileParticle(QString path) {
}

void GLWidget::setBackgroundColor(const QColor &col) {
    try{
        renderer->setBackgroundColor(col.redF(), col.greenF(), col.blueF());
        updateGL();
    }
    catch(Shared::Platform::megaglest_runtime_error &e) {
        std::cout << e.what() << std::endl;
        ((MainWindow*)parentWidget())->showRuntimeError("Setting Background Color",e);
    }
}

void GLWidget::screenshot(QString path, bool transparent) {
    makeCurrent();
    renderer->setAlphaColor(transparent ? 0.0f : 1.0f);
    #if QT_VERSION >= 0x050400
    update();
    QPixmap pixmap(size());
    render(&pixmap);//no special method for pixmaps anymore
    pixmap.toImage().save(path);
    #else
    context()->swapBuffers();//since Qt 5.4 this is not needed any more
    renderPixmap().toImage().save(path);
    #endif
}

void GLWidget::setPlayerColor(const QColor &col) {
    try{
        playerColor = col;
        renderer->setPlayerColor(col.redF(), col.greenF(), col.blueF());
        renderer->initModelManager();
        renderer->initTextureManager();
        updateGL();
    }
    catch(Shared::Platform::megaglest_runtime_error &e) {
        std::cout << e.what() << std::endl;
        ((MainWindow*)parentWidget())->showRuntimeError("Setting Player Color",e);
    }
    //~ for(unsigned int idx = 0; idx < this->particlePathList.size(); idx++) {
        //~ setFactionColor(Vec3f(col.redF(), col.greenF(), col.blueF()));
    //~ }
}


void GLWidget::toggleNormals() {
    renderer->toggleNormals();
    updateGL();
}

void GLWidget::toggleWireframe() {
    makeCurrent();//use opengl on this widget
    renderer->toggleWireframe();
    #if QT_VERSION < 0x050400
    context()->swapBuffers();//push stuff to GPU//
    #endif
    updateGL();
}

void GLWidget::toggleGrid() {
    renderer->toggleGrid();
    updateGL();
}

void GLWidget::clearAll() {
    //resets everything we added to the scene
    modelPathList.clear();
    particlePathList.clear();
    //~ particleProjectilePathList.clear();
    //~ particleSplashPathList.clear(); // as above
    //~ //if(timer) timer->Stop();
    if(renderer) renderer->end();
    unitParticleSystems.clear();
    unitParticleSystemTypes.clear();
    //~ projectileParticleSystems.clear();
    //~ projectileParticleSystemTypes.clear();
    //~ splashParticleSystems.clear(); // as a//TODO: FIX!bove
    //~ splashParticleSystemTypes.clear();
    delete model;
    model = NULL;
    //~ loadUnit("","");
    loadModel("");
    //~ loadParticle("");
    //~ loadProjectileParticle("");
    //~ loadSplashParticle(""); // as above
    }

void GLWidget::initializeGL() {
    GLuint err = glewInit();//init GLEW, renderer needs this
    if (GLEW_OK != err) {
        fprintf(stderr, "Error [main]: glewInit failed: %s\n", glewGetErrorString(err));
        //return 1;
        throw std::runtime_error((char *)glewGetErrorString(err));
    }
    renderer->init();
    renderer->setDimension(width(),height());
    renderer->setPlayerColor(playerColor.redF(), playerColor.greenF(), playerColor.blueF());
    renderer->initTextureManager();
}

void GLWidget::resizeGL(int w, int h) {
    renderer->setDimension(w,h);
}

//void GLWidget::resizeGL( int w, int h ){}
void GLWidget::paintGL() {

    //back to blank background
    renderer->reset();
    renderer->transform(rotX, rotY, zoom);
    renderer->renderGrid();
    //context()->swapBuffers();

    renderer->renderTheModel(model, 0);
    /*for(int i=0; i< 10; ++i) {
        renderer->updateParticleManager();
    }*/
    renderer->renderParticleManager();

    if(!modelPathList.empty()){// && resetAnimation && haveLoadedParticles) {
        //~ if(anim >= resetAnim && resetAnim > 0) {
            std::cout << "RESETTING EVERYTHING ...\n" << std::endl;
            //~ fflush(stdout);

            //~ resetAnimation        = false;
            //~ particleLoopStart     = resetParticleLoopStart;

            //~ wxCommandEvent event;
            //~ if(unitPath.first != "") {
                //onMenuFileClearAll(event)

                modelPathList.clear();
                particlePathList.clear();
                //~ particleProjectilePathList.clear();
                //~ particleSplashPathList.clear(); // as above

                //~ onMenuRestart(event);
            //~ }
            //~ else {
                //~ onMenuRestart(event);
            //~ }
        //~ }
    }
}

void GLWidget::wheelEvent ( QWheelEvent* e) {
    if(e->delta() > 0) {
        zoom*= 1.1f;
    }
    else{
        zoom*= 0.90909f;
    }
    updateGL();
}

void GLWidget::mousePressEvent(QMouseEvent* e){
    std::cout << "mouse press" << std::endl;
    oldRotX = rotX;
    oldRotY = rotY;
    oldPos = e->pos();
    e->accept();
}
void GLWidget::mouseMoveEvent(QMouseEvent* e){
    std::cout << "mouse move" << std::endl;
    QPoint currPos = mapFromGlobal(QCursor::pos());
    rotX = oldRotX + (e->pos().x() - oldPos.x()) / 4;
    rotY = oldRotY + (e->pos().y() - oldPos.y()) / 4;
    updateGL();
    e->accept();
}

#if QT_VERSION >= 0x050400
void GLWidget::updateGL(){
    update();
}
#endif

}}
