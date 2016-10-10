set(headers ${headers} ../src/Examples/ARVuforia/
)
set(currsources
 ../src/Examples/ARVuforia/ARVuforiaController.cpp ../src/Examples/ARVuforia/ARVuforiaController.h ../src/Examples/ARVuforia/ARVuforiaExample.cpp ../src/Examples/ARVuforia/ARVuforiaExample.h
 ../src/Examples/ARVuforia/ARCameraController.cpp ../src/Examples/ARVuforia/ARCameraController.h ../src/Examples/ARVuforia/ARVuforiaExampleFactory.cpp ../src/Examples/ARVuforia/ARVuforiaExampleFactory.h ../src/Examples/ARVuforia/IARTracker.h
)

set(sources ${sources} ${currsources})
source_group(\\src\\Examples\\ARVuforia\\ FILES ${currsources})
