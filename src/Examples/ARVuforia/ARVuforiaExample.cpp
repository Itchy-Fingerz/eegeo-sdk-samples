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

#include "EegeoWorld.h"
#include "PrecacheService.h"
#include "IStreamingVolume.h"
#include "LatLongAltitude.h"
#include "CubeMapCellInfo.h"
#include "MortonKey.h"
#include <string>
#include "Logger.h"

#define INTERIOR_NEAR_MULTIPLIER 0.025f
#define EXTERIOR_NEAR_MULTIPLIER 0.1f


using namespace std;

namespace Examples
{
    
    
    class SphereVolume : public Eegeo::Streaming::IStreamingVolume
    {
        Eegeo::dv3 m_ecefCentre;
        double m_sphereVolumeRadius;
        
    public:
        SphereVolume(double lat, double lon, double radius)
        : m_ecefCentre(Eegeo::Space::LatLong::FromDegrees(lat, lon).ToECEF())
        , m_sphereVolumeRadius(radius)
        {}
        
        bool IntersectsKey(const Eegeo::Streaming::MortonKey& key,
                           bool& canRefineIntersectedKey,
                           double& intersectedNodeDepthSortSignedDistance)
        {
            intersectedNodeDepthSortSignedDistance = 0.0;
            
            
            const Eegeo::Space::CubeMap::CubeMapCellInfo cellInfo(key);
            const Eegeo::dv3 keyEcef = cellInfo.GetFaceCentreECEF();
            
            const double distance = (m_ecefCentre - keyEcef).Length();
            double cellWidth = key.WidthInMetres();
//            const double cellBoundingSphereRadius = std::sqrt((cellWidth*cellWidth) + (cellWidth*cellWidth));
            
            if(distance < cellWidth)
            {
                canRefineIntersectedKey = true;
                return true;
            }
            
            canRefineIntersectedKey = false;
            return false;
        }
        
        void PrintIntersectedKeys() const
        {
        }
    };

    
    
    

    ARVuforiaExample::ARVuforiaExample(Eegeo::EegeoWorld& eegeoWorld,
                                        Eegeo::Web::PrecacheService& precacheService,
                                            Eegeo::Camera::GlobeCamera::GlobeCameraController* pCameraController,
                                            const IScreenPropertiesProvider& initialScreenProperties,
                                            Eegeo::Modules::Map::MapModule& mapModule,
                                            Examples::IARTracker& arTracker)
    : m_world(eegeoWorld)
    , m_pARController(NULL)
    , m_precacheService(precacheService)
	, m_arTracker(arTracker)
    , m_startedPrecaching(false)
    , m_precacheComplete(false)
    
    {
        Eegeo::m44 projectionMatrix = Eegeo::m44(pCameraController->GetRenderCamera().GetProjectionMatrix());
        m_pCameraController = new Eegeo::AR::ARCameraController(initialScreenProperties.GetScreenProperties().GetScreenWidth(), initialScreenProperties.GetScreenProperties().GetScreenHeight());
        m_pCameraController->GetCamera().SetProjectionMatrix(projectionMatrix);
        m_pARController = Eegeo_NEW(Eegeo::AR::ARVuforiaController)(initialScreenProperties.GetScreenProperties().GetScreenWidth(), initialScreenProperties.GetScreenProperties().GetScreenHeight(), mapModule, precacheService, eegeoWorld, *m_pCameraController);
        m_sphereVolume = NULL;
        NotifyScreenPropertiesChanged(initialScreenProperties.GetScreenProperties());
    }
    
	ARVuforiaExample::~ARVuforiaExample()
	{
		delete m_pCameraController;
	    Eegeo_DELETE m_pARController;
        if(m_sphereVolume!=NULL)
        {
            Eegeo_DELETE m_sphereVolume;
        }
    }
    
    void ARVuforiaExample::Start()
    {
        m_arTracker.InitVuforia();
        Eegeo::Space::LatLongAltitude eyePosLla = Eegeo::Space::LatLongAltitude::FromDegrees(40.763647, -73.973468, 0);
        m_pCameraController->SetStartLatLongAltitude(eyePosLla);
    }
    
    void ARVuforiaExample::Update(float dt)
    {
        if(!m_world.Initialising() && !m_startedPrecaching && !m_precacheService.CurrentlyPrecaching())
        {
            double sphereVolumeCentreLatitudeDegrees = 40.763647;
            double sphereVolumeCentreLongitudeDegrees = -73.973468;
            double sphereVolumeRadiusMetres = 1.0;
            
            if(m_sphereVolume==NULL)
            {
                m_sphereVolume = Eegeo_NEW(SphereVolume)(sphereVolumeCentreLatitudeDegrees, sphereVolumeCentreLongitudeDegrees, sphereVolumeRadiusMetres);
            }
            
//            SphereVolume volume(sphereVolumeCentreLatitudeDegrees, sphereVolumeCentreLongitudeDegrees, sphereVolumeRadiusMetres);
            m_precacheService.Precache(*m_sphereVolume);
            m_sphereVolume->PrintIntersectedKeys();
            m_startedPrecaching = true;
            EXAMPLE_LOG("logs::::: Precache started!\n");
        }
        else
        {
//            EXAMPLE_LOG("logs::::: Precache didn't start.!\n");
        }
        if(m_startedPrecaching && m_precacheService.CurrentlyPrecaching())
        {
            float toLoad = m_precacheService.TotalUrlsToLoad();
            float loaded = m_precacheService.UrlsLoaded();
            
            float percent = loaded/toLoad;
            int displayPercent = static_cast<int>(percent * 100.f);
            EXAMPLE_LOG("logs::::: Precache %d%% complete!\n", displayPercent);
        }
        else if(m_startedPrecaching && !m_precacheComplete)
        {
            EXAMPLE_LOG("logs::::: Precache 100%% complete!\n");
            m_precacheComplete = true;
        }
    }
    
    void ARVuforiaExample::Suspend()
    {
        if(m_precacheService.CurrentlyPrecaching())
        {
            m_precacheService.CancelPrecaching();
        }
        
        m_startedPrecaching = false;
    	m_pARController->StopCamera();
    	m_arTracker.DeInitVuforia();
    }

    void ARVuforiaExample::Draw() {}

    void ARVuforiaExample::UpdateWorld(float dt, Eegeo::EegeoWorld& world, Eegeo::Camera::CameraState cameraState, Examples::ScreenPropertiesProvider& screenPropertyProvider, Eegeo::Streaming::IStreamingVolume& streamingVolume)
    {
        if(m_sphereVolume!=NULL)
        {
            m_pARController->Update(dt, GetCurrentCameraState(), m_world, screenPropertyProvider, *m_sphereVolume);
            //m_pARController->Update(dt, GetCurrentCameraState(), m_world, screenPropertyProvider, streamingVolume);
        }
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
        
//        Eegeo::m33 orientation;
//        Eegeo::v3 right = Eegeo::v3(headTransform[0],headTransform[4],headTransform[8]);
//        Eegeo::v3 up = Eegeo::v3(headTransform[1],headTransform[5],headTransform[9]);
//        Eegeo::v3 forward = Eegeo::v3(-headTransform[2],-headTransform[6],-headTransform[10]);
//        orientation.SetRow(0, right);
//        orientation.SetRow(1, up);
//        orientation.SetRow(2, forward);
        
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
