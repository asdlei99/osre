/*-----------------------------------------------------------------------------------------------
The MIT License (MIT)

Copyright (c) 2015-2020 OSRE ( Open Source Render Engine ) by Kim Kulling

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
-----------------------------------------------------------------------------------------------*/
#include <osre/App/AppBase.h>
#include <osre/App/AssetRegistry.h>
#include <osre/App/AssimpWrapper.h>
#include <osre/App/Component.h>
#include <osre/App/Entity.h>
#include <osre/App/World.h>
#include <osre/IO/Uri.h>
#include <osre/Math/BaseMath.h>
#include <osre/Platform/AbstractWindow.h>
#include <osre/Properties/Settings.h>
#include <osre/RenderBackend/RenderBackendService.h>
#include <osre/RenderBackend/RenderCommon.h>
#include <osre/Scene/DbgRenderer.h>
#include <osre/Scene/MeshBuilder.h>
#include <osre/Scene/Node.h>
#include <osre/Scene/Stage.h>
#include <osre/Scene/View.h>

using namespace ::OSRE;
using namespace ::OSRE::App;
using namespace ::OSRE::Common;
using namespace ::OSRE::RenderBackend;
using namespace ::OSRE::Scene;

// To identify local log entries
static const c8 *Tag = "ModelLoadingApp";

// The file to load
static const char *ModelPath = "file://assets/Models/Obj/spider.obj";

static const char *AssetFolderArg = "asset_folder";

static const char *ModelArg = "model";

/// The example application, will create the render environment and render a simple triangle onto it
class ModelLoadingApp : public App::AppBase {
    String m_assetFolder;
    Scene::Stage *m_stage;
    Scene::View *m_view;
    f32 m_angle;
    glm::mat4 m_model;
    TransformMatrixBlock m_transformMatrix;
    Node::NodePtr m_modelNode;

public:
    ModelLoadingApp(int argc, char *argv[]) :
            AppBase(argc, (const char **)argv, "api:model", "The render API:The model to load"),
            m_assetFolder(""),
            m_stage(nullptr),
            m_view(nullptr),
            m_angle(0.0f),
            m_model(),
            m_transformMatrix(),
            m_modelNode() {
        // empty
    }

    ~ModelLoadingApp() override {
        // empty
    }

protected:
    bool onCreate() override {
        if (!AppBase::onCreate()) {
            return false;
        }
        AppBase::setWindowsTitle("ModelLoader sample!");

        const Common::ArgumentParser &parser = AppBase::getArgumentParser();
        if (parser.hasArgument(AssetFolderArg)) {
            m_assetFolder = parser.getArgument(AssetFolderArg);
        }

        IO::Uri modelLoc(ModelPath);
        if (parser.hasArgument(ModelArg)) {
            String modelArg = parser.getArgument(ModelArg);
            String model = "file://assets/" + modelArg;
            modelLoc.setPath(model);
        }

#ifdef OSRE_WINDOWS
        AssetRegistry::registerAssetPath("assets", "../../media");
#else
        AssetRegistry::registerAssetPath("assets", "../media");
#endif
        AssimpWrapper assimpWrapper(*getIdContainer());
        if (assimpWrapper.importAsset(modelLoc, 0)) {
            RenderBackendService *rbSrv(getRenderBackendService());
            if (nullptr == rbSrv) {
                return false;
            }

            Platform::AbstractWindow *rootWindow(getRootWindow());
            if (nullptr == rootWindow) {
                return false;
            }

            m_stage = AppBase::createStage("ModelLoading");
            AppBase::setActiveStage(m_stage);
            Scene::View *view = m_stage->addView("default_view", nullptr);
            AppBase::setActiveView(view);

            Rect2ui windowsRect;
            rootWindow->getWindowsRect(windowsRect);
            view->setProjectionParameters(60.f, (f32)windowsRect.m_width, (f32)windowsRect.m_height, 0.01f, 1000.f);
            Entity *entity = assimpWrapper.getEntity();

            World *world = getActiveWorld();
            world->addEntity(entity);
            view->observeBoundingBox(entity->getAABB());
            m_modelNode = entity->getNode();
        }

        return true;
    }

    void onUpdate() override {

        // Rotate the model
        glm::mat4 rot(1.0);
        m_transformMatrix.m_model = glm::rotate(rot, m_angle, glm::vec3(0, 1, 1));

        m_angle += 0.01f;
        RenderBackendService *rbSrv(getRenderBackendService());

        rbSrv->beginPass(PipelinePass::getPassNameById(RenderPassId));
        rbSrv->beginRenderBatch("b1");

        rbSrv->setMatrix(MatrixType::Model, m_transformMatrix.m_model);

        rbSrv->endRenderBatch();
        rbSrv->endPass();

        Scene::DbgRenderer::getInstance()->renderDbgText(1, 1, 2U, "XXX");

        AppBase::onUpdate();
    }
};

int main(int argc, char *argv[]) {
    ModelLoadingApp myApp(argc, argv);
    if (!myApp.initWindow(10, 10, 1024, 768, "ModelLoader-Sample", false, App::RenderBackendType::OpenGLRenderBackend)) {
        return 1;
    }

    while (myApp.handleEvents()) {
        myApp.update();
        myApp.requestNextFrame();
    }

    myApp.destroy();

    return 0;
}
