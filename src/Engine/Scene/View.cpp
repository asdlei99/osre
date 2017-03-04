/*-----------------------------------------------------------------------------------------------
The MIT License (MIT)

Copyright (c) 2015-2017 OSRE ( Open Source Render Engine ) by Kim Kulling

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
#include <osre/Scene/View.h>
#include <osre/Common/Logger.h>
#include <osre/RenderBackend/Parameter.h>
#include <glm/gtc/matrix_transform.hpp>

namespace OSRE {
namespace Scene {

static const String Tag = "View";

View::View( const String &name ) 
: Object( name )
, m_node( nullptr )
, m_pos( 1, 1, 1 )
, m_lookAt( 0, 0, 0 )
, m_up( 0, 0, 1 )
, m_view()
, m_projection() {
    // empty
}

View::~View() {
    // empty
}

void View::observeNode( Node *node ) {
    m_node = node;
}

void View::update( RenderBackend::RenderBackendService *renderBackendSrv ) {
    m_view = glm::lookAt( m_pos, m_lookAt, m_up );
}

void View::setLookAt( const glm::vec3 &pos, const glm::vec3 &view, const glm::vec3 &up ) {
    m_pos    = pos;
    m_lookAt = view;
    m_up     = up;

    m_view = glm::lookAt( pos, view, up );
}

void View::setProjectionMode( f32 fov, f32 aspectRatio, f32 near, f32 far ) {
    m_projection = glm::perspective( fov, aspectRatio, near, far );
}

void View::setOrthoMode( f32 left, f32 right, f32 bottom, f32 top, f32 near, f32 far ) {
    m_projection = glm::ortho( left, right, bottom, top, near, far );
}

const glm::mat4 &View::getView() const {
    return m_view;
}

const glm::mat4 &View::getProjection() const {
    return m_projection;
}

} // Namespace Scene
} // Namespace OSRE

