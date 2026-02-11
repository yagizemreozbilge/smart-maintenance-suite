#define ENABLE_SMART_MAINTENANCE_SUITE_TEST  // Enable Smart Maintenance Suite tests

#include "gtest/gtest.h"
#include "../../smart_maintenance_suite/header/smart_maintenance_suite.h"
#include <stdexcept>
#include <string>

using namespace SmartMaintenance::Analytics;

// ============================================================================
// SensorAnalytics Tests
// ============================================================================

class SensorAnalyticsTest : public ::testing::Test {
 protected:
  // Using fixed size arrays for testing
  double normalReadings[5] = {75.0, 76.0, 75.5, 76.2, 75.8};
  double anomalyReadings[5] = {75.0, 76.0, 75.5, 76.2, 95.0};
  int size = 5;

  void SetUp() override {
    // Arrays already initialized
  }
};

TEST_F(SensorAnalyticsTest, CalculateMovingAverage_ValidInput) {
  double result = SensorAnalytics::calculateMovingAverage(normalReadings, size, 3);
  EXPECT_NEAR(result, 75.833, 0.01); // Average of last 3: (75.5 + 76.2 + 75.8) / 3
}

TEST_F(SensorAnalyticsTest, CalculateMovingAverage_FullWindow) {
  double result = SensorAnalytics::calculateMovingAverage(normalReadings, size, 5);
  EXPECT_NEAR(result, 75.7, 0.01); // Average of all 5
}

TEST_F(SensorAnalyticsTest, CalculateMovingAverage_EmptyArray) {
  // Pass nullptr or size 0
  EXPECT_THROW(SensorAnalytics::calculateMovingAverage(nullptr, 0, 3), std::invalid_argument);
  EXPECT_THROW(SensorAnalytics::calculateMovingAverage(normalReadings, 0, 3), std::invalid_argument);
}

TEST_F(SensorAnalyticsTest, CalculateMovingAverage_InvalidWindowSize) {
  EXPECT_THROW(SensorAnalytics::calculateMovingAverage(normalReadings, size, 0), std::invalid_argument);
  EXPECT_THROW(SensorAnalytics::calculateMovingAverage(normalReadings, size, 10), std::invalid_argument);
}

TEST_F(SensorAnalyticsTest, DetectAnomaly_NoAnomaly) {
  bool result = SensorAnalytics::detectAnomaly(normalReadings, size, 2.0);
  EXPECT_FALSE(result);
}

TEST_F(SensorAnalyticsTest, DetectAnomaly_WithAnomaly) {
  bool result = SensorAnalytics::detectAnomaly(anomalyReadings, size, 2.0);
  EXPECT_TRUE(result);
}

TEST_F(SensorAnalyticsTest, DetectAnomaly_InsufficientData) {
  double single[] = {75.0};
  bool result = SensorAnalytics::detectAnomaly(single, 1, 2.0);
  EXPECT_FALSE(result); // Not enough data
}

TEST_F(SensorAnalyticsTest, CalculateStandardDeviation_ValidInput) {
  double data[] = {2.0, 4.0, 4.0, 4.0, 5.0, 5.0, 7.0, 9.0};
  int dataSize = 8;
  double result = SensorAnalytics::calculateStandardDeviation(data, dataSize);
  EXPECT_NEAR(result, 2.0, 0.01);
}

TEST_F(SensorAnalyticsTest, CalculateStandardDeviation_SingleValue) {
  double single[] = {5.0};
  double result = SensorAnalytics::calculateStandardDeviation(single, 1);
  EXPECT_DOUBLE_EQ(result, 0.0);
}

TEST_F(SensorAnalyticsTest, CalculateStandardDeviation_EmptyArray) {
  EXPECT_THROW(SensorAnalytics::calculateStandardDeviation(nullptr, 0), std::invalid_argument);
}

