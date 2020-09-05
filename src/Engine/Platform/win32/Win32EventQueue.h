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

#include <osre/Platform/AbstractPlatformEventQueue.h>
#include <cppcore/Container/TArray.h>

#include <osre/Platform/Windows/MinWindows.h>
#include <map>

namespace OSRE {

// Forward declarations
namespace Common {
    struct Event;
    struct EventData;
    class EventTriggerer;
}

namespace Platform {

class AbstractWindow;
class OSEventListener;

struct AbstractInputUpdate;

//-------------------------------------------------------------------------------------------------
///	@ingroup	Engine
///
///	@brief  This class implements the win32-specific event handler for OS-events.
//-------------------------------------------------------------------------------------------------
class Win32EventQueue : public AbstractPlatformEventQueue {
public:
    Win32EventQueue( AbstractWindow *rootWindow );
    virtual ~Win32EventQueue() override;
    virtual bool update() override;
    static LRESULT CALLBACK winproc( HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam );
    void setRootSurface( AbstractWindow *window );
    AbstractWindow *getRootSurface() const;
    void enablePolling( bool enabled ) override;
    bool isPolling() const;
    void registerEventListener( const Common::EventPtrArray &events, OSEventListener *listener ) override;
    void unregisterEventListener( const Common::EventPtrArray &events, OSEventListener *listener ) override;
    void unregisterAllEventHandler(const Common::EventPtrArray &events) override;
    void registerMenuCommands(ui32 id, MenuFunctor func) override;
    static void registerEventQueue(Win32EventQueue *server, HWND hWnd);
    static void unregisterEventQueue( Win32EventQueue *server, HWND hWnd );
    static Win32EventQueue *getInstance( HWND hWnd );

protected:
    virtual bool onQuit();

private:
    static std::map<HWND, Win32EventQueue*> s_WindowsServerMap;
    static std::map<ui32, MenuFunctor> s_MenuFunctorMap;
    AbstractInputUpdate *m_updateInstance;
    Common::EventTriggerer *m_eventTriggerer;
    AbstractWindow *m_rootWindow;
    bool m_shutdownRequested;
    bool m_isPolling;
};

} // Namespace Platform
} // Namespace OSRE
