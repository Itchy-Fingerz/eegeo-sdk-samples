// Copyright eeGeo Ltd (2012-2016), All Rights Reserved
#include "CameraFrustumStreamingVolume.h"
#include "StreamingVolumeController.h"
#include "ARVuforiaController.h"
#include "SpaceHelpers.h"
#include "EcefTangentBasis.h"
#include "CameraHelpers.h"

namespace Eegeo
{
    namespace AR
    {
    
        ARVuforiaController::ARVuforiaController(int width
                                                 , int height
                                                 , Eegeo::Modules::Map::MapModule& mapModule
                                                 , Eegeo::Web::PrecacheService& precacheService
                                                 , Eegeo::EegeoWorld& world
                                                 , Eegeo::AR::ARCameraController& arCameraController
                                                 )
        : currentCamera(Vuforia::CameraDevice::CAMERA_DIRECTION_BACK)
        , screenWidth(width)
        , screenHeight(height)
        , m_startedPrecaching(false)
        , m_precacheComplete(false)
        , m_positionObtained(false)
        , m_world(world)
        , m_precacheService(precacheService)
        , m_arCameraController(arCameraController)
        , m_scale(1.0f)
        , m_rotationHeading(0.f)
        {
            
        }
        
        ARVuforiaController::~ARVuforiaController()
        {
            
            
            StopCamera();
            
        	DeinitApplicationNative();
        	DestroyTrackerData();
        	DeinitTracker();
        }
        
        int ARVuforiaController::InitTracker()
        {
            Vuforia::TrackerManager& trackerManager = Vuforia::TrackerManager::getInstance();
            Vuforia::Tracker* tracker = trackerManager.initTracker(Vuforia::ObjectTracker::getClassType());
            if (tracker == NULL)
            {
                return 0;
            }
            InitApplicationNative(screenWidth, screenHeight);
            return 1;
        }
        
        void ARVuforiaController::DeinitTracker()
        {
            // Deinit the object tracker:
            Vuforia::TrackerManager& trackerManager = Vuforia::TrackerManager::getInstance();
            trackerManager.deinitTracker(Vuforia::ObjectTracker::getClassType());
        }
        
        int ARVuforiaController::LoadTrackerData()
        {
            // Get the object tracker:
            Vuforia::TrackerManager& trackerManager = Vuforia::TrackerManager::getInstance();
            Vuforia::ObjectTracker* objectTracker = static_cast<Vuforia::ObjectTracker*>(trackerManager.getTracker(Vuforia::ObjectTracker::getClassType()));
            if (objectTracker == NULL)
            {
                return 0;
            }
            
            // Create the data sets:
            dataSetStonesAndChips = objectTracker->createDataSet();
            if (dataSetStonesAndChips == 0)
            {
                return 0;
            }
            
            dataSetTarmac = objectTracker->createDataSet();
            if (dataSetTarmac == 0)
            {
                return 0;
            }
            
            // Load the data sets:
            if (!dataSetStonesAndChips->load("StonesAndChips.xml", Vuforia::STORAGE_APPRESOURCE))
            {
                return 0;
            }
            
            if (!dataSetTarmac->load("Tarmac.xml", Vuforia::STORAGE_APPRESOURCE))
            {
                return 0;
            }
            
            // Activate the data set:
            if (!objectTracker->activateDataSet(dataSetStonesAndChips))
            {
                return 0;
            }
            return 1;
        }
        
        int ARVuforiaController::DestroyTrackerData()
        {
            Vuforia::TrackerManager& trackerManager = Vuforia::TrackerManager::getInstance();
            Vuforia::ObjectTracker* objectTracker = static_cast<Vuforia::ObjectTracker*>(
                                                                                         trackerManager.getTracker(Vuforia::ObjectTracker::getClassType()));
            if (objectTracker == NULL)
            {
                return 0;
            }
            
            if (dataSetStonesAndChips != 0)
            {
                if (objectTracker->getActiveDataSet() == dataSetStonesAndChips &&
                    !objectTracker->deactivateDataSet(dataSetStonesAndChips))
                {
                    return 0;
                }
                
                if (!objectTracker->destroyDataSet(dataSetStonesAndChips))
                {
                    return 0;
                }
                
                dataSetStonesAndChips = 0;
            }
            
            if (dataSetTarmac != 0)
            {
                if (objectTracker->getActiveDataSet() == dataSetTarmac &&
                    !objectTracker->deactivateDataSet(dataSetTarmac))
                {
                    return 0;
                }
                
                if (!objectTracker->destroyDataSet(dataSetTarmac))
                {
                    return 0;
                }
                
                dataSetTarmac = 0;
            }
            
            return 1;
        }
        
