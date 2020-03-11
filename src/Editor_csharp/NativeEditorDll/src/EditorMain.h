/*-----------------------------------------------------------------------------------------------
The MIT License (MIT)

Copyright (c) 2015-2019 OSRE ( Open Source Render Engine ) by Kim Kulling

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

#include <osre/Common/osre_common.h>

using namespace ::OSRE;

// Define compiler-specific export macro
#ifdef OSRE_WINDOWS
#  define TAG_DLL_EXPORT __declspec(dllexport)
#  define TAG_DLL_IMPORT __declspec(dllimport )
#endif 

#define OSRE_EDITOR_BUILD_EXPORT

#ifdef OSRE_WINDOWS
#   ifdef OSRE_EDITOR_BUILD_EXPORT
#       define OSRE_EDITOR_EXPORT TAG_DLL_EXPORT
#   else
#       define OSRE_EDITOR_EXPORT TAG_DLL_IMPORT
#   endif
#else
#   define OSRE_EDITOR_EXPORT  __attribute__ ((visibility("default")))
#endif

// Define the calling convention macro for each supported platform
#ifdef OSRE_WINDOWS
#   define STDCALL __stdcall
#else
#   define STDCALL
#endif

static const i32 OsreError = -1;
static const i32 OsreInvalidParameterError = -2;
static const i32 OsreInvalidNodeError = -3;

typedef void(STDCALL *fnc_log_callback) (int, const char*);
/*
struct CSharpEvent {
    int type;
    int x;
    int y;
    int mouseBtnState;
};
*/
struct NativeOSREWorld {
    int NumStages;
};

struct NativeOSREStage {

};

struct NativeOSRENode {
    int mNumChildren;
};

// Exported API for C# --------------------------------------------------------
extern "C" OSRE_EDITOR_EXPORT int  STDCALL CreateEditorApp(int* mainWindowHandle );
extern "C" OSRE_EDITOR_EXPORT int  STDCALL EditorUpdate();
extern "C" OSRE_EDITOR_EXPORT int  STDCALL EditorRequestNextFrame();
extern "C" OSRE_EDITOR_EXPORT int  STDCALL DestroyEditorApp();
extern "C" OSRE_EDITOR_EXPORT int  STDCALL EditorResize(i32 x, i32 y, i32 w, i32 h);
extern "C" OSRE_EDITOR_EXPORT int  STDCALL NewProject(const char *name);
extern "C" OSRE_EDITOR_EXPORT int  STDCALL LoadProject(const char *filelocation, int flags);
extern "C" OSRE_EDITOR_EXPORT int  STDCALL SaveProject(const char *filelocation, int flags );
extern "C" OSRE_EDITOR_EXPORT int  STDCALL ImportAsset(const char *filename, int flags);
extern "C" OSRE_EDITOR_EXPORT int  STDCALL OpenWorldAccess( const char *name );
extern "C" OSRE_EDITOR_EXPORT int  STDCALL OpenStageAccess( const char *name );
extern "C" OSRE_EDITOR_EXPORT int  STDCALL OpenNodeAccess( const char *name );
extern "C" OSRE_EDITOR_EXPORT int  STDCALL CreateNode( const char *name, const char *parent );
extern "C" OSRE_EDITOR_EXPORT int  STDCALL CloseNodeAccess( void );
extern "C" OSRE_EDITOR_EXPORT int  STDCALL CloseStageAccess( void );
extern "C" OSRE_EDITOR_EXPORT int  STDCALL CloseWorldAccess( void );
extern "C" OSRE_EDITOR_EXPORT void STDCALL RegisterLogCallback(fnc_log_callback* fnc);
extern "C" OSRE_EDITOR_EXPORT int  STDCALL LeftMousePressed(int x, int y, bool pressed);
extern "C" OSRE_EDITOR_EXPORT int  STDCALL MiddleMousePressed(int x, int y, bool pressed);
extern "C" OSRE_EDITOR_EXPORT int  STDCALL RightMousePressed(int x, int y, bool pressed);
