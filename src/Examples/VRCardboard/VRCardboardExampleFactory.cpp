// Copyright eeGeo Ltd (2012-2014), All Rights Reserved

#include "VRCardboardExampleFactory.h"
#include "VRCardboardExample.h"
#include "EegeoWorld.h"
#include "ResourceCeilingProvider.h"
#include "DefaultCameraControllerFactory.h"
#include "MapModule.h"

namespace Examples
{

	VRCardboardExampleFactory::VRCardboardExampleFactory(Eegeo::EegeoWorld& world,
                                                               DefaultCameraControllerFactory&defaultCameraControllerFactory,
                                                               IVRHeadTracker& headTracker,
                                                               const IScreenPropertiesProvider& screenPropertiesProvider)
    : m_world(world)
    , m_headTracker(headTracker)
    , m_screenPropertiesProvider(screenPropertiesProvider)
    , m_defaultCameraControllerFactory(defaultCameraControllerFactory)
{
    
}

IExample* VRCardboardExampleFactory::CreateExample() const
{
    Eegeo::Modules::Map::MapModule& mapModule = m_world.GetMapModule();
    
    const Eegeo::Rendering::ScreenProperties& initialScreenProperties = m_screenPropertiesProvider.GetScreenProperties();
    
    return new Examples::VRCardboardExample(m_world,
                                                mapModule.GetResourceCeilingProvider(),
                                                m_defaultCameraControllerFactory.Create(),
                                                m_headTracker,
                                                initialScreenProperties);
}

std::string VRCardboardExampleFactory::ExampleName() const
{
	return Examples::VRCardboardExample::GetName();
}
}