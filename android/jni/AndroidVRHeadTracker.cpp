// Copyright eeGeo Ltd (2012-2014), All Rights Reserved

#include <jni.h>
#include "AndroidVRHeadTracker.h"
#include "Logger.h"

namespace Examples
{
    
    AndroidVRHeadTracker::AndroidVRHeadTracker(AndroidNativeState& nativeState)
    :m_nativeState(nativeState){
    }
    
    void AndroidVRHeadTracker::ResetTracker()
    {
        
        AndroidSafeNativeThreadAttachment attached(m_nativeState);
        JNIEnv* env = attached.envForThread;
        
        jmethodID resetTrackerMethod = env->GetMethodID(m_nativeState.activityClass, "resetTracker", "()V");
        env->CallVoidMethod(m_nativeState.activity, resetTrackerMethod);
        
    }

    void AndroidVRHeadTracker::EnterVRMode()
        {

            AndroidSafeNativeThreadAttachment attached(m_nativeState);
            JNIEnv* env = attached.envForThread;

            jmethodID resetTrackerMethod = env->GetMethodID(m_nativeState.activityClass, "enterVRMode", "()V");
            env->CallVoidMethod(m_nativeState.activity, resetTrackerMethod);

        }

    void AndroidVRHeadTracker::ExitVRMode()
        {

            AndroidSafeNativeThreadAttachment attached(m_nativeState);
            JNIEnv* env = attached.envForThread;

            jmethodID resetTrackerMethod = env->GetMethodID(m_nativeState.activityClass, "exitVRMode", "()V");
            env->CallVoidMethod(m_nativeState.activity, resetTrackerMethod);

        }


}


