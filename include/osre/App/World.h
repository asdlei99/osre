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
#pragma once

#include <osre/App/AppCommon.h>
#include <osre/Scene/SceneCommon.h>
#include <osre/Common/Object.h>
#include <osre/Common/Ids.h>
#include <cppcore/Container/TArray.h>
#include <cppcore/Container/THashMap.h>

namespace OSRE {
namespace App {

class Entity;

//-------------------------------------------------------------------------------------------------
///	@ingroup	Engine
///
///	@brief  This class is used to create a world for the engine. Worlds are the container for all
/// content which shall be rendered.
/// A world is build by set a stage. A stage is the container for scene nodes ( @see Stage ). 
/// A world can be seen by defining an active view. A view represents the camera for looking into 
/// the stage ( @see View ). 
//-------------------------------------------------------------------------------------------------
class OSRE_EXPORT World : public Common::Object {
public:
    /// @brief  The class constructor with the name and the requested render-mode.
    /// @param  worldName   [in] The world name.
    /// @param  renderMode  [in] The requested render mode. @see RenderMode
    explicit World( const String &worldName, RenderMode renderMode = RenderMode::Render3D );

    /// @brief  The class destructor.
    virtual ~World();

    /// @brief  Will add a new stage to the world. @see Stage
    /// @param  stage       [in] A pointer showing to the new stage.
    void addStage( Scene::Stage *stage );

    /// @brief  Will set the active stage.
    /// @param  activeStage     [in] A pointer showing to the active stage.
    /// @return true, if successful, false if stage was not attached before.
    Scene::Stage *setActiveStage( Scene::Stage *activeStage );

    /// @brief  Will set the active stage.
    /// @param  stageName       [in] The name of the stage-
    /// @return true, if successful, false if stage was not attached before.
    Scene::Stage *setActiveStage( const String &stageName );

    /// @brief  Will return the number of stages in the world. 
    /// @return The number of stages.
    size_t getNumStages() const;

    /// @brief  Returns the stage at the given index.
    /// @param  i       [in] The index for the stage.
    /// @return A pointer showing to the stage or nullptr if the index is invalid.
    Scene::Stage *getStageAt(ui32 i) const;

    /// @brief  Will return a pointer showing to the active stage.
    /// @return A pointer showing to the active stage or nullptr if no active stage was set before.
    Scene::Stage *getActiveStage() const;

    /// @brief  Will add a new view.
    /// @param  view        [in] A pointer showing to the view.
    void addView( Scene::View *view);

    /// @brief  Will set the active view.
    /// @param  activeView  [i] A pointer showing to the active view.
    /// @return true, if set was successful, false if the view is not part of the world.
    Scene::View *setActiveView( Scene::View *activeView );

    /// @brief  Will set the active view.
    /// @param  viewName    [in] The name of the view.
    /// @return true, if set was successful, false if the view is not part of the world.
    Scene::View *setActiveView( const String &viewName );

    /// @brief  Will return the number of assigned views.
    /// @return The number of assigned views.
    size_t getNumViews() const;

    /// @brief  Will return the view at the given index.
    /// @param  i       [in] The index.
    /// @return A pointer showing to the view or a nullptr in case of an error.
    Scene::View *getViewAt(ui32 i) const;

    /// @brief  Will return the active view.
    /// @return A pointer showing to the active view or nullptr, if no view is currently active.
    Scene::View *getActiveView() const;

    void addEntity( Entity *entity );
    Entity *getEntityByName( const String &name ) const;

    /// @brief  Will update the world.
    /// @param  dt      [in] The current delta time-tick.
    void update( Time dt );

    /// @brief  Will draw the world-
    /// @param  rbService   [in] The renderbackend.
    void draw( RenderBackend::RenderBackendService *rbService );

    /// @brief  Will return the active render mode.
    /// @return The active render mode, @see RenderMode .
    RenderMode getRenderMode() const;

private:
    CPPCore::TArray<Scene::Stage*> m_stages;
    CPPCore::THashMap<ui32, Scene::Stage*> m_lookupStates;
    CPPCore::TArray<Scene::View*> m_views;
    CPPCore::THashMap<ui32, Scene::View*> m_lookupViews;
    CPPCore::TArray<Entity *> m_entities;
    Scene::Stage *m_activeStage;
    Scene::View *m_activeView;
    Common::Ids m_ids;
    RenderMode m_renderMode;
};

} // Namespace App
} // Namespace OSRE

