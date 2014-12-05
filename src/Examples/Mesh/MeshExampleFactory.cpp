// Copyright eeGeo Ltd (2012-2014), All Rights Reserved

#include "MeshExampleFactory.h"
#include "MeshExample.h"

#include "RenderingModule.h"

namespace Examples
{
    MeshExampleFactory::MeshExampleFactory(Eegeo::EegeoWorld& world,
                                                     Eegeo::Camera::GlobeCamera::GlobeCameraController& globeCameraController)
    : m_world(world)
    , m_globeCameraController(globeCameraController)
    {
        
    }
    
    IExample* MeshExampleFactory::CreateExample() const
    {
        Eegeo::Modules::Core::RenderingModule& renderingModule = m_world.GetRenderingModule();
        const Eegeo::Modules::IPlatformAbstractionModule& platformAbstractionModule = m_world.GetPlatformAbstractionModule();
        Eegeo::Modules::Map::MapModule& mapModule = m_world.GetMapModule();
        
        MeshExampleConfig config;
        config.textureFilename = "mesh_example/quadrants.png";
        config.originLatLong = std::make_pair(37.790, -122.404);
        config.spacing = 0.002f;
        config.altitude = 100.f;
        config.meshRows = 5;
        config.meshColumns = 3;
        config.boxWidth = 50.f;
        config.boxHeight = 30.f;
        config.revsPerMinuteA = 29.f;
        config.revsPerMinuteB = -17.f;
        config.environmentFlatteningCyclesPerMinute = 4.f;
        
        return new Examples::MeshExample(m_globeCameraController, renderingModule, platformAbstractionModule.GetTextureFileLoader(), mapModule.GetEnvironmentFlatteningService(), config);

    }
    
    std::string MeshExampleFactory::ExampleName() const
    {
        return Examples::MeshExample::GetName();
    }
}
