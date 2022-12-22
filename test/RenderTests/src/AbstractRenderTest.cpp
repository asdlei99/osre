/*-----------------------------------------------------------------------------------------------
The MIT License (MIT)

Copyright (c) 2015-2021 OSRE ( Open Source Render Engine ) by Kim Kulling

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
#include "AbstractRenderTest.h"

#include <osre/Common/Logger.h>
#include <osre/RenderBackend/Material.h>
#include <osre/Debugging/osre_debugging.h>
#include <osre/IO/IOService.h>
#include <osre/Platform/AbstractWindow.h>
#include <osre/Platform/PlatformInterface.h>
#include <osre/RenderBackend/RenderCommon.h>

namespace OSRE {
namespace RenderTest {

using namespace ::OSRE::RenderBackend;

static const c8 *Tag = "AbstractRenderTest";

AbstractRenderTest::AbstractRenderTest(const String &renderTestName) :
        m_renderTestName(renderTestName),
        m_window(nullptr) {
    // empty
}

bool AbstractRenderTest::create(RenderBackendService *rbSrv) {
    osre_assert(nullptr != rbSrv);

    osre_info(m_renderTestName, "=> Creating test.");
    const String &name(getTestName());
    m_window = Platform::PlatformInterface::getInstance()->getRootWindow();
    if (nullptr != m_window) {
        m_window->setWindowsTitle("Performing test " + name);
    }

    return onCreate(rbSrv);
}

bool AbstractRenderTest::destroy(RenderBackendService *rbSrv) {
    osre_assert(nullptr != rbSrv);

    osre_info(m_renderTestName, "<= Destroying test.");

    return onDestroy(rbSrv);
}

bool AbstractRenderTest::render(RenderBackendService *rbSrv) {
    osre_assert(nullptr != rbSrv);

    return onRender(rbSrv);
}

bool AbstractRenderTest::onCreate(RenderBackendService *) {
    return true;
}

bool AbstractRenderTest::onDestroy(RenderBackendService *) {
    return true;
}

bool AbstractRenderTest::onRender(RenderBackendService *) {
    return true;
}

void AbstractRenderTest::setup(RenderBackendService *) {
    // empty
}

void AbstractRenderTest::teardown(RenderBackendService *) {
    // empty
}

const String &AbstractRenderTest::getTestName() const {
    return m_renderTestName;
}

Material *AbstractRenderTest::createMaterial(const String &matName, const String &VsSrc, const String &FsSrc) {
    if (matName.empty()) {
        return nullptr;
    }

    Material *mat = new Material(matName, IO::Uri());
    mat->m_numTextures = 0;
    mat->m_type = MaterialType::ShaderMaterial;
    ShaderSourceArray shArray;
    shArray[static_cast<ui32>(ShaderType::SH_VertexShaderType)] = VsSrc;
    shArray[static_cast<ui32>(ShaderType::SH_FragmentShaderType)] = FsSrc;
    mat->createShader(shArray);

    return mat;
}

Platform::AbstractWindow *AbstractRenderTest::getWindow() const {
    return m_window;
}

} // Namespace RenderTest
} // Namespace OSRE
