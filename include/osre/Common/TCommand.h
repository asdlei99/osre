/*-----------------------------------------------------------------------------------------------
The MIT License (MIT)

Copyright (c) 2015-2018 OSRE ( Open Source Render Engine ) by Kim Kulling

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

#include <osre/Common/TFunctor.h>

namespace OSRE {
namespace App {

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
template<class TParam, class TFunc>
class TCommand {
public:
    TCommand();
    explicit TCommand( TParam param, TFunc func );
    ~TCommand();
    bool execute( TParam param );

private:
    TParam m_param;
    TFunc m_funcBinding;
};

template<class TParam, class TFunc>
inline
TCommand<TParam, TFunc>::TCommand()
: m_param()
, m_funcBinding() {
    // empty
}

template<class TParam, class TFunc>
inline
TCommand<TParam, TFunc>::TCommand(TParam param, TFunc func)
: m_param( param )
, m_funcBinding( func ) {
    // empty
}

template<class TParam, class TFunc>
inline
TCommand<TParam, TFunc>::~TCommand() {
    // empty
}

template<class TParam, class TFunc>
inline
bool TCommand<TParam, TFunc>::execute( TParam param ) {
    if (nullptr == m_funcBinding) {
        return false;
    }
    m_funcBinding(m_param);

    return true;
}

} // Namespace Common
} // Namespace OSRE