TEST_F(SensorAnalyticsTest, PredictRemainingLife_ValidInput) {
  double result = SensorAnalytics::predictRemainingLife(80.0, 100.0, 2.0);
  EXPECT_DOUBLE_EQ(result, 10.0); // (100 - 80) / 2 = 10
}

TEST_F(SensorAnalyticsTest, PredictRemainingLife_AtThreshold) {
  double result = SensorAnalytics::predictRemainingLife(100.0, 100.0, 2.0);
  EXPECT_DOUBLE_EQ(result, 0.0);
}

TEST_F(SensorAnalyticsTest, PredictRemainingLife_BeyondThreshold) {
  double result = SensorAnalytics::predictRemainingLife(105.0, 100.0, 2.0);
  EXPECT_DOUBLE_EQ(result, 0.0);
}

TEST_F(SensorAnalyticsTest, PredictRemainingLife_InvalidDegradationRate) {
  EXPECT_THROW(SensorAnalytics::predictRemainingLife(80.0, 100.0, 0.0), std::invalid_argument);
  EXPECT_THROW(SensorAnalytics::predictRemainingLife(80.0, 100.0, -1.0), std::invalid_argument);
}

// ============================================================================
// MaintenanceScheduler Tests
// ============================================================================

class MaintenanceSchedulerTest : public ::testing::Test {
 protected:
  double failureRate;
  double maintenanceCost;
  double failureCost;

  void SetUp() override {
    failureRate = 0.05;
    maintenanceCost = 500.0;
    failureCost = 2000.0;
  }
};

TEST_F(MaintenanceSchedulerTest, CalculateOptimalInterval_ValidInput) {
  double result = MaintenanceScheduler::calculateOptimalInterval(
                    failureRate, maintenanceCost, failureCost
                  );
  EXPECT_GT(result, 0.0);
  EXPECT_NEAR(result, 4.47, 0.1); // sqrt((2 * 500) / (0.05 * 2000))
}

TEST_F(MaintenanceSchedulerTest, CalculateOptimalInterval_InvalidFailureRate) {
  EXPECT_THROW(MaintenanceScheduler::calculateOptimalInterval(0.0, 500.0, 2000.0),
               std::invalid_argument);
  EXPECT_THROW(MaintenanceScheduler::calculateOptimalInterval(-0.1, 500.0, 2000.0),
               std::invalid_argument);
}

TEST_F(MaintenanceSchedulerTest, CalculateOptimalInterval_NegativeCosts) {
  EXPECT_THROW(MaintenanceScheduler::calculateOptimalInterval(0.05, -500.0, 2000.0),
               std::invalid_argument);
  EXPECT_THROW(MaintenanceScheduler::calculateOptimalInterval(0.05, 500.0, -2000.0),
               std::invalid_argument);
}

TEST_F(MaintenanceSchedulerTest, CalculateTotalCost_ValidInput) {
  double result = MaintenanceScheduler::calculateTotalCost(3.0, 12.0, 500.0);
  EXPECT_DOUBLE_EQ(result, 2000.0); // (12 / 3) * 500 = 2000
}

TEST_F(MaintenanceSchedulerTest, CalculateTotalCost_InvalidInterval) {
  EXPECT_THROW(MaintenanceScheduler::calculateTotalCost(0.0, 12.0, 500.0),
               std::invalid_argument);
  EXPECT_THROW(MaintenanceScheduler::calculateTotalCost(-1.0, 12.0, 500.0),
               std::invalid_argument);
}

TEST_F(MaintenanceSchedulerTest, AssessMaintenancePriority_Normal) {
  int result = MaintenanceScheduler::assessMaintenancePriority(50.0, 70.0, 90.0);
  EXPECT_EQ(result, 0); // Normal
}

TEST_F(MaintenanceSchedulerTest, AssessMaintenancePriority_Warning) {
  int result = MaintenanceScheduler::assessMaintenancePriority(75.0, 70.0, 90.0);
  EXPECT_EQ(result, 1); // Warning
}

