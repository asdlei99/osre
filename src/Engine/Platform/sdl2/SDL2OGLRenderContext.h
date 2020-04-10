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

#include <osre/Platform/AbstractOGLRenderContext.h>

typedef void *SDL_GLContext;

namespace OSRE {
namespace Platform {

class SDL2Surface;

//-------------------------------------------------------------------------------------------------
///	@class		::OSRE::Platform::SDL2RenderContext
///	@ingroup	Engine
///
///	@brief
//-------------------------------------------------------------------------------------------------
class SDL2RenderContext : public AbstractOGLRenderContext {
public:
    /// The class constructor.
    SDL2RenderContext();
    ///  The class destructor.
    virtual ~SDL2RenderContext();

protected:
    virtual bool onCreate( AbstractWindow *pSurface );
    virtual bool onDestroy();
    virtual bool onUpdate( );
    virtual bool onActivate( );

private:
    SDL_GLContext m_renderContext;
    SDL2Surface *m_surface;
    bool m_isActive;
};

} // Namespace Platform
} // Namespace OSRE
