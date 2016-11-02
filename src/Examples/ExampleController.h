// Copyright eeGeo Ltd (2012-2014), All Rights Reserved

#ifndef __ExampleApp__ExampleController__
#define __ExampleApp__ExampleController__

#include "GlobeCameraExampleBase.h"
#include "IExampleFactory.h"
#include "Types.h"
#include "EegeoWorld.h"
#include "UIActionHandler.h"
#include "IExampleControllerView.h"
#include "Camera.h"
#include "ScreenPropertiesProvider.h"
#include "DefaultCameraControllerFactory.h"
#include "CameraState.h"
#include <vector>
#include <string>
#include "VRCameraState.h"
#include "IVRModeTracker.h"
#include "IARTracker.h"

namespace Examples
{
class ExampleController : private Eegeo::NonCopyable
{
	IExample* m_pCurrentExample;
	int m_currentExampleFactoryIndex;
	Eegeo::EegeoWorld& m_world;
	std::vector<IExampleFactory*> m_factories;
	bool m_uiVisible;
  	DefaultCameraControllerFactory& m_defaultCameraControllerFactory;
    Eegeo::Camera::GlobeCamera::GlobeCameraTouchController& m_globeCameraTouchController;

	IExampleControllerView& m_view;
	UIActionHandler<ExampleController> m_nextExampleHandler;
	UIActionHandler<ExampleController> m_previousExampleHandler;
    UIActionHandler<ExampleController> m_selectedExampleChangedHandler;

	void DestroyCurrentExample();

	void RefreshExample();

public:
	ExampleController(Eegeo::EegeoWorld& world,
	                  IExampleControllerView& view,
                      DefaultCameraControllerFactory& defaultCameraControllerFactory,
                      Eegeo::Camera::GlobeCamera::GlobeCameraTouchController& globeCameraTouchController);

	~ExampleController();

	std::vector<std::string> GetExampleNames() const;

	void UpdateSelectedExample();

	void ActivatePrevious();

	void ActivateNext();

	void EarlyUpdate(float dt);

	void Update(float dt);
    
	void PreWorldDraw();
    
	void Draw();

	void RegisterExample(IExampleFactory* pFactory);
    
    Eegeo::Camera::CameraState GetCurrentCameraState() const;
	void UpdateCardboardProfile(const float cardboardProfile[]);
	void SetVRCameraState(const float headTransform[]){m_pCurrentExample->SetVRCameraState(headTransform);};
	int InitTracker(){ return m_pCurrentExample->InitTracker();};
	int LoadTrackerData(){return m_pCurrentExample->LoadTrackerData();}
	void OnVuforiaInitializedNative(){ m_pCurrentExample->OnVuforiaInitializedNative();}
	void InitVuforiaRendering(){ m_pCurrentExample->InitVuforiaRendering();	}
	void UpdateVuforiaRendering(int width, int height){	m_pCurrentExample->UpdateVuforiaRendering(width, height);}

	void UpdateWorld(float dt, Examples::ScreenPropertiesProvider& screenPropertyProvider);
	void DrawWorld(Examples::ScreenPropertiesProvider& screenPropertyProvider);
    
    Eegeo::Streaming::IStreamingVolume& GetCurrentStreamingVolume() const;
    
    void NotifyScreenPropertiesChanged(const Eegeo::Rendering::ScreenProperties& screenProperties);

	template <typename TExampleFactory>
	void RegisterExample()
	{
		m_factories.push_back(Eegeo_NEW((TExampleFactory)(m_world)));
	}

	template <typename TExampleFactory>
	void RegisterCameraExample()
	{
		m_factories.push_back(Eegeo_NEW((TExampleFactory)(m_world, m_defaultCameraControllerFactory, m_globeCameraTouchController)));
	}
    
    template <typename TExampleFactory>
    void RegisterCameraControllerScreenPropertiesProviderExample(const ScreenPropertiesProvider& screenPropertiesProvider)
    {
        m_factories.push_back(Eegeo_NEW((TExampleFactory)(m_world, m_defaultCameraControllerFactory, m_globeCameraTouchController, screenPropertiesProvider)));
    }
    
    template <typename TExampleFactory>
    void RegisterScreenPropertiesProviderVRExample(const ScreenPropertiesProvider& screenPropertiesProvider, const Eegeo::Config::DeviceSpec& deviceSpecs, Examples::IVRModeTracker& vrModeTracker)
    {
    	m_factories.push_back(Eegeo_NEW((TExampleFactory)(m_world, deviceSpecs, m_defaultCameraControllerFactory, screenPropertiesProvider, vrModeTracker)));
    }

    template <typename TExampleFactory>
    void RegisterScreenPropertiesProviderARExample(const ScreenPropertiesProvider& screenPropertiesProvider, Examples::IARTracker& arTracker)
    {
    	m_factories.push_back(Eegeo_NEW((TExampleFactory)(m_world, m_defaultCameraControllerFactory, screenPropertiesProvider, arTracker)));
    }

    template <typename TExampleFactory>
    void RegisterScreenPropertiesProviderExample(const ScreenPropertiesProvider& screenPropertiesProvider)
    {
        m_factories.push_back(Eegeo_NEW((TExampleFactory)(m_world, screenPropertiesProvider)));
    }

	void Event_TouchRotate(const AppInterface::RotateData& data);
	void Event_TouchRotate_Start(const AppInterface::RotateData& data);
	void Event_TouchRotate_End(const AppInterface::RotateData& data);

	void Event_TouchPinch(const AppInterface::PinchData& data);
	void Event_TouchPinch_Start(const AppInterface::PinchData& data);
	void Event_TouchPinch_End(const AppInterface::PinchData& data);

	void Event_TouchPan(const AppInterface::PanData& data);
	void Event_TouchPan_Start(const AppInterface::PanData& data);
	void Event_TouchPan_End(const AppInterface::PanData& data);

	void Event_TouchTap(const AppInterface::TapData& data);
	void Event_TouchDoubleTap(const AppInterface::TapData& data);

	void Event_TouchDown(const AppInterface::TouchData& data);
	void Event_TouchMove(const AppInterface::TouchData& data);
	void Event_TouchUp(const AppInterface::TouchData& data);
};
}

#endif /* defined(__ExampleApp__ExampleController__) */