        void ARVuforiaController::OnVuforiaInitializedNative()
        {
            StartCamera();
            m_interstPoint = m_arCameraController.GetCameraPosition();
        }
        
        void ARVuforiaController::RenderFrame()
        {
        }
        
        void ARVuforiaController::ConfigureVideoBackground()
        {
            Vuforia::CameraDevice& cameraDevice = Vuforia::CameraDevice::getInstance();
            Vuforia::VideoMode videoMode = cameraDevice.
            getVideoMode(Vuforia::CameraDevice::MODE_DEFAULT);
            
            Vuforia::VideoBackgroundConfig config;
            config.mEnabled = true;
            config.mPosition.data[0] = 0.0f;
            config.mPosition.data[1] = 0.0f;
            
            config.mSize.data[0] = videoMode.mHeight * (screenHeight / (float)videoMode.mWidth);
            config.mSize.data[1] = screenHeight;
            
            if(config.mSize.data[0] < screenWidth)
            {
                config.mSize.data[0] = screenWidth;
                config.mSize.data[1] = screenWidth *
                (videoMode.mWidth / (float)videoMode.mHeight);
            }
            
            Vuforia::Renderer::getInstance().setVideoBackgroundConfig(config);
            
            viewport[0] = ((screenWidth - config.mSize.data[0]) / 2) + config.mPosition.data[0];
            viewport[1] = ((screenHeight - config.mSize.data[1]) / 2) + config.mPosition.data[1];
            viewport[2] = config.mSize.data[0];
            viewport[3] = config.mSize.data[1];
        }
        
        void ARVuforiaController::InitApplicationNative(int width, int height)
        {
            screenWidth = width;
            screenHeight = height;
        }
        
        void ARVuforiaController::DeinitApplicationNative()
        {
        }
        
        void ARVuforiaController::StartCamera()
        {
            if (!Vuforia::CameraDevice::getInstance().init(currentCamera))
                return;
            
            if (!Vuforia::CameraDevice::getInstance().selectVideoMode(Vuforia::CameraDevice::MODE_DEFAULT))
                return;
            
            ConfigureVideoBackground();
            
            if (!Vuforia::CameraDevice::getInstance().start())
                return;
            
            Vuforia::TrackerManager& trackerManager = Vuforia::TrackerManager::getInstance();
            Vuforia::Tracker* objectTracker = trackerManager.getTracker(Vuforia::ObjectTracker::getClassType());
            if(objectTracker != 0)
                objectTracker->start();
            InitRendering();
        }
        
        void ARVuforiaController::StopCamera()
        {
            Vuforia::TrackerManager& trackerManager = Vuforia::TrackerManager::getInstance();
            Vuforia::Tracker* objectTracker = trackerManager.getTracker(Vuforia::ObjectTracker::getClassType());
            if(objectTracker != 0)
                objectTracker->stop();
            
            Vuforia::CameraDevice::getInstance().stop();
            Vuforia::CameraDevice::getInstance().deinit();
        }
        
        void ARVuforiaController::SetProjectionMatrix()
        {
            const Vuforia::CameraCalibration& cameraCalibration =
            Vuforia::CameraDevice::getInstance().getCameraCalibration();
            projectionMatrix = Vuforia::Tool::getProjectionGL(cameraCalibration, 10.0f, 5000.0f);
        }
        
        bool ARVuforiaController::StartExtendedTracking()
        {
            Vuforia::TrackerManager& trackerManager = Vuforia::TrackerManager::getInstance();
            Vuforia::ObjectTracker* objectTracker = static_cast<Vuforia::ObjectTracker*>(trackerManager.getTracker(Vuforia::ObjectTracker::getClassType()));
            
            Vuforia::DataSet* currentDataSet = objectTracker->getActiveDataSet();
            if (objectTracker == 0 || currentDataSet == 0)
                return false;
            
            for (int tIdx = 0; tIdx < currentDataSet->getNumTrackables(); tIdx++)
            {
                Vuforia::Trackable* trackable = currentDataSet->getTrackable(tIdx);
                if(!trackable->startExtendedTracking())
                    return false;
            }
            
            isExtendedTrackingActivated = true;
            return true;
        }
        
        bool ARVuforiaController::StopExtendedTracking()
        {
            Vuforia::TrackerManager& trackerManager = Vuforia::TrackerManager::getInstance();
            Vuforia::ObjectTracker* objectTracker = static_cast<Vuforia::ObjectTracker*>(trackerManager.getTracker(Vuforia::ObjectTracker::getClassType()));
            Vuforia::DataSet* currentDataSet = objectTracker->getActiveDataSet();
            
            if (objectTracker == 0 || currentDataSet == 0)
                return false;
            
            for (int tIdx = 0; tIdx < currentDataSet->getNumTrackables(); tIdx++)
            {
                Vuforia::Trackable* trackable = currentDataSet->getTrackable(tIdx);
                if(!trackable->stopExtendedTracking())
                    return false;
            }
            
            isExtendedTrackingActivated = false;
            return true;
        }
        
        void ARVuforiaController::InitRendering()
        {
            // Define clear color
            glClearColor(0.0f, 0.0f, 0.0f, Vuforia::requiresAlpha() ? 0.0f : 1.0f);
            
        }
        
        void ARVuforiaController::UpdateRendering(int width, int height)
        {
            // Update screen dimensions
            screenWidth = width;
            screenHeight = height;
            
            // Reconfigure the video background
            ConfigureVideoBackground();

            // Setting up the projection matrix
            SetProjectionMatrix();
        }
        

