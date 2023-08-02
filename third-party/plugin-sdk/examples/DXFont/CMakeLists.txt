project(DXFont)
set(SOURCES main.cpp)

if (PSDK_GTAIII)
  new_test(${PROJECT_NAME} III ${SOURCES})

  target_link_directories(${PROJECT_NAME}_III PRIVATE $ENV{RWD3D9_DIR})
  target_include_directories(${PROJECT_NAME}_III PRIVATE $ENV{RWD3D9_DIR})
  target_link_libraries(${PROJECT_NAME}_III PRIVATE rwd3d9.lib)
endif()

if (PSDK_GTASA)
  new_test(${PROJECT_NAME} SA ${SOURCES})
endif()

if (PSDK_GTAVC)
  new_test(${PROJECT_NAME} VC ${SOURCES})

  target_link_directories(${PROJECT_NAME}_VC PRIVATE $ENV{RWD3D9_DIR})
  target_include_directories(${PROJECT_NAME}_VC PRIVATE $ENV{RWD3D9_DIR})
  target_link_libraries(${PROJECT_NAME}_VC PRIVATE rwd3d9.lib)
endif()