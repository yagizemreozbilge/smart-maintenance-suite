/**
 * @file smart_maintenance_suite.h
 *
 * @brief Smart Maintenance Suite - Core Utility Library
 * @details Provides sensor data processing, alert threshold calculations,
 *          and predictive maintenance utilities for industrial IoT systems.
 */

#ifndef SMART_MAINTENANCE_SUITE_H
#define SMART_MAINTENANCE_SUITE_H

#include "../../utility/header/commonTypes.h"
#include <string>

namespace SmartMaintenance {
namespace Analytics {

/**
 * @class SensorAnalytics
 * @brief Provides sensor data analysis and predictive maintenance calculations
 */
class SensorAnalytics {
 public:
  /**
   * Calculates the moving average of sensor readings
   * @param readings Array of sensor readings
   * @param size Size of the readings array
   * @param windowSize Size of the moving average window
   * @return Moving average value
   */
  static double calculateMovingAverage(const double *readings, int size, int windowSize);

  /**
   * Detects anomalies in sensor data using standard deviation
   * @param readings Array of sensor readings
   * @param size Size of the readings array
   * @param threshold Number of standard deviations for anomaly detection (default: 2.0)
   * @return true if anomaly detected, false otherwise
   */
  static bool detectAnomaly(const double *readings, int size, double threshold = 2.0);

  /**
   * Calculates the standard deviation of sensor readings
   * @param readings Array of sensor readings
   * @param size Size of the readings array
   * @return Standard deviation value
   */
  static double calculateStandardDeviation(const double *readings, int size);

  /**
   * Predicts remaining useful life (RUL) based on degradation rate
   * @param currentValue Current sensor value
   * @param thresholdValue Critical threshold value
   * @param degradationRate Rate of degradation per time unit
   * @return Estimated remaining useful life in time units
   */
  static double predictRemainingLife(double currentValue, double thresholdValue, double degradationRate);
};

/**
 * @class MaintenanceScheduler
 * @brief Provides maintenance scheduling and cost optimization utilities
 */
class MaintenanceScheduler {
 public:
  /**
   * Calculates optimal maintenance interval based on failure rate
   * @param failureRate Failure rate (failures per time unit)
   * @param maintenanceCost Cost of preventive maintenance
   * @param failureCost Cost of corrective maintenance (failure)
   * @return Optimal maintenance interval in time units
   */
  static double calculateOptimalInterval(double failureRate, double maintenanceCost, double failureCost);

  /**
   * Calculates total maintenance cost over a period
   * @param interval Maintenance interval
   * @param period Total period to calculate cost for
   * @param costPerMaintenance Cost per maintenance event
   * @return Total maintenance cost
   */
  static double calculateTotalCost(double interval, double period, double costPerMaintenance);

  /**
   * Determines if immediate maintenance is required
   * @param sensorValue Current sensor value
   * @param warningThreshold Warning threshold
   * @param criticalThreshold Critical threshold
   * @return 0 (normal), 1 (warning), 2 (critical)
   */
  static int assessMaintenancePriority(double sensorValue, double warningThreshold, double criticalThreshold);
};

/**
 * @class InventoryOptimizer
 * @brief Provides inventory management and optimization utilities
 */
class InventoryOptimizer {
 public:
  /**
   * Calculates Economic Order Quantity (EOQ)
   * @param annualDemand Annual demand for the part
   * @param orderingCost Cost per order
   * @param holdingCost Annual holding cost per unit
   * @return Optimal order quantity
   */
  static double calculateEOQ(double annualDemand, double orderingCost, double holdingCost);

  /**
   * Calculates reorder point for inventory
   * @param dailyDemand Average daily demand
   * @param leadTime Lead time in days
   * @param safetyStock Safety stock quantity
   * @return Reorder point quantity
   */
  static double calculateReorderPoint(double dailyDemand, double leadTime, double safetyStock);

  /**
   * Determines if stock is below minimum level
   * @param currentStock Current stock quantity
   * @param minStockLevel Minimum stock level
   * @return true if reorder needed, false otherwise
   */
  static bool isReorderNeeded(double currentStock, double minStockLevel);
};

} // namespace Analytics
} // namespace SmartMaintenance

#endif // SMART_MAINTENANCE_SUITE_H
