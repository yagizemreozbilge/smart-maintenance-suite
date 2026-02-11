# tests/smart_maintenance_suite_app/CMakeLists.txt
set(ROOT src/tests)
set(TESTNAME smart_maintenance_suite_app)
set(EXENAME ${TESTNAME}_tests)

message(STATUS "[${ROOT}/${TESTNAME}] Module Tests...")

# Include header files
file(GLOB APP_HEADERS
  "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp")

# Include source files
file(GLOB APP_SOURCES
  "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/*.cc")

# Define sources from the application module we want to test
# We need to compile the implementation of the demo functions
set(APP_IMPL_SOURCES
    "${CMAKE_SOURCE_DIR}/src/smart_maintenance_suite_app/src/smart_maintenance_suite_app.cpp"
)

# Enable testing
enable_testing()

# Define the target executable
add_executable(${EXENAME} ${APP_HEADERS} ${APP_SOURCES} ${APP_IMPL_SOURCES})

# Define macro to exclude main() from app source when testing
target_compile_definitions(${EXENAME} PRIVATE ENABLE_DEMO_APP_TEST)

# Add include directories
target_include_directories(${EXENAME} PUBLIC
                           ${CMAKE_SOURCE_DIR}/src/utility/header
                           ${CMAKE_SOURCE_DIR}/src/smart_maintenance_suite/header
                           ${CMAKE_SOURCE_DIR}/src/smart_maintenance_suite_app/header
                           ${CMAKE_CURRENT_SOURCE_DIR})

# Link libraries
target_link_libraries(${EXENAME} PRIVATE smart_maintenance_suite utility gtest gtest_main)

# Register tests
include(GoogleTest)
gtest_discover_tests(${EXENAME})

install(TARGETS ${EXENAME}
        RUNTIME DESTINATION bin
)

message(STATUS "[${ROOT}/${TESTNAME}] Added target: ${EXENAME}")
