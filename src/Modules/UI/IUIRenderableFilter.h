// Copyright eeGeo Ltd (2012-2014), All Rights Reserved

#pragma once

#include "IUIRenderable.h"

namespace Eegeo
{
    namespace UI
    {
        class IUIRenderableFilter
        {
        public:
            virtual void RegisterRenderable(IUIRenderable* pUIItemToRender) = 0;
            virtual void UnRegisterRenderable(IUIRenderable* pUIItemToRender) = 0;
            virtual ~IUIRenderableFilter() {}
        };
        
    }
}

