// Copyright eeGeo Ltd (2012-2016), All Rights Reserved

#include "CameraHelpers.h"
#include "IntersectionTests.h"
#include "RenderingModule.h"
#include "WorldMenuItemView.h"
#include "WorldMenuItem.h"

namespace Eegeo
{
    namespace UI
    {
        namespace WorldMenu
        {
            
            WorldMenuItemView::WorldMenuItemView(WorldMenuItem& worldMenuItem
                                                       , Eegeo::UI::IUIRenderableFilter& uiRenderableFilter
                                                       , Eegeo::UI::IUIQuadFactory& quadFactory
                                                       , const std::string& assetPath
                                                       , const Eegeo::UI::UIProgressBarConfig& progressBarConfig
                                                       , Eegeo::v2& dimension
                                                       , Eegeo::v2& uvMin
                                                       , Eegeo::v2& uvMax
                                                       )
            : WorldMenuUIButton(uiRenderableFilter
                                   , quadFactory
                                   , assetPath
                                   , progressBarConfig
                                   , dimension
                                   , Eegeo::v3::One()
                                   , Eegeo::v3::One()
                                   , Eegeo::v4::One()
                                   , uvMin
                                   , uvMax)
            ,m_WorldMenuItem(worldMenuItem)
            {}
            
            bool WorldMenuItemView::IsCollidingWithPoint(const Eegeo::v2& screenPoint, Eegeo::UI::IUICameraProvider& cameraProvider)
            {
                
                if(!m_pSprite->GetItemShouldRender())
                    return false;
                
                Eegeo::Camera::RenderCamera& renderCamera = cameraProvider.GetRenderCameraForUI();
                if (renderCamera.GetEcefLocation().SquareDistanceTo(m_pSprite->GetEcefPosition()) < (GetItemRadius() * GetItemRadius()))
                {
                    return false;
                }
                
                Eegeo::dv3 rayOrigin = renderCamera.GetEcefLocation();
                Eegeo::dv3 rayDirection;
                
                Eegeo::Camera::CameraHelpers::GetScreenPickRay(renderCamera, screenPoint.GetX(), screenPoint.GetY(), rayDirection);
                return Eegeo::Geometry::IntersectionTests::TestRaySphere(rayOrigin, rayDirection, m_pSprite->GetEcefPosition(), GetItemRadius());
            }
            
            void WorldMenuItemView::OnItemClicked()
            {
                m_WorldMenuItem.GetCallback()(m_WorldMenuItem);
            }
            
            
        }
    }
}
