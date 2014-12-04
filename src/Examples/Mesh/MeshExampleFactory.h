// Copyright eeGeo Ltd (2012-2014), All Rights Reserved

#pragma once

#include "IExampleFactory.h"
#include "IExample.h"
#include "EegeoWorld.h"

namespace Examples
{
    class MeshExampleFactory : public IExampleFactory
    {
        Eegeo::EegeoWorld& m_world;
        Eegeo::Camera::GlobeCamera::GlobeCameraController& m_globeCameraController;
        
    public:
        MeshExampleFactory(Eegeo::EegeoWorld& world,
                                Eegeo::Camera::GlobeCamera::GlobeCameraController& globeCameraController);
        
        std::string ExampleName() const;
        
        IExample* CreateExample() const;
    };
}