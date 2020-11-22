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

#include <osre/Common/osre_common.h>
#include <osre/Common/TObjPtr.h>
#include <osre/Platform/PlatformInterface.h>

namespace OSRE {
namespace Scene {

class Node;

//-------------------------------------------------------------------------------------------------
///	@ingroup	Engine
///
///	@brief	this class implements a simple trackball. You can use it to rotate a model.
//-------------------------------------------------------------------------------------------------
class OSRE_EXPORT TrackBall : public Platform::OSEventListener {
public:
    ///	@brief	The class constructor.
    ///	@param	objName		[in] The name for the instance.
    ///	@param	w			[in] The width of the view-port to navigate in
    ///	@param	h			[in] The height of the view-port to navigate in
    TrackBall( const String &objName, ui32 w, ui32 h );

    ///	@brief	The class destructor.
    ~TrackBall();

    ///	@brief	The event callback.
    ///	@param	osEvent		[in] The incoming event from the operation system.
    ///	@param	data		[in] The event data.
    void onOSEvent( const Common::Event &osEvent, const Common::EventData *data );
    
    void rotateTo(const Vec2f &from, Vec2f &to);

    ///	@brief	Maps a 2D-point to a sphere and returns the 3D-coordinate.
    ///	@param	pNewPt		[in] The 2D-point to map.
    ///	@param	NewVec		[out] The mapped 3D-point.
    void mapToSphere( const Vec2f *pNewPt, Vec3f *NewVec );
    
    ///	@brief	The current rotation will be calculated.
    void computeRotation();

    ///	@brief	Calculates the current scaling.
    ///	@param	y			[in] The current y value for the scaling.
    void computeScaling( ui32 y );
    const glm::quat &getRotation() const;
    const Vec3f &getScale() const;

private:
    Vec3f mStartVector, mEndVector;
    TRectangle<ui32> m_Dimension;
    glm::quat m_rotation;
    Vec3f mScale;
    bool m_bLeftMButtonClicked;
    bool m_bMiddleClicked;
    bool m_bRightMButtonClicked;
    bool m_bStartPosInited;
    f32 m_adjWidth;
    f32 m_adjHeight;
    ui32 m_screenY;
    ui32 m_screenYOld;
    f32 mRadius;
};

inline const glm::quat &TrackBall::getRotation() const {
    return m_rotation;
}

inline const Vec3f &TrackBall::getScale() const {
    return mScale;
}

} // Namespace Scene
} // namespace OSRE
