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
#include <src/Engine/Platform/PlatformPluginFactory.h>
#include <osre/Common/Logger.h>
#include <osre/Debugging/osre_debugging.h>
#ifdef OSRE_WINDOWS
#   include <src/Engine/Platform/win32/Win32Window.h>
#   include <src/Engine/Platform/win32/Win32EventQueue.h>
#   include <src/Engine//Platform/win32/Win32Timer.h>
#   include <src/Engine//Platform/win32/Win32OGLRenderContext.h>
#   include <src/Engine/Platform/win32/Win32ThreadFactory.h>
#   include <src/Engine/Platform/win32/Win32DbgLogStream.h>
#   include <src/Engine/Platform/win32/Win32DynamicLoader.h>
#   include "Engine/Platform/win32/Win32SystemInfo.h"
#endif
#include <src/Engine/Platform/sdl2/SDL2Window.h>
#include <src/Engine/Platform/sdl2/SDL2EventQueue.h>
#include <src/Engine/Platform/sdl2/SDL2OGLRenderContext.h>
#include <src/Engine/Platform/sdl2/SDL2Timer.h>
#include <src/Engine/Platform/sdl2/SDL2ThreadFactory.h>
#include <src/Engine/Platform/sdl2/SDL2Initializer.h>
#include <src/Engine/Platform/sdl2/SDL2DynamicLoader.h>
#include <src/Engine/Platform/sdl2/SDL2SystemInfo.h>

namespace OSRE {
namespace Platform {

static const c8 *Tag = "PlatformPluginFactory";

bool PlatformPluginFactory::init( PluginType type ) {
    static_cast< void >( createThreadFactory( type ) );
    if ( type == PluginType::SDL2Plugin ) {
        return SDL2Initializer::init();
    }

    return true;
}

bool PlatformPluginFactory::release( PluginType type ) {
    if( type == PluginType::SDL2Plugin ) {
        return SDL2Initializer::release();
    }
    return true;
}

AbstractPlatformEventQueue *PlatformPluginFactory::createPlatformEventHandler( PluginType type, AbstractWindow *rootSurface ) {
    AbstractPlatformEventQueue *eventHandler( nullptr );
    switch( type ) {
#ifdef OSRE_WINDOWS
        case Platform::PluginType::WindowsPlugin: {
                Win32Window *win32Surface = static_cast<Win32Window*>( rootSurface );
                if( win32Surface ) {
                    eventHandler = new Win32EventQueue( win32Surface );
                    Win32EventQueue::registerEventQueue( ( Win32EventQueue* ) eventHandler, win32Surface->getHWnd() );
                }
            }
            break;
#endif // OSRE_WINDOWS

        case Platform::PluginType::SDL2Plugin:
            eventHandler = new SDL2EventHandler( rootSurface );
            break;

        default:
            break;
    }

    OSRE_ASSERT(nullptr != eventHandler);

    return eventHandler;
}

AbstractWindow *PlatformPluginFactory::createSurface( PluginType type, WindowsProperties *pProps ) {
    AbstractWindow *surface( nullptr );
    switch( type ) {
#ifdef OSRE_WINDOWS
        case Platform::PluginType::WindowsPlugin:
            surface = new Win32Window( pProps );
            break;
#endif // OSRE_WINDOWS

        case Platform::PluginType::SDL2Plugin:
            surface = new SDL2Surface( pProps );
            break;

        default:
            osre_info( Tag, "Enum value not handled." );
            break;
    }
    OSRE_ASSERT( nullptr != surface);

    return surface;
}

AbstractOGLRenderContext *PlatformPluginFactory::createRenderContext( PluginType type ) {
    AbstractOGLRenderContext *renderCtx( nullptr );
    switch( type ) {
#ifdef OSRE_WINDOWS
        case Platform::PluginType::WindowsPlugin:
            renderCtx = new Win32RenderContext();
            break;
#endif // OSRE_WINDOWS

        case Platform::PluginType::SDL2Plugin:
            renderCtx = new SDL2RenderContext();
            break;

        default:
            osre_error( Tag, "Enum value not handled." );
            break;
    }
    OSRE_ASSERT(nullptr != renderCtx);

    return renderCtx;
}

AbstractTimer *PlatformPluginFactory::createTimer( PluginType type ) {
    AbstractTimer *timer( nullptr );
    switch( type ) {
#ifdef OSRE_WINDOWS
        case Platform::PluginType::WindowsPlugin:
            timer = new Win32Timer();
            break;
#endif // OSRE_WINDOWS

        case Platform::PluginType::SDL2Plugin:
            timer = new SDL2Timer();
            break;

        default:
            break;
    }
    return timer;
}

AbstractThreadFactory *PlatformPluginFactory::createThreadFactory( PluginType type ) {
    AbstractThreadFactory *instance( nullptr );
    switch( type ) {
#ifdef OSRE_WINDOWS
        case Platform::PluginType::WindowsPlugin:
            instance = new Win32ThreadFactory();
            break;
#endif // OSRE_WINDOWS

        case Platform::PluginType::SDL2Plugin:
            instance = new SDL2ThreadFactory();
            break;

        default:
            break;
    }

    osre_info( Tag, "Set thread factory." );
    AbstractThreadFactory::setInstance( instance );

    return instance;
}

Common::AbstractLogStream *PlatformPluginFactory::createPlatformLogStream() {
    Common::AbstractLogStream *stream( nullptr );
#ifdef OSRE_WINDOWS
    stream = new Win32DbgLogStream;
#endif // OSRE_WINDOWS

    return stream;
}

AbstractDynamicLoader *PlatformPluginFactory::createDynmicLoader( PluginType type ) {
    AbstractDynamicLoader *dynloader( nullptr );
    switch ( type ) {
#ifdef OSRE_WINDOWS
        case Platform::PluginType::WindowsPlugin:
            dynloader = new Win32DynamicLoader;
            break;
#endif // OSRE_WINDOWS

        case Platform::PluginType::SDL2Plugin:
            dynloader = new SDL2DynamicLoader();
            break;

    default:
        break;
    }

    return dynloader;
}

AbstractSystemInfo *PlatformPluginFactory::createSystemInfo( PluginType type ) {
    AbstractSystemInfo *sysInfo( nullptr );
    switch ( type ) {
#ifdef OSRE_WINDOWS
    case Platform::PluginType::WindowsPlugin:
        sysInfo = new Win32SystemInfo;
        break;
#endif // OSRE_WINDOWS

    case Platform::PluginType::SDL2Plugin:
        sysInfo = new SDL2SystemInfo();
        break;

    default:
        break;
    }

    return sysInfo;
}

} // Namespace Platform
} // Namespace OSRE