TEST_F(MaintenanceSchedulerTest, AssessMaintenancePriority_Critical) {
  int result = MaintenanceScheduler::assessMaintenancePriority(95.0, 70.0, 90.0);
  EXPECT_EQ(result, 2); // Critical
}

TEST_F(MaintenanceSchedulerTest, AssessMaintenancePriority_AtThresholds) {
  EXPECT_EQ(MaintenanceScheduler::assessMaintenancePriority(70.0, 70.0, 90.0), 1); // At warning
  EXPECT_EQ(MaintenanceScheduler::assessMaintenancePriority(90.0, 70.0, 90.0), 2); // At critical
}

// ============================================================================
// InventoryOptimizer Tests
// ============================================================================

class InventoryOptimizerTest : public ::testing::Test {
 protected:
  double annualDemand;
  double orderingCost;
  double holdingCost;

  void SetUp() override {
    annualDemand = 1200.0;
    orderingCost = 50.0;
    holdingCost = 2.0;
  }
};

TEST_F(InventoryOptimizerTest, CalculateEOQ_ValidInput) {
  double result = InventoryOptimizer::calculateEOQ(annualDemand, orderingCost, holdingCost);
  EXPECT_NEAR(result, 244.95, 0.1); // sqrt((2 * 1200 * 50) / 2)
}

TEST_F(InventoryOptimizerTest, CalculateEOQ_InvalidParameters) {
  EXPECT_THROW(InventoryOptimizer::calculateEOQ(0.0, 50.0, 2.0), std::invalid_argument);
  EXPECT_THROW(InventoryOptimizer::calculateEOQ(1200.0, 0.0, 2.0), std::invalid_argument);
  EXPECT_THROW(InventoryOptimizer::calculateEOQ(1200.0, 50.0, 0.0), std::invalid_argument);
  EXPECT_THROW(InventoryOptimizer::calculateEOQ(-100.0, 50.0, 2.0), std::invalid_argument);
}

TEST_F(InventoryOptimizerTest, CalculateReorderPoint_ValidInput) {
  double result = InventoryOptimizer::calculateReorderPoint(10.0, 7.0, 50.0);
  EXPECT_DOUBLE_EQ(result, 120.0); // (10 * 7) + 50 = 120
}

TEST_F(InventoryOptimizerTest, CalculateReorderPoint_ZeroValues) {
  double result = InventoryOptimizer::calculateReorderPoint(0.0, 0.0, 0.0);
  EXPECT_DOUBLE_EQ(result, 0.0);
}

TEST_F(InventoryOptimizerTest, CalculateReorderPoint_NegativeValues) {
  EXPECT_THROW(InventoryOptimizer::calculateReorderPoint(-10.0, 7.0, 50.0),
               std::invalid_argument);
  EXPECT_THROW(InventoryOptimizer::calculateReorderPoint(10.0, -7.0, 50.0),
               std::invalid_argument);
  EXPECT_THROW(InventoryOptimizer::calculateReorderPoint(10.0, 7.0, -50.0),
               std::invalid_argument);
}

TEST_F(InventoryOptimizerTest, IsReorderNeeded_BelowMinimum) {
  bool result = InventoryOptimizer::isReorderNeeded(45.0, 50.0);
  EXPECT_TRUE(result);
}

TEST_F(InventoryOptimizerTest, IsReorderNeeded_AtMinimum) {
  bool result = InventoryOptimizer::isReorderNeeded(50.0, 50.0);
  EXPECT_TRUE(result);
}

TEST_F(InventoryOptimizerTest, IsReorderNeeded_AboveMinimum) {
  bool result = InventoryOptimizer::isReorderNeeded(55.0, 50.0);
  EXPECT_FALSE(result);
}

// ============================================================================
// Main Function
// ============================================================================

int main(int argc, char **argv) {
#ifdef ENABLE_SMART_MAINTENANCE_SUITE_TEST
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
#else
  return 0;
#endif
}
