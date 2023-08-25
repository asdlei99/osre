///
///
// To identify local log entries we will define this tag.
static constexpr c8 Tag[] = "TerrainRenderingApp";

/// The example application, will create the render environment and render a simple triangle onto it
class TerrainRenderingApp : public App::AppBase {
    /// The transform block, contains the model-, view- and projection-matrix
    TransformMatrixBlock mTransformMatrix;
    /// The entity to render
    Entity *mEntity;
    /// The keyboard controller instance.
    Animation::AnimationControllerBase *mKeyboardTransCtrl;

public:
    TerrainRenderingApp(int argc, char *argv[]) :
            AppBase(argc, (const char **)argv),
            mTransformMatrix(),
            mEntity(nullptr),
            mKeyboardTransCtrl(nullptr) {
        // empty
    }
    
    ~TerrainRenderingApp() override = default;

protected:
    Camera *setupCamera(World *world) {
        Entity *camEntity = new Entity("camera", *getIdContainer(), world);
        world->addEntity(camEntity);
        Camera *camera =(Camera*) camEntity->createComponent(ComponentType::CameraComponentType);
        world->setActiveCamera(camera);
        ui32 w, h;
        AppBase::getResolution(w, h);
        camera->setProjectionParameters(60.f, (f32)w, (f32)h, 0.001f, 1000.f);
        
        return camera;
    }

    bool onCreate() override {
        if (!AppBase::onCreate()) {
            return false;
        }

        AppBase::setWindowsTitle("Hello-World sample! Rotate with keyboard: w, a, s, d, scroll with q, e");
        World *world = getStage()->addActiveWorld("hello_world");
        mEntity = new Entity("entity", *AppBase::getIdContainer(), world);
        Camera *camera = setupCamera(world);

        MeshBuilder meshBuilder;
        RenderBackend::Mesh *mesh = meshBuilder.createCube(VertexType::ColorVertex, .5,.5,.5,BufferAccessType::ReadOnly).getMesh();
        if (nullptr != mesh) {
            RenderComponent *rc = (RenderComponent*) mEntity->getComponent(ComponentType::RenderComponentType);
            rc->addStaticMesh(mesh);

            Time dt;
            world->update(dt);
            camera->observeBoundingBox(mEntity->getAABB());
        }
        mKeyboardTransCtrl = AppBase::getTransformController(mTransformMatrix);

        osre_info(Tag, "Creation finished.");

        return true;
    }

    void onUpdate() override {
        Platform::Key key = AppBase::getKeyboardEventListener()->getLastKey();
        mKeyboardTransCtrl->update(TransformController::getKeyBinding(key));

        RenderBackendService *rbSrv = ServiceProvider::getService<RenderBackendService>(ServiceType::RenderService);
        rbSrv->beginPass(RenderPass::getPassNameById(RenderPassId));
        rbSrv->beginRenderBatch("b1");

        rbSrv->setMatrix(MatrixType::Model, m_transformMatrix.m_model);

        rbSrv->endRenderBatch();
        rbSrv->endPass();

        AppBase::onUpdate();
    }
};