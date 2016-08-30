//  Copyright (c) 2014 eeGeo Ltd. All rights reserved.

#pragma once
#include "Types.h"
#include "RenderCamera.h"
#include "VectorMathDecl.h"
#include "Space.h"
#include "IInterestPointProvider.h"
#include "TerrainHeightProvider.h"
#include "CameraState.h"
#include "IVRHeadTracker.h"

namespace Eegeo
{
    namespace VR
    {
        class VRCameraController : public Eegeo::Location::IInterestPointProvider, protected Eegeo::NonCopyable
        {
            
        public:
            VRCameraController(float screenWidth, float screenHeight,
                               Examples::IVRHeadTracker& headTracker)
            : m_ecefPosition(0.0, 0.0, 0.0)
            , m_pTerrainHeightProvider(NULL)
            , m_nearMultiplier(0.1f)
            , m_pHeadTracker(headTracker)
            {
                m_renderCamera = new Camera::RenderCamera();
                m_orientation.Identity();
                m_currentOrientation.Identity();
                m_headTrackerOrientation.Identity();
                m_renderCamera->SetViewport(0,0,screenWidth, screenHeight);
                m_renderCamera->SetProjection(0.7, 0.1, 4000);
            }
            
            ~VRCameraController() { };
            
            Eegeo::dv3 GetEcefInterestPoint() const;
            double GetAltitudeAboveSeaLevel() const;

            Eegeo::Camera::RenderCamera& GetCamera() { return *m_renderCamera; }
            const dv3& GetCameraPosition() const { return m_ecefPosition; }
            const m33& GetCameraOrientation() const { return m_orientation; }
            const Eegeo::m33& GetHeadTrackerOrientation() { return m_headTrackerOrientation; };
            void SetNearMultiplier(float p_nearMultiplier) { m_nearMultiplier = p_nearMultiplier; }

            
            void SetProjectionMatrix(Eegeo::m44& projection);
            void UpdateFromPose(const Eegeo::m33& orientation, float eyeDistance);
            void SetEcefPosition(const Eegeo::dv3& ecef);
            void SetStartLatLongAltitude(const Eegeo::Space::LatLongAltitude& eyePos);
            
            void GetNearFarPlaneDistances(float& out_near, float& out_far);
                        
            void Update(float dt);
            
            void SetTerrainHeightProvider(Eegeo::Resources::Terrain::Heights::TerrainHeightProvider * pTerrainHeightProvider) { m_pTerrainHeightProvider = pTerrainHeightProvider;}
            
            Camera::CameraState GetCameraState() const;
            
            m33& GetOrientation();
            
            
        private:
            
            Examples::IVRHeadTracker& m_pHeadTracker;
            Eegeo::Camera::RenderCamera* m_renderCamera;
            Eegeo::Resources::Terrain::Heights::TerrainHeightProvider * m_pTerrainHeightProvider;

            
            
            dv3 m_interestEcef;
            dv3 m_ecefPosition;
            m33 m_orientation;
            m33 m_currentOrientation;
            m33 m_headTrackerOrientation;
            float m_nearMultiplier;

        };
    }
}
