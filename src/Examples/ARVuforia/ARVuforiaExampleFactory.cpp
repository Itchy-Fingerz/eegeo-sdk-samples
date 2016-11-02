// Copyright eeGeo Ltd (2012-2014), All Rights Reserved

#include "ARVuforiaExampleFactory.h"
#include "ARVuforiaExample.h"
#include "EegeoWorld.h"
#include "ResourceCeilingProvider.h"
#include "DefaultCameraControllerFactory.h"
#include "MapModule.h"

namespace Examples
{

    ARVuforiaExampleFactory::ARVuforiaExampleFactory(Eegeo::EegeoWorld& world,
    		 	 	 	 	 	 	 	 	 	 	 	 DefaultCameraControllerFactory& defaultCameraControllerFactory,
                                                         const IScreenPropertiesProvider& screenPropertiesProvider,
														 Examples::IARTracker& arTracker)
    : m_world(world)
    , m_screenPropertiesProvider(screenPropertiesProvider)
    , m_defaultCameraControllerFactory(defaultCameraControllerFactory)
	, m_arTracker(arTracker)
	{
    
	}

	IExample* ARVuforiaExampleFactory::CreateExample() const
	{
	    Eegeo::Modules::Map::MapModule& mapModule = m_world.GetMapModule();

	    return new Examples::ARVuforiaExample(m_world, m_world.GetStreamingModule().GetPrecachingService(),
	                                            m_defaultCameraControllerFactory.Create(),
												m_screenPropertiesProvider,
                                                						mapModule,
												m_arTracker);
	}

	std::string ARVuforiaExampleFactory::ExampleName() const
	{
		return Examples::ARVuforiaExample::GetName();
	}
}
