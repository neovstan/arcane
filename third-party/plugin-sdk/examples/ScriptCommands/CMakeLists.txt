project(ScriptCommands)
set(SOURCES ScriptCommands.cpp)

if (PSDK_GTAIII)
  new_test(${PROJECT_NAME} III ${SOURCES})
endif()

if (PSDK_GTASA)
  new_test(${PROJECT_NAME} SA ${SOURCES})
endif()

if (PSDK_GTAVC)
  new_test(${PROJECT_NAME} VC ${SOURCES})
endif()