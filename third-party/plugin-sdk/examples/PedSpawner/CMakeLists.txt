project(PedSpawner)
set(SOURCES main.cpp)

if (PSDK_GTASA)
  new_test(${PROJECT_NAME} SA ${SOURCES})
endif()

if (PSDK_GTAVC)
  new_test(${PROJECT_NAME} VC ${SOURCES})
endif()