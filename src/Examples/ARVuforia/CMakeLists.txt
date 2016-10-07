set(headers ${headers} ../src/Modules/SkyboxModule/
)
set(currsources
  ../src/Examples/ARVuforia/ARVuforiaController.cpp
  ../src/Examples/ARVuforia/ARVuforiaController.h
)

set(sources ${sources} ${currsources})
source_group(\\src\\Examples\\ARVuforiaController\\ FILES ${currsources})
