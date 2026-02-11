/**
 * @file smart_maintenance_suite_app.h
 * @brief Header file for Smart Maintenance Suite Demo Application
 */

#ifndef SMART_MAINTENANCE_SUITE_APP_H
#define SMART_MAINTENANCE_SUITE_APP_H

#include <string>

/**
 * @namespace DemoApp
 * @brief Contains demonstration functions for the Smart Maintenance Suite
 */
namespace DemoApp {

/**
 * Demonstrates Sensor Analytics features.
 * @return true if demonstration runs successfully without exception.
 */
bool demonstrateSensorAnalytics();

/**
 * Demonstrates Maintenance Scheduling features.
 * @return true if demonstration runs successfully without exception.
 */
bool demonstrateMaintenanceScheduling();

/**
 * Demonstrates Inventory Optimization features.
 * @return true if demonstration runs successfully without exception.
 */
bool demonstrateInventoryOptimization();

/**
 * Helper function to print headers.
 * @param title Title to print
 */
void printHeader(const std::string &title);

} // namespace DemoApp

#endif // SMART_MAINTENANCE_SUITE_APP_H
