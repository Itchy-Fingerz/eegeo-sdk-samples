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
#include "CameraFrustumStreamingVolume.h"
#include "IStreamingVolume.h"
#include "ARCameraController.h"
#include "SampleMath.h"
#include "AppInterface.h"

namespace Eegeo
{
    namespace AR
    {
        class ARVuforiaController
        {
        private:
            
            
            Eegeo::dv3 m_targetPosition;
            Eegeo::dv3 m_objectPosition;
            bool m_positionObtained;
            
            bool m_startedPrecaching;
            bool m_precacheComplete;
            bool m_isTracking;
            
            Eegeo::EegeoWorld& m_world;
            
            
            unsigned int shaderProgramID    = 0;
            GLint vertexHandle              = 0;
            GLint textureCoordHandle        = 0;
            GLint mvpMatrixHandle           = 0;
            GLint texSampler2DHandle        = 0;
            
            int screenWidth                 = 0;
            int screenHeight                = 0;
            
            Vuforia::Matrix44F projectionMatrix;
            
            GLfloat viewport[4];
            
            Vuforia::DataSet* dataSetStonesAndChips  = 0;
            Vuforia::DataSet* dataSetTarmac          = 0;
            
            bool switchDataSetAsap           = false;
            bool isExtendedTrackingActivated = false;
            
            Vuforia::CameraDevice::CAMERA_DIRECTION currentCamera;
            
            const int STONES_AND_CHIPS_DATASET_ID = 0;
            const int TARMAC_DATASET_ID = 1;
            int selectedDataset = STONES_AND_CHIPS_DATASET_ID;
            Eegeo::AR::ARCameraController& m_arCameraController;
            
            Eegeo::dv3 m_interstPoint;
            Eegeo::dv3 m_cachedInterstPoint;
            
            Eegeo::Web::PrecacheService& m_precacheService;
            double m_scale;
            double m_rotationHeading;
            double m_cachedRotationData;
        public:
            
            ARVuforiaController(int width,
                                int height,
                                Eegeo::Modules::Map::MapModule& mapModule,
                                Eegeo::Web::PrecacheService& precacheService,
                                Eegeo::EegeoWorld& world,
                                Eegeo::AR::ARCameraController& arCameraController);
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
            
            Eegeo::m33 GetLookAtOrientationMatrix(const Eegeo::v3& targetPosition, const Eegeo::v3& objectPosition, Eegeo::v3 up);
            
            void Draw (Eegeo::EegeoWorld& eegeoWorld, Eegeo::Camera::CameraState cameraState, Examples::ScreenPropertiesProvider& screenPropertyProvider);
            
            const Eegeo::dv3& GetInterestPoint(){return m_interstPoint;}

            void Event_TouchPinch (const AppInterface::PinchData& data);
            void Event_TouchPan (const AppInterface::PanData& data);
            void Event_TouchPan_Start (const AppInterface::PanData& data);
            void Event_TouchRotate (const AppInterface::RotateData& data);
            void Event_TouchRotate_Start (const AppInterface::RotateData& data);
        };

    }
}


