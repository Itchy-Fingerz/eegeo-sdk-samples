// Copyright eeGeo Ltd (2012-2014), All Rights Reserved

#pragma once

#include "ARCameraController.h"
#include "ExampleHandler.h"
#include "Camera.h"
#include "Geometry.h"
#include "Streaming.h"
#include "GlobeCamera.h"
#include "ICallback.h"
#include "ConfigSections.h"
#include "ARVuforiaController.h"
#include "IARTracker.h"
#include "ScreenPropertiesProvider.h"
#include "Types.h"

namespace Examples
{

	class SphereVolume;
	class ARVuforiaExample : public ExampleHandler, Eegeo::NonCopyable
	{

	private:

		Eegeo::EegeoWorld& m_world;

		Eegeo::AR::ARCameraController* m_pCameraController;
		Eegeo::AR::ARVuforiaController* m_pARController;
		Examples::IARTracker& m_arTracker;
        
        bool m_startedPrecaching;
        bool m_precacheComplete;
        
        Eegeo::Web::PrecacheService& m_precacheService;
        SphereVolume* m_sphereVolume;
        
	public:

        ARVuforiaExample(Eegeo::EegeoWorld& eegeoWorld,
                            Eegeo::Web::PrecacheService& precacheService,
						   	Eegeo::Camera::GlobeCamera::GlobeCameraController* pCameraController,
						   	const IScreenPropertiesProvider& initialScreenProperties,
                           				Eegeo::Modules::Map::MapModule& mapModule,
						   	Examples::IARTracker& arTracker);

		virtual ~ARVuforiaExample();

		static std::string GetName()
		{
			return "ARVuforiaExample";
		}
		std::string Name() const
		{
			return GetName();
		}

		void Start();
		void OrientationUpdate();
		void EarlyUpdate(float dt);
        	void Update(float dt);
		void PreWorldDraw() { }
		void Draw();
		void Suspend();
		virtual int InitTracker();
		virtual int LoadTrackerData();
		virtual void OnVuforiaInitializedNative();
		virtual void InitVuforiaRendering();
		virtual void UpdateVuforiaRendering(int width, int height);

		virtual void UpdateWorld(float dt, Eegeo::EegeoWorld& world, Eegeo::Camera::CameraState cameraState, Examples::ScreenPropertiesProvider& screenPropertyProvider, Eegeo::Streaming::IStreamingVolume& streamingVolume);
		virtual void DrawWorld(Eegeo::EegeoWorld& world,  Eegeo::Camera::CameraState cameraState, Examples::ScreenPropertiesProvider& screenPropertyProvider);

        
        const Eegeo::m33& GetCurrentCameraOrientation();
        const Eegeo::m33& GetBaseOrientation();
        const Eegeo::m33& GetHeadTrackerOrientation();
        virtual void SetVRCameraState(const float headTransform[]);
        
        
		virtual Eegeo::Camera::CameraState GetCurrentCameraState() const;
		virtual bool IsVRExample(){return true;}

		virtual void NotifyScreenPropertiesChanged(const Eegeo::Rendering::ScreenProperties& screenProperties);

		void NotifyViewNeedsLayout() {}

		void Event_TouchRotate (const AppInterface::RotateData& data) { }
		void Event_TouchRotate_Start (const AppInterface::RotateData& data) { }
		void Event_TouchRotate_End (const AppInterface::RotateData& data) { }

		void Event_TouchPinch (const AppInterface::PinchData& data) { m_pARController->Event_TouchPinch(data); }
		void Event_TouchPinch_Start (const AppInterface::PinchData& data) { }
		void Event_TouchPinch_End (const AppInterface::PinchData& data) { }

        void Event_TouchPan (const AppInterface::PanData& data) { m_pARController->Event_TouchPan(data); }
        void Event_TouchPan_Start (const AppInterface::PanData& data) { m_pARController->Event_TouchPan_Start(data); }
		void Event_TouchPan_End (const AppInterface::PanData& data) { }

		void Event_TouchTap (const AppInterface::TapData& data) { }
		void Event_TouchDoubleTap (const AppInterface::TapData& data) { }
		void Event_TouchDown (const AppInterface::TouchData& data) { }
		void Event_TouchMove (const AppInterface::TouchData& data) { }
		void Event_TouchUp (const AppInterface::TouchData& data) { }
	
	};
}
