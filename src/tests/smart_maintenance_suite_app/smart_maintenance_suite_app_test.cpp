
#define ENABLE_SMART_MAINTENANCE_SUITE_TEST // For main library
#include "gtest/gtest.h"
#include "../../smart_maintenance_suite_app/header/smart_maintenance_suite_app.h"

// Define a test fixture if needed, but simple tests are fine for these demos
class SmartMaintenanceSuiteAppTest : public ::testing::Test {
 protected:
  void SetUp() override {
    // Setup code if needed
  }
};

TEST_F(SmartMaintenanceSuiteAppTest, DemoSensorAnalyticsRuns) {
  bool result = DemoApp::demonstrateSensorAnalytics();
  EXPECT_TRUE(result) << "Sensor Analytics Demo failed to run";
}

TEST_F(SmartMaintenanceSuiteAppTest, DemoMaintenanceSchedulingRuns) {
  bool result = DemoApp::demonstrateMaintenanceScheduling();
  EXPECT_TRUE(result) << "Maintenance Scheduling Demo failed to run";
}

TEST_F(SmartMaintenanceSuiteAppTest, DemoInventoryOptimizationRuns) {
  bool result = DemoApp::demonstrateInventoryOptimization();
  EXPECT_TRUE(result) << "Inventory Optimization Demo failed to run";
}
