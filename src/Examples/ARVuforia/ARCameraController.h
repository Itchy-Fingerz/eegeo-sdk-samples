//  Copyright (c) 2014 eeGeo Ltd. All rights reserved.

#pragma once
#include "Types.h"
#include "RenderCamera.h"
#include "VectorMathDecl.h"
#include "Space.h"
#include "IInterestPointProvider.h"
#include "TerrainHeightProvider.h"
#include "CameraState.h"

namespace Eegeo
{
    namespace AR
    {
        class ARCameraController : public Eegeo::Location::IInterestPointProvider, protected Eegeo::NonCopyable
        {
            
        public:
            ARCameraController(float screenWidth,
            				   float screenHeight)
            : m_ecefPosition(0.0, 0.0, 0.0)
            , m_nearMultiplier(0.1f)
            , m_pRenderCamera(NULL)
            {
                m_pRenderCamera = new Camera::RenderCamera();
                m_orientation.Identity();
                m_currentOrientation.Identity();
                m_headTrackerOrientation.Identity();
                m_pRenderCamera->SetViewport(0,0,screenWidth, screenHeight);
                m_pRenderCamera->SetProjection(0.7, 0.1, 4000);
            }
            
            ~ARCameraController() { delete m_pRenderCamera; };
            
            Eegeo::dv3 GetEcefInterestPoint() const;
            double GetAltitudeAboveSeaLevel() const;

            Eegeo::Camera::RenderCamera& GetCamera() { return *m_pRenderCamera; }
            const dv3& GetCameraPosition() const { return m_ecefPosition; }
            const m33& GetCameraOrientation() const { return m_orientation; }
            const Eegeo::m33& GetHeadTrackerOrientation() { return m_headTrackerOrientation; };
            void SetNearMultiplier(float nearMultiplier) { m_nearMultiplier = nearMultiplier; }

            
            void SetProjectionMatrix(Eegeo::m44& projection);
            void UpdateFromPose(const Eegeo::m33& orientation, float eyeDistance);
            void LookAt(const dv3& ecefPosition);
            void SetEcefPosition(const Eegeo::dv3& ecef);
            void SetStartLatLongAltitude(const Eegeo::Space::LatLongAltitude& eyePos);
            
            void GetNearFarPlaneDistances(float& out_near, float& out_far);
                        
            void Update(float dt);
            
            
            Camera::CameraState GetCameraState() const;
            
            m33& GetOrientation();
            
        private:
            
            Eegeo::Camera::RenderCamera* m_pRenderCamera;

            
            
            dv3 m_interestEcef;
            dv3 m_ecefPosition;
            m33 m_orientation;
            m33 m_currentOrientation;
            m33 m_headTrackerOrientation;
            float m_nearMultiplier;

        };
    }
}
