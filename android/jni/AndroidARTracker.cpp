// Copyright eeGeo Ltd (2012-2014), All Rights Reserved

#include <jni.h>
#include "AndroidARTracker.h"

namespace Examples
{
    
    AndroidARTracker::AndroidARTracker(const AndroidNativeState& nativeState)
    :m_nativeState(nativeState)
    {
    }
    
    void AndroidARTracker::InitVuforia()
    {

		AndroidSafeNativeThreadAttachment attached(m_nativeState);
		JNIEnv* pEnv = attached.envForThread;

		jmethodID enterVRModeMethod = pEnv->GetMethodID(m_nativeState.activityClass, "initVuforia", "()V");
		pEnv->CallVoidMethod(m_nativeState.activity, enterVRModeMethod);

    }

    void AndroidARTracker::DeInitVuforia()
        {

    		AndroidSafeNativeThreadAttachment attached(m_nativeState);
    		JNIEnv* pEnv = attached.envForThread;

    		jmethodID enterVRModeMethod = pEnv->GetMethodID(m_nativeState.activityClass, "deInitVuforia", "()V");
    		pEnv->CallVoidMethod(m_nativeState.activity, enterVRModeMethod);

        }

}


