set(headers ${headers} ../src/Modules/SkyboxModule/
)
set(currsources
  ../src/Modules/VuforiaModule/VuforiaModule.cpp
  ../src/Modules/VuforiaModule/VuforiaModule.h
)

set(sources ${sources} ${currsources})
source_group(\\src\\Modules\\VuforiaModule\\ FILES ${currsources})
