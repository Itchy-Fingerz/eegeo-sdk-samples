// Copyright eeGeo Ltd (2012-2014), All Rights Reserved

#ifndef __ExampleApp__VRCardboardExampleFactory__
#define __ExampleApp__VRCardboardExampleFactory__

#include "IExampleFactory.h"
#include "EegeoRootDeclarations.h"
#include "ScreenPropertiesProvider.h"
#include "IVRHeadTracker.h"

namespace Examples
{
    class VRCardboardExampleFactory : public IExampleFactory
    {
        Eegeo::EegeoWorld& m_world;
        const IScreenPropertiesProvider& m_screenPropertiesProvider;
        DefaultCameraControllerFactory& m_defaultCameraControllerFactory;
        IVRHeadTracker& m_headTracker;
        
    public:
        VRCardboardExampleFactory( Eegeo::EegeoWorld& world,
                                      DefaultCameraControllerFactory& defaultCameraControllerFactory,
                                      IVRHeadTracker& headTracker,
                                      const IScreenPropertiesProvider& screenPropertiesProvider);
        
        std::string ExampleName() const;
        
        IExample* CreateExample() const;
    };
}


#endif /* defined(__ExampleApp__VRCardboardFactory__) */
