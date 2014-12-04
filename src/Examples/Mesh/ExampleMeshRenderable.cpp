// Copyright eeGeo Ltd (2012-2014), All Rights Reserved

#include "ExampleMeshRenderable.h"
#include "RenderContext.h"
#include "RenderCamera.h"
#include "MathsHelpers.h"
#include "Mesh.h"
#include "VertexBindingPool.h"
#include "IMaterial.h"
#include "Shader.h"

namespace Examples
{
    ExampleMeshRenderable::ExampleMeshRenderable(Eegeo::Rendering::LayerIds::Values layerId,
                                                 const Eegeo::dv3& ecefPosition,
                                                 const Eegeo::Rendering::Materials::IMaterial& material,
                                                 const Eegeo::Rendering::VertexLayouts::VertexBinding& vertexBinding,
                                                 Eegeo::Rendering::Mesh& mesh,
                                                 const Eegeo::v4& initialColor,
                                                 bool depthTest,
                                                 bool alphaBlend)
    : Eegeo::Rendering::RenderableBase(layerId, ecefPosition, &material, vertexBinding)
    , m_mesh(mesh)
    , m_color(initialColor)
    , m_depthTest(depthTest)
    , m_alphaBlend(alphaBlend)
    , m_orientationEcef(Eegeo::m44::CreateIdentity())
    {

    }
    
    void ExampleMeshRenderable::OnMaterialChanged(const Eegeo::Rendering::Materials::IMaterial* pMaterial, Eegeo::Rendering::VertexLayouts::VertexBindingPool& vertexBindingPool)
    {
        const Eegeo::Rendering::VertexLayouts::VertexBinding& vertexBinding = vertexBindingPool.GetVertexBinding(m_mesh.GetVertexLayout(), GetMaterial()->GetShader().GetVertexAttributes());
        SetVertexBinding(vertexBinding);
    }
    
    void ExampleMeshRenderable::Render(Eegeo::Rendering::GLState& glState) const
    {
        m_material->SetStatePerRenderable(this, glState);
        
        m_mesh.BindVertexBuffers(GetVertexBinding(), glState);
        
        Eegeo_GL(glDrawElements(GL_TRIANGLES, m_mesh.GetNumOfIndices(), GL_UNSIGNED_SHORT, (void*)(0)));
        
        m_mesh.UnbindVertexBuffers(glState);
    }
    
    void ExampleMeshRenderable::UpdateMVP(const Eegeo::Rendering::RenderContext& renderContext, float environmentFlatteningScale)
    {
        const Eegeo::Camera::RenderCamera& renderCamera = renderContext.GetRenderCamera();
        
        const Eegeo::m44& viewProjection = renderCamera.GetViewProjectionMatrix();
        const Eegeo::dv3& ecefCameraPosition = renderCamera.GetEcefLocation();
        
        const Eegeo::v3& cameraRelativeModelOrigin = (GetEcefPosition() - ecefCameraPosition).ToSingle();
        
        Eegeo::m44 cameraRelativeModel;
        Eegeo::Helpers::MathsHelpers::ComputeScaleAndOffset(cameraRelativeModel, environmentFlatteningScale, GetEcefPosition().Norm().ToSingle(), cameraRelativeModelOrigin);
        

        Eegeo::m44 orientedCameraRelativeModel;
        Eegeo::m44::Mul(orientedCameraRelativeModel, cameraRelativeModel, m_orientationEcef);
        
        Eegeo::m44::Mul(m_modelViewProjection, viewProjection, orientedCameraRelativeModel);
    }
}