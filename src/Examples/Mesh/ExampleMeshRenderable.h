// Copyright eeGeo Ltd (2012-2014), All Rights Reserved

#pragma once

#include "RenderableBase.h"
#include "Quaternion.h"


namespace Examples
{
    class ExampleMeshRenderable : public Eegeo::Rendering::RenderableBase
    {
    public:
        ExampleMeshRenderable(Eegeo::Rendering::LayerIds::Values layerId,
                              const Eegeo::dv3& ecefPosition,
                              const Eegeo::Rendering::Materials::IMaterial& material,
                              const Eegeo::Rendering::VertexLayouts::VertexBinding& vertexBinding,
                              Eegeo::Rendering::Mesh& mesh,
                              const Eegeo::v4& initialColor,
                              bool depthTest,
                              bool alphaBlend
                              );
        
        virtual void UpdateMVP(const Eegeo::Rendering::RenderContext& renderContext, float environmentFlatteningScale);

        virtual void Render(Eegeo::Rendering::GLState& glState) const;
        
        virtual void OnMaterialChanged(const Eegeo::Rendering::Materials::IMaterial* pMaterial, Eegeo::Rendering::VertexLayouts::VertexBindingPool& vertexBindingPool);
        
        void SetOrientationEcef(const Eegeo::m33& orientationEcef) { m_orientationEcef = orientationEcef.ToM44(); }
        void SetColor(const Eegeo::v4& color) { m_color = color; }
        void SetDepthTest(bool enabled) { m_depthTest = enabled; }
        void SetAlphaBlend(bool enabled) { m_alphaBlend = enabled; }
        
        const Eegeo::v4& GetColor() const { return m_color; }
        bool IsDepthTest() const { return m_depthTest; }
        bool IsAlphaBlend() const { return m_alphaBlend; }
        
    private:
        Eegeo::Rendering::Mesh& m_mesh;
        Eegeo::m44 m_orientationEcef;
        Eegeo::v4 m_color;
        bool m_depthTest;
        bool m_alphaBlend;
    };
}