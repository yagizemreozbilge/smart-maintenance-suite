/**
 * @file smart_maintenance_suite.cpp
 * @brief Smart Maintenance Suite - Demo Application Implementation
 */
#include "smart_maintenance_suite.h"
#include <cmath>
#include <stdexcept>
#include <iostream>



using namespace SmartMaintenance::Analytics;

// ============================================================================
// SensorAnalytics Implementation
// ============================================================================

double SensorAnalytics::calculateMovingAverage(const double *readings, int size, int windowSize) {
  if (readings == nullptr || size == 0) {
    throw std::invalid_argument("Readings array cannot be empty");
  }

  if (windowSize <= 0 || windowSize > size) {
    throw std::invalid_argument("Invalid window size");
  }

  // Calculate average of last 'windowSize' readings
  double sum = 0.0;

  // Starting index: size - windowSize
  // Example: size=10, window=3 -> start index 7 (readings[7], [8], [9])
  for (int i = size - windowSize; i < size; i++) {
    sum += readings[i];
  }

  return sum / windowSize;
}

bool SensorAnalytics::detectAnomaly(const double *readings, int size, double threshold) {
  if (readings == nullptr || size < 2) {
    return false; // Not enough data to detect anomaly
  }

  // Calculate mean
  double sum = 0.0;

  for (int i = 0; i < size; i++) {
    sum += readings[i];
  }

  double mean = sum / size;
  // Calculate standard deviation manually
  double varianceSum = 0.0;

  for (int i = 0; i < size; i++) {
    varianceSum += (readings[i] - mean) * (readings[i] - mean);
  }

  double variance = varianceSum / size;
  double stdDev = std::sqrt(variance);

  // Check if stdDev is non-zero to avoid division by zero
  if (stdDev == 0.0) return false;

  // Check if the last reading is an anomaly
  double lastReading = readings[size - 1];
  double zScore = std::abs((lastReading - mean) / stdDev);
  return zScore > threshold;
}

double SensorAnalytics::calculateStandardDeviation(const double *readings, int size) {
  if (readings == nullptr || size == 0) {
    throw std::invalid_argument("Readings array cannot be empty");
  }

  if (size == 1) {
    return 0.0;
  }

  // Calculate mean
  double sum = 0.0;

  for (int i = 0; i < size; i++) {
    sum += readings[i];
  }

  double mean = sum / size;
  // Calculate variance
  double varianceSum = 0.0;

  for (int i = 0; i < size; i++) {
    varianceSum += (readings[i] - mean) * (readings[i] - mean);
  }

  double variance = varianceSum / size;
  return std::sqrt(variance);
}

double SensorAnalytics::predictRemainingLife(double currentValue, double thresholdValue, double degradationRate) {
  if (degradationRate <= 0) {
    throw std::invalid_argument("Degradation rate must be positive");
  }

  if (currentValue >= thresholdValue) {
    return 0.0; // Already at or beyond threshold
  }

  // RUL = (threshold - current) / degradation_rate
  return (thresholdValue - currentValue) / degradationRate;
}

// ============================================================================
// MaintenanceScheduler Implementation
// ============================================================================

double MaintenanceScheduler::calculateOptimalInterval(double failureRate, double maintenanceCost, double failureCost) {
  if (failureRate <= 0) {
    throw std::invalid_argument("Failure rate must be positive");
  }

  if (maintenanceCost < 0 || failureCost < 0) {
    throw std::invalid_argument("Costs cannot be negative");
  }

  // Optimal interval using reliability theory
  // T_opt = sqrt(2 * C_pm / (lambda * C_cm))
  double costRatio = (2.0 * maintenanceCost) / (failureRate *failureCost);
  return std::sqrt(costRatio);
}

double MaintenanceScheduler::calculateTotalCost(double interval, double period, double costPerMaintenance) {
  if (interval <= 0) {
    throw std::invalid_argument("Interval must be positive");
  }

  if (period < 0 || costPerMaintenance < 0) {
    throw std::invalid_argument("Period and cost cannot be negative");
  }

  // Number of maintenance events = period / interval
  double numMaintenances = period / interval;
  return numMaintenances *costPerMaintenance;
}

int MaintenanceScheduler::assessMaintenancePriority(double sensorValue, double warningThreshold, double criticalThreshold) {
  if (sensorValue >= criticalThreshold) {
    return 2; // Critical - immediate maintenance required
  } else if (sensorValue >= warningThreshold) {
    return 1; // Warning - schedule maintenance soon
  } else {
    return 0; // Normal - no immediate action needed
  }
}

// ============================================================================
// InventoryOptimizer Implementation
// ============================================================================

double InventoryOptimizer::calculateEOQ(double annualDemand, double orderingCost, double holdingCost) {
  if (annualDemand <= 0 || orderingCost <= 0 || holdingCost <= 0) {
    throw std::invalid_argument("All parameters must be positive");
  }

  // EOQ = sqrt((2 * D * S) / H)
  return std::sqrt((2.0 * annualDemand *orderingCost) / holdingCost);
}

double InventoryOptimizer::calculateReorderPoint(double dailyDemand, double leadTime, double safetyStock) {
  if (dailyDemand < 0 || leadTime < 0 || safetyStock < 0) {
    throw std::invalid_argument("All parameters must be non-negative");
  }

  // ROP = (daily demand * lead time) + safety stock
  return (dailyDemand *leadTime) + safetyStock;
}

bool InventoryOptimizer::isReorderNeeded(double currentStock, double minStockLevel) {
  return currentStock <= minStockLevel;
}
