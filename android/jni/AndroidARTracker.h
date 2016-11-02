// Copyright eeGeo Ltd (2012-2014), All Rights Reserved
#pragma once

#include <jni.h>
#include "AndroidNativeState.h"
#include "IARTracker.h"

namespace Examples
{
    class AndroidARTracker : public IARTracker
    {
        const AndroidNativeState& m_nativeState;
        
    public:
        
        AndroidARTracker(const AndroidNativeState& nativeState);
        virtual void InitVuforia();
        virtual void DeInitVuforia();
    };
}
