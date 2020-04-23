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

#include <osre/Common/Object.h>
#include <osre/IO/Uri.h>

namespace OSRE {
namespace RenderBackend {

struct OGLTexture;
struct Texture;

class OGLRenderBackend;

//-------------------------------------------------------------------------------------------------
///	@ingroup	Engine
///
///	@brief
//-------------------------------------------------------------------------------------------------
class FontBase : public Common::Object {
public:
    FontBase( const String &name );
    virtual ~FontBase();
    virtual void setSize( size_t size );
    virtual size_t getSize() const;
    virtual void setUri( const IO::Uri &uri );
    virtual void setTextureName( const String &name );
    virtual void setTexture(Texture* texture);
    virtual Texture* getTexture() const;
    virtual const String &getTextureName() const;
    virtual void setAtlasCols( ui32 numCols );
    virtual void setAtlasRows( ui32 numRows );
    virtual bool loadFromStream( OGLRenderBackend *rb );

private:
    size_t m_size;
    String m_texName;
    ui32 m_numCols, m_numRows;
    Texture *m_texture;
    OGLTexture *m_fontAtlas;
    IO::Uri m_uri;
};

} // Namespace RenderBackend
} // Namespace OSRE
