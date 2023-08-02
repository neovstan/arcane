if (PSDK_GTASA)
  new_test(TimeNotTouchingPadOpcode SA TimeNotTouchingPadOpcode.cpp)

  target_link_directories(TimeNotTouchingPadOpcode_SA PRIVATE $ENV{CLEO_SDK_SA_DIR})
  target_include_directories(TimeNotTouchingPadOpcode_SA PRIVATE $ENV{CLEO_SDK_SA_DIR})
  target_link_libraries(TimeNotTouchingPadOpcode_SA PRIVATE CLEO.lib)
endif()