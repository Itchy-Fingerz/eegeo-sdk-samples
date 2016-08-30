//  Copyright (c) 2014 eeGeo Ltd. All rights reserved.

#include "VRCameraController.h"
#include "LatLongAltitude.h"
#include "CameraHelpers.h"
#include "SpaceHelpers.h"
#include "EcefTangentBasis.h"
#include "Quaternion.h"
#include "EarthConstants.h"
#include "MathFunc.h"
#include "IVRHeadTracker.h"
#include "Logger.h"

namespace Eegeo
{
    namespace VR
    {
        const float GravityAcceleration = 15.0f;
        const float TerminalVelocity = 500.f;
        
        Eegeo::dv3 VRCameraController::GetEcefInterestPoint() const
        {
            
            dv3 ecefPosition = m_ecefPosition;
            return ecefPosition.Normalise() * Eegeo::Space::EarthConstants::Radius;
        }
        
        double VRCameraController::GetAltitudeAboveSeaLevel() const
        {
            return Space::SpaceHelpers::GetAltitude(m_renderCamera->GetEcefLocation());
        }
        
        void VRCameraController::SetProjectionMatrix(Eegeo::m44& projection)
        {
            m_renderCamera->SetProjectionMatrix(projection);
        }
        
        Camera::CameraState VRCameraController::GetCameraState() const
        {
            return Camera::CameraState(m_renderCamera->GetEcefLocation(),
                                       GetEcefInterestPoint(),
                                       m_renderCamera->GetViewMatrix(),
                                       m_renderCamera->GetProjectionMatrix());
        }
        
        m33& VRCameraController::GetOrientation()
        {
            return m_currentOrientation;
        }
        
        void VRCameraController::UpdateFromPose(const Eegeo::m33& orientation, float eyeDistance)
        {
            m_headTrackerOrientation = orientation;
            m33 orientationMatrix;
            m33::Mul(orientationMatrix, m_orientation, orientation);
            
            v3 eyeOffsetModified = dv3::ToSingle(m_ecefPosition.Norm()*eyeDistance);
            v3 rotatedEyeOffset = v3::Mul(eyeOffsetModified, orientationMatrix);
            
            v3 rA = m_orientation.GetRow(0);
            v3 rB = orientationMatrix.GetRow(0);
            
            v3 uA = m_orientation.GetRow(1);
            v3 uB = orientationMatrix.GetRow(1);
            
            v3 fA = m_orientation.GetRow(2) * -1;
            v3 fB = orientationMatrix.GetRow(2) * -1;
            
            float rAngle = Math::Rad2Deg(Math::ACos(v3::Dot(rA, rB) / (rA.Length() * rB.Length())));
            float uAngle = Math::Rad2Deg(Math::ACos(v3::Dot(uA, uB) / (uA.Length() * uB.Length())));
            float fAngle = Math::Rad2Deg(Math::ACos(v3::Dot(fA, fB) / (fA.Length() * fB.Length())));
            
            float factor = 1.0f;
            if(uAngle<100.f && fAngle<100.f){
                factor = 1.f - (uAngle/90.f + fAngle/90.f)/2.f;
            }else{
                factor = rAngle / 90.f;
                if(factor > 0.9f)
                    factor = 0.9f;
            }
            
            m_currentOrientation = Eegeo::m33(orientationMatrix);
            
            float near, far;
            GetNearFarPlaneDistances(near,far);
            if(!std::isnan(factor)){
                m_renderCamera->SetOrientationMatrix(orientationMatrix);
                m_renderCamera->SetEcefLocation(dv3(m_ecefPosition.x + rotatedEyeOffset.x, m_ecefPosition.y + rotatedEyeOffset.y, m_ecefPosition.z + rotatedEyeOffset.z));
                m_renderCamera->SetProjection(0.7f, near*m_nearMultiplier, far);
            }
            
        }
        
        void VRCameraController::SetEcefPosition(const Eegeo::dv3& ecef)
        {
            m_ecefPosition = ecef;
            m_renderCamera->SetEcefLocation(m_ecefPosition);
        }
        
        void VRCameraController::SetStartLatLongAltitude(const Eegeo::Space::LatLongAltitude& eyePos)
        {
            m_ecefPosition = eyePos.ToECEF();
            
            Space::EcefTangentBasis tangentBasis;
            Camera::CameraHelpers::EcefTangentBasisFromPointAndHeading(m_ecefPosition, 0.f, tangentBasis);
            
            m_orientation.SetRow(0, tangentBasis.GetRight());
            m_orientation.SetRow(1, tangentBasis.GetUp());
            m_orientation.SetRow(2, -tangentBasis.GetForward());
            
            m_renderCamera->SetOrientationMatrix(m_orientation);
            m_renderCamera->SetEcefLocation(m_ecefPosition);
        }
        
        void VRCameraController::Update(float dt)
        {
            
        }



        void VRCameraController::GetNearFarPlaneDistances(float& out_near, float& out_far)
        {
            double cameraAltitude = GetAltitudeAboveSeaLevel();
            double approxTerrainAltitude = 100;
            double approxTerrainAltitudeDelta = approxTerrainAltitude -100;
            
            const double ClipPlaneThresholdAltitude = 15000.0;
            //             * \param The altitude in meters of the camera
            //             * \param The approximate altitude of the highest terrain in the scene
            //             * \param The difference between the highest and lowest terrain in the scene
            //             * \param Altitude at which the method for scaling the near/far bounds changes (Default 15000m)
            //             * \param out_nearDistance [out] resultant recommended Near value based on the provided altitudes
            //             * \param out_farDistance [out] resultant reecommended Far value based on the provided altitudes
            Camera::CameraHelpers::GetAltitudeInterpolatedNearFar(cameraAltitude, approxTerrainAltitude, approxTerrainAltitudeDelta, ClipPlaneThresholdAltitude, out_near, out_far);
  
            
        }

    }
}
