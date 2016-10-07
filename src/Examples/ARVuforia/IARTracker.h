// Copyright eeGeo Ltd (2012-2014), All Rights Reserved

#pragma once

namespace Examples
{
    class IARTracker
    {
    public:
        virtual ~IARTracker(){};
        virtual void InitVuforia() = 0;
        virtual void DeInitVuforia() = 0;
    };
}
