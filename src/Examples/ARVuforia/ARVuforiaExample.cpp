// Copyright eeGeo Ltd (2012-2014), All Rights Reserved

#include "ARVuforiaExample.h"
#include "VectorMath.h"
#include "LatLongAltitude.h"
#include "CatmullRomSpline.h"
#include "CameraSplinePlaybackController.h"
#include "ResourceCeilingProvider.h"
#include "GlobeCameraController.h"
#include "EegeoWorld.h"
#include "EarthConstants.h"
#include "ScreenProperties.h"

#define INTERIOR_NEAR_MULTIPLIER 0.025f
#define EXTERIOR_NEAR_MULTIPLIER 0.1f


using namespace std;

namespace Examples
{

	ARVuforiaExample::ARVuforiaExample(Eegeo::EegeoWorld& eegeoWorld,
                                           Eegeo::Camera::GlobeCamera::GlobeCameraController* pCameraController,
                                           const IScreenPropertiesProvider& initialScreenProperties,
										   Examples::IARTracker& arTracker)
    : m_world(eegeoWorld)
    , m_pARController(NULL)
	, m_arTracker(arTracker)
    {
        Eegeo::m44 projectionMatrix = Eegeo::m44(pCameraController->GetRenderCamera().GetProjectionMatrix());
        m_pCameraController = new Eegeo::AR::ARCameraController(initialScreenProperties.GetScreenProperties().GetScreenWidth(), initialScreenProperties.GetScreenProperties().GetScreenHeight());
        m_pCameraController->GetCamera().SetProjectionMatrix(projectionMatrix);
        m_pARController = Eegeo_NEW(Eegeo::AR::ARVuforiaController)(initialScreenProperties.GetScreenProperties().GetScreenWidth(), initialScreenProperties.GetScreenProperties().GetScreenHeight(), *m_pCameraController);
        NotifyScreenPropertiesChanged(initialScreenProperties.GetScreenProperties());
    }
    
	ARVuforiaExample::~ARVuforiaExample()
	{
		delete m_pCameraController;
	    Eegeo_DELETE m_pARController;
    }
    
    void ARVuforiaExample::Start()
    {
        m_arTracker.InitVuforia();
        Eegeo::Space::LatLongAltitude eyePosLla = Eegeo::Space::LatLongAltitude::FromDegrees(40.763647, -73.973468, 0);
        m_pCameraController->SetStartLatLongAltitude(eyePosLla);
    }
    
    void ARVuforiaExample::Suspend()
    {
    	m_pARController->StopCamera();
    	m_arTracker.DeInitVuforia();
    }

    void ARVuforiaExample::Draw()
    {

    }

    void ARVuforiaExample::UpdateWorld(float dt, Eegeo::EegeoWorld& world, Eegeo::Camera::CameraState cameraState, Examples::ScreenPropertiesProvider& screenPropertyProvider, Eegeo::Streaming::IStreamingVolume& streamingVolume)
    {
    	m_pARController->Update(dt, GetCurrentCameraState(), m_world, screenPropertyProvider, streamingVolume);
    }

    void ARVuforiaExample::DrawWorld(Eegeo::EegeoWorld& world,  Eegeo::Camera::CameraState cameraState, Examples::ScreenPropertiesProvider& screenPropertyProvider)
    {
    	m_pARController->Draw(m_world, cameraState, screenPropertyProvider);
    }
    
    void ARVuforiaExample::EarlyUpdate(float dt)
    {
        m_pCameraController->Update(dt);
        m_pCameraController->SetNearMultiplier(EXTERIOR_NEAR_MULTIPLIER);
    }
    
    void ARVuforiaExample::NotifyScreenPropertiesChanged(const Eegeo::Rendering::ScreenProperties& screenProperties)
    {
    	//m_pARController->NotifyScreenPropertiesChanged(screenProperties);
    }
    
    Eegeo::Camera::CameraState ARVuforiaExample::GetCurrentCameraState() const
    {
        return m_pCameraController->GetCameraState();
    }

    
    void ARVuforiaExample::SetVRCameraState(const float headTransform[])
    {
        
        Eegeo::m33 orientation;
        Eegeo::v3 right = Eegeo::v3(headTransform[0],headTransform[4],headTransform[8]);
        Eegeo::v3 up = Eegeo::v3(headTransform[1],headTransform[5],headTransform[9]);
        Eegeo::v3 forward = Eegeo::v3(-headTransform[2],-headTransform[6],-headTransform[10]);
        orientation.SetRow(0, right);
        orientation.SetRow(1, up);
        orientation.SetRow(2, forward);
        
//        m_pCameraController->UpdateFromPose(orientation, 0.0f);
        
    }
    
    const Eegeo::m33& ARVuforiaExample::GetCurrentCameraOrientation()
    {
        return m_pCameraController->GetOrientation();
    }
    
    const Eegeo::m33& ARVuforiaExample::GetBaseOrientation()
    {
        return m_pCameraController->GetCameraOrientation();
    }
    
    const Eegeo::m33& ARVuforiaExample::GetHeadTrackerOrientation()
    {
        return m_pCameraController->GetHeadTrackerOrientation();
    }
    
    
	int ARVuforiaExample::InitTracker()
	{
		return m_pARController->InitTracker();
	}

	int ARVuforiaExample::LoadTrackerData()
	{
		return m_pARController->LoadTrackerData();
	}

	void ARVuforiaExample::OnVuforiaInitializedNative()
	{
		m_pARController->OnVuforiaInitializedNative();
	}

	void ARVuforiaExample::InitVuforiaRendering()
	{
		m_pARController->InitRendering();
	}

	void ARVuforiaExample::UpdateVuforiaRendering(int width, int height)
	{
		m_pARController->UpdateRendering(width, height);
	}
}
