// Copyright eeGeo Ltd (2012-2016), All Rights Reserved

#pragma once

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "Logger.h"

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#include <Vuforia/Vuforia.h>
#include <Vuforia/CameraDevice.h>
#include <Vuforia/Renderer.h>
#include <Vuforia/VideoBackgroundConfig.h>
#include <Vuforia/Trackable.h>
#include <Vuforia/TrackableResult.h>
#include <Vuforia/Tool.h>
#include <Vuforia/Tracker.h>
#include <Vuforia/TrackerManager.h>
#include <Vuforia/ObjectTracker.h>
#include <Vuforia/CameraCalibration.h>
#include <Vuforia/UpdateCallback.h>
#include <Vuforia/DataSet.h>
#include "SampleUtils.h"
#include "CubeShaders.h"
#include "CameraState.h"
#include "EegeoWorld.h"
#include "ScreenPropertiesProvider.h"
#include "IStreamingVolume.h"

namespace Eegeo
{
    namespace AR
    {
        class ARVuforiaController
        {
        private:
            // Textures:
            //int textureCount                = 0;
            //Texture** textures              = 0;
            
            unsigned int shaderProgramID    = 0;
            GLint vertexHandle              = 0;
            GLint textureCoordHandle        = 0;
            GLint mvpMatrixHandle           = 0;
            GLint texSampler2DHandle        = 0;

            // Screen dimensions:
            int screenWidth                 = 0;
            int screenHeight                = 0;
            
            // The projection matrix used for rendering virtual objects:
            Vuforia::Matrix44F projectionMatrix;
            
            // The viewport used for rendering virtual objects
            GLfloat viewport[4];
            
            // Constants:
            //static const float kObjectScale          = 3.f;
            //static const float kBuildingsObjectScale = 12.f;
            
            Vuforia::DataSet* dataSetStonesAndChips  = 0;
            Vuforia::DataSet* dataSetTarmac          = 0;
            
            bool switchDataSetAsap           = false;
            bool isExtendedTrackingActivated = false;
            
            Vuforia::CameraDevice::CAMERA_DIRECTION currentCamera;
            
            const int STONES_AND_CHIPS_DATASET_ID = 0;
            const int TARMAC_DATASET_ID = 1;
            int selectedDataset = STONES_AND_CHIPS_DATASET_ID;
            
        public:
            ARVuforiaController(int width, int height);
            ~ARVuforiaController();
            int InitTracker();
            void DeinitTracker();
            int LoadTrackerData();
            int DestroyTrackerData();
            void OnVuforiaInitializedNative();
            void RenderFrame();
            void ConfigureVideoBackground();
            void InitApplicationNative(int width, int height);
            void DeinitApplicationNative();
            void StartCamera();
            void StopCamera();
            void SetProjectionMatrix();
            bool StartExtendedTracking();
            bool StopExtendedTracking();
            void InitRendering();
            void UpdateRendering(int width, int height);
            void Update(float dt, const Eegeo::Camera::CameraState cameraState, Eegeo::EegeoWorld& eegeoWorld, Examples::ScreenPropertiesProvider& screenPropertyProvider, Eegeo::Streaming::IStreamingVolume& streamingVolume);
            void Draw (Eegeo::EegeoWorld& eegeoWorld, Eegeo::Camera::CameraState cameraState, Examples::ScreenPropertiesProvider& screenPropertyProvider);
        };

    }
}


