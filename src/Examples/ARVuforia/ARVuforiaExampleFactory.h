// Copyright eeGeo Ltd (2012-2014), All Rights Reserved

#pragma once

#include "IExampleFactory.h"
#include "EegeoRootDeclarations.h"
#include "ScreenPropertiesProvider.h"
#include "ConfigSections.h"
#include "IARTracker.h"

namespace Examples
{
    class ARVuforiaExampleFactory : public IExampleFactory
    {
        Eegeo::EegeoWorld& m_world;
        const IScreenPropertiesProvider& m_screenPropertiesProvider;
        DefaultCameraControllerFactory& m_defaultCameraControllerFactory;
        Examples::IARTracker& m_arTracker;
        
    public:
        ARVuforiaExampleFactory(Eegeo::EegeoWorld& world,
                                  DefaultCameraControllerFactory& defaultCameraControllerFactory,
                                  const IScreenPropertiesProvider& screenPropertiesProvider,
                                  Examples::IARTracker& arTracker);
        
        std::string ExampleName() const;
        
        IExample* CreateExample() const;
    };
}