        void ARVuforiaController::Update(float dt, const Eegeo::Camera::CameraState cameraState, Eegeo::EegeoWorld& eegeoWorld, Examples::ScreenPropertiesProvider& screenPropertyProvider, Eegeo::Streaming::IStreamingVolume& streamingVolume)
        {
            
            Vuforia::State state = Vuforia::Renderer::getInstance().begin();
            
            for(int tIdx = 0; tIdx < state.getNumTrackableResults(); tIdx++)
            {
                
                Eegeo::Space::EcefTangentBasis basis;
                Eegeo::Camera::CameraHelpers::EcefTangentBasisFromPointAndHeading(m_interstPoint,
                                                                                  -m_rotationHeading,
                                                                                  basis);
                
                const Vuforia::TrackableResult* trackableResult = state.getTrackableResult(tIdx);
                const Vuforia::Trackable& trackable = trackableResult->getTrackable();
                Vuforia::Matrix34F pose = trackableResult->getPose();
                Vuforia::Matrix44F modelViewMatrix = Vuforia::Tool::convertPose2GLMatrix(pose);
                
                
//                Eegeo::m44 markerMatrix;
//                markerMatrix.SetRow(0, Eegeo::v4(modelViewMatrix.data[0], modelViewMatrix.data[1], modelViewMatrix.data[2], modelViewMatrix.data[3]));
//                markerMatrix.SetRow(1, Eegeo::v4(modelViewMatrix.data[4], modelViewMatrix.data[5], modelViewMatrix.data[6], modelViewMatrix.data[7]));
//                markerMatrix.SetRow(2, Eegeo::v4(modelViewMatrix.data[8], modelViewMatrix.data[9], modelViewMatrix.data[10], modelViewMatrix.data[11]));
//                markerMatrix.SetRow(3, Eegeo::v4(modelViewMatrix.data[12], modelViewMatrix.data[13], modelViewMatrix.data[14], modelViewMatrix.data[15]));

//                Eegeo::m44::Inverse(markerMatrix, markerMatrix);
//                Eegeo::m44::Transpose(markerMatrix, markerMatrix);
                
                Vuforia::Matrix44F inverseMV = SampleMath::Matrix44FInverse(modelViewMatrix);
                Vuforia::Matrix44F invTranspMV = SampleMath::Matrix44FTranspose(inverseMV);
                
//                                Eegeo::m44 markerMatrix;
//                                markerMatrix.SetRow(0, Eegeo::v4(invTranspMV.data[0], invTranspMV.data[1], invTranspMV.data[2], 0));
//                                markerMatrix.SetRow(1, Eegeo::v4(invTranspMV.data[4], invTranspMV.data[5], invTranspMV.data[6], 0));
//                                markerMatrix.SetRow(2, -Eegeo::v4(invTranspMV.data[8], invTranspMV.data[9], invTranspMV.data[10], 0));
//                                markerMatrix.SetRow(3, Eegeo::v4(invTranspMV.data[12], invTranspMV.data[13], invTranspMV.data[14], 1));
                
                
                
//                Eegeo::m33 markerOrientation;
//                markerOrientation.SetRow(0, Eegeo::v3(markerMatrix.GetRow(0).x, markerMatrix.GetRow(0).y, markerMatrix.GetRow(0).z));
//                markerOrientation.SetRow(1, Eegeo::v3(markerMatrix.GetRow(1).x, markerMatrix.GetRow(1).y, markerMatrix.GetRow(1).z));
//                markerOrientation.SetRow(2, -Eegeo::v3(markerMatrix.GetRow(2).x, markerMatrix.GetRow(2).y, markerMatrix.GetRow(2).z));
//                markerOrientation.SetRow(0, Eegeo::v3(invTranspMV.data[0], invTranspMV.data[1], invTranspMV.data[2]));
//                markerOrientation.SetRow(1, Eegeo::v3(invTranspMV.data[4], invTranspMV.data[5], invTranspMV.data[6]));
//                markerOrientation.SetRow(2, Eegeo::v3(invTranspMV.data[8], invTranspMV.data[9], invTranspMV.data[10]));
                
//                Eegeo::m33 boMat;
//                basis.GetBasisOrientationAsMatrix(boMat);
//                
//                Eegeo::m44 basisMatrix;
//                basisMatrix.SetFromBasis(basis.GetRight(), basis.GetUp(), basis.GetForward(), basis.GetPointEcef().ToSingle());
//                
//                Eegeo::m44 resultMatrix;
//                Eegeo::m44::Mul(resultMatrix, basisMatrix, markerMatrix);
//                
//                Eegeo::m33 orientationMatrix;
//                orientationMatrix.SetRow(0, Eegeo::v3(resultMatrix.GetRow(0).x, resultMatrix.GetRow(0).y, resultMatrix.GetRow(0).z));
//                orientationMatrix.SetRow(1, Eegeo::v3(resultMatrix.GetRow(1).x, resultMatrix.GetRow(1).y, resultMatrix.GetRow(1).z));
//                orientationMatrix.SetRow(2, Eegeo::v3(resultMatrix.GetRow(2).x, resultMatrix.GetRow(2).y, resultMatrix.GetRow(2).z));
//
//                Eegeo::dv3 position(resultMatrix.GetRow(3).x, resultMatrix.GetRow(3).y, resultMatrix.GetRow(3).z);
                
                
                
//                m_arCameraController.UpdateFromPose(orientationMatrix, 0.f);
//                m_arCameraController.SetEcefPosition(position);
                m_targetPosition = m_interstPoint;
                m_objectPosition = m_interstPoint + (((invTranspMV.data[12]*basis.GetRight())+(invTranspMV.data[13]*basis.GetForward())+(invTranspMV.data[14]*basis.GetUp())) * m_scale);
               
                
               
//                Eegeo::v3 oD = orientationMatrix.GetRow(2).Norm()*10.;
//                m_objectPosition = position;
//                m_targetPosition = position + Eegeo::dv3(oD.x, oD.y, oD.z);
                
                m_positionObtained = true;
                
                m_arCameraController.SetEcefPosition(m_objectPosition);
                m_arCameraController.UpdateFromPose(GetLookAtOrientationMatrix(m_targetPosition.ToSingle(), m_objectPosition.ToSingle(), basis.GetUp()), 0.f);

                Eegeo::EegeoUpdateParameters updateParameters(dt,
                                                              m_objectPosition,
                                                              m_targetPosition,
                                                              cameraState.ViewMatrix(),
                                                              cameraState.ProjectionMatrix(),
                                                              streamingVolume,
                                                              screenPropertyProvider.GetScreenProperties());
                eegeoWorld.Update(updateParameters);
                
            }
        }
        
        Eegeo::m33 ARVuforiaController::GetLookAtOrientationMatrix(const Eegeo::v3& targetPosition, const Eegeo::v3& objectPosition, Eegeo::v3 up){
            Eegeo::v3 delta = targetPosition-objectPosition;
            Eegeo::v3 direction(delta.Norm());
            Eegeo::v3 right = (Eegeo::v3::Cross(up,direction)).Norm();
            up = (Eegeo::v3::Cross(direction, right)).Norm();
            Eegeo::m33 orientation;
            orientation.SetFromBasis(right, up, direction);
            return orientation;
        }
        
        void ARVuforiaController::Draw(Eegeo::EegeoWorld& eegeoWorld, Eegeo::Camera::CameraState cameraState, Examples::ScreenPropertiesProvider& screenPropertyProvider)
        {
            
        	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            
            Vuforia::Renderer::getInstance().drawVideoBackground();
            
            if(m_positionObtained)
            {
                Eegeo::EegeoDrawParameters drawParameters(m_objectPosition,
                                                          m_targetPosition,
                                                          cameraState.ViewMatrix(),
                                                          cameraState.ProjectionMatrix(),
                                                          screenPropertyProvider.GetScreenProperties());
                
                eegeoWorld.Draw(drawParameters);
            }
            m_positionObtained = false;

        	Vuforia::Renderer::getInstance().end();
        }

        void ARVuforiaController::Event_TouchPinch (const AppInterface::PinchData& data)
        {
            if(Eegeo::Math::Abs(data.scale) < 0.1f )
            {
                m_scale += data.scale * 3;
            }
            if(m_scale < 0.7f)
            {
                m_scale = 0.7f;
            }
            if(m_scale > 4.f)
            {
                m_scale = 4.f;
            }
        }
        
        void ARVuforiaController::Event_TouchPan (const AppInterface::PanData& data)
        {
            float cameraHeading = Eegeo::Camera::CameraHelpers::GetAbsoluteBearingRadians(m_interstPoint, m_objectPosition.ToSingle());
            Eegeo::Space::EcefTangentBasis basis;
            Eegeo::Camera::CameraHelpers::EcefTangentBasisFromPointAndHeading(m_interstPoint,
                                                                              Eegeo::Math::Rad2Deg(cameraHeading),
                                                                              basis);
            float d = (m_targetPosition - m_objectPosition).Length();
            float mpp = (Eegeo::Math::Tan(0.35)*d) / data.majorScreenDimension;
            Eegeo::v3 p = (basis.GetForward() * data.pointRelative.GetY() * -1.f) + (basis.GetRight() * data.pointRelative.GetX());
            p = p*mpp;
            m_interstPoint = m_cachedInterstPoint + Eegeo::dv3(p.x, p.y, p.z) ;
        }
        
        void ARVuforiaController::Event_TouchPan_Start (const AppInterface::PanData& data)
        {
            m_cachedInterstPoint = m_interstPoint;
        }
        
        void ARVuforiaController::Event_TouchRotate (const AppInterface::RotateData& data)
        {
            m_rotationHeading = m_cachedRotationData + Eegeo::Math::Rad2Deg(data.rotation);
        }
        
        void ARVuforiaController::Event_TouchRotate_Start (const AppInterface::RotateData& data)
        {
            m_cachedRotationData = m_rotationHeading;
        }
    }
}
