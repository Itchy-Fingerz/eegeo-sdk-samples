set(headers ${headers} ../src/Examples/VRCameraSpline
)
set(currsources  ../src/Examples/VRCameraSpline/IVRHeadTracker.h  ../src/Examples/VRCameraSpline/VRCardboardExample.cpp  ../src/Examples/VRCameraSpline/VRCardboardExample.h  ../src/Examples/VRCameraSpline/VRCardboardExampleFactory.cpp  ../src/Examples/VRCameraSpline/VRCardboardExampleFactory.h  ../src/Examples/VRCameraSpline/VRCameraController.cpp  ../src/Examples/VRCameraSpline/VRCameraController.h
)

set(sources ${sources} ${currsources})
source_group(\\src\\Examples\\VRCameraSpline FILES ${currsources})