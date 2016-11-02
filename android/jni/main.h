// Copyright eeGeo Ltd (2012-2014), All Rights Reserved

#ifndef MAIN_H_
#define MAIN_H_

#include <jni.h>

extern "C"
{
	JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* pvt);

	JNIEXPORT long JNICALL Java_com_eegeo_mobilesdkharness_NativeJniCalls_createNativeCode(JNIEnv* jenv, jobject obj, jobject activity, jobject assetManager, jfloat dpi);
	JNIEXPORT void JNICALL Java_com_eegeo_mobilesdkharness_NativeJniCalls_destroyNativeCode(JNIEnv* jenv, jobject obj);
	JNIEXPORT void JNICALL Java_com_eegeo_mobilesdkharness_NativeJniCalls_pauseNativeCode(JNIEnv* jenv, jobject obj);
	JNIEXPORT void JNICALL Java_com_eegeo_mobilesdkharness_NativeJniCalls_resumeNativeCode(JNIEnv* jenv, jobject obj);
	JNIEXPORT void JNICALL Java_com_eegeo_mobilesdkharness_NativeJniCalls_setNativeSurface(JNIEnv* jenv, jobject obj, jobject surface);
	JNIEXPORT void JNICALL Java_com_eegeo_mobilesdkharness_NativeJniCalls_updateNativeCode(JNIEnv* jenv, jobject obj, jfloat deltaSeconds, jfloatArray headTransform);
	JNIEXPORT void JNICALL Java_com_eegeo_mobilesdkharness_NativeJniCalls_updateCardboardProfile(JNIEnv* jenv, jobject obj, jfloatArray headTransform);
	JNIEXPORT void JNICALL Java_com_eegeo_mobilesdkharness_NativeJniCalls_magnetTriggered(JNIEnv* jenv, jobject obj);
	JNIEXPORT int JNICALL Java_com_eegeo_mobilesdkharness_NativeJniCalls_initTracker(JNIEnv* jenv, jobject obj);
	JNIEXPORT int JNICALL Java_com_eegeo_mobilesdkharness_NativeJniCalls_loadTrackerData(JNIEnv* jenv, jobject obj);
	JNIEXPORT void JNICALL Java_com_eegeo_mobilesdkharness_NativeJniCalls_onVuforiaInitializedNative(JNIEnv* jenv, jobject obj);
	JNIEXPORT void JNICALL Java_com_eegeo_mobilesdkharness_NativeJniCalls_updateVuforiaRendering(JNIEnv* jenv, jobject obj, jint width, jint height);

	//input
	JNIEXPORT void JNICALL Java_com_eegeo_mobilesdkharness_EegeoSurfaceView_processNativePointerDown(JNIEnv* jenv, jobject obj,
	        jint primaryActionIndex,
	        jint primaryActionIdentifier,
	        jint numPointers,
	        jfloatArray x,
	        jfloatArray y,
	        jintArray pointerIdentity,
	        jintArray pointerIndex);

	JNIEXPORT void JNICALL Java_com_eegeo_mobilesdkharness_EegeoSurfaceView_processNativePointerUp(JNIEnv* jenv, jobject obj,
	        jint primaryActionIndex,
	        jint primaryActionIdentifier,
	        jint numPointers,
	        jfloatArray x,
	        jfloatArray y,
	        jintArray pointerIdentity,
	        jintArray pointerIndex);

	JNIEXPORT void JNICALL Java_com_eegeo_mobilesdkharness_EegeoSurfaceView_processNativePointerMove(JNIEnv* jenv, jobject obj,
	        jint primaryActionIndex,
	        jint primaryActionIdentifier,
	        jint numPointers,
	        jfloatArray x,
	        jfloatArray y,
	        jintArray pointerIdentity,
	        jintArray pointerIndex);
};

#endif /* MAIN_H_ */
