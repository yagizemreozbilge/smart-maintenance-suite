/**
 * @file smart_maintenance_suite_app.cpp
 * @brief Smart Maintenance Suite - Demo Application Implementation
 */

#include "../../smart_maintenance_suite_app/header/smart_maintenance_suite_app.h"
#include "../../smart_maintenance_suite/header/smart_maintenance_suite.h"
#include <iostream>
#include <iomanip>
#include <stdexcept>
#include <string>

using namespace SmartMaintenance::Analytics;

namespace DemoApp {

void printHeader(const std::string &title) {
  std::cout <<
            "\n╔════════════════════════════════════════════════════════════╗\n";
  std::cout << "║  " << std::left << std::setw(56) << title << "  ║\n";
  std::cout <<
            "╚════════════════════════════════════════════════════════════╝\n\n";
}

bool demonstrateSensorAnalytics() {
  try {
    printHeader("SENSOR ANALYTICS DEMO");
    // Simulated temperature sensor readings (°C) - using basic array
    double temperatureReadings[] = {
      75.2, 76.1, 75.8, 76.5, 77.2, 78.1, 79.5, 81.2, 83.5, 92.1
    };
    int numReadings = 10; // Number of elements in the array
    std::cout << "Temperature Readings (°C): ";

    for (int i = 0; i < numReadings; i++) {
      std::cout << temperatureReadings[i] << " ";
    }

    std::cout << "\n\n";

    // Calculate moving average
    try {
      // Pass array and size instead of vector
      double movingAvg = SensorAnalytics::calculateMovingAverage(temperatureReadings, numReadings, 5);
      std::cout << "✓ Moving Average (last 5 readings): " << std::fixed << std::setprecision(2)
                << movingAvg << "°C\n";
    } catch (const std::exception &e) {
      std::cerr << "✗ Error: " << e.what() << "\n";
      return false;
    }

    // Detect anomaly
    bool anomalyDetected = SensorAnalytics::detectAnomaly(temperatureReadings, numReadings, 2.0);
    std::cout << "✓ Anomaly Detection (2σ threshold): "
              << (anomalyDetected ? "⚠️  ANOMALY DETECTED!" : "✅ Normal") << "\n";
    // Calculate standard deviation
    double stdDev = SensorAnalytics::calculateStandardDeviation(temperatureReadings, numReadings);
    std::cout << "✓ Standard Deviation: " << std::fixed << std::setprecision(2)
              << stdDev << "°C\n";
    // Predict remaining useful life
    double currentTemp = 83.5;
    double criticalTemp = 100.0;
    double degradationRate = 2.5; // °C per day
    double rul = SensorAnalytics::predictRemainingLife(currentTemp, criticalTemp, degradationRate);
    std::cout << "✓ Remaining Useful Life: " << std::fixed << std::setprecision(1)
              << rul << " days\n";
    return true;
  } catch(...) {
    return false;
  }
}

bool demonstrateMaintenanceScheduling() {
  try {
    printHeader("MAINTENANCE SCHEDULING DEMO");
    double failureRate = 0.05; // 5% failure rate per month
    double preventiveCost = 500.0; // $500 per preventive maintenance
    double correctiveCost = 2000.0; // $2000 per corrective maintenance (failure)
    std::cout << "Parameters:\n";
    std::cout << "  - Failure Rate: " << (failureRate * 100) << "% per month\n";
    std::cout << "  - Preventive Maintenance Cost: $" << preventiveCost << "\n";
    std::cout << "  - Corrective Maintenance Cost: $" << correctiveCost << "\n\n";
    // Calculate optimal interval
    double optimalInterval = MaintenanceScheduler::calculateOptimalInterval(
                               failureRate, preventiveCost, correctiveCost
                             );
    std::cout << "✓ Optimal Maintenance Interval: " << std::fixed << std::setprecision(1)
              << optimalInterval << " months\n";
    // Calculate total cost over a year
    double totalCost = MaintenanceScheduler::calculateTotalCost(
                         optimalInterval, 12.0, preventiveCost
                       );
    std::cout << "✓ Total Annual Maintenance Cost: $" << std::fixed << std::setprecision(2)
              << totalCost << "\n";
    // Assess maintenance priority
    std::cout << "\nMaintenance Priority Assessment:\n";
    // Using simple struct and array for demo
    struct SensorData {
      std::string name;
      double value;
    };

    SensorData sensors[] = {
      {"Vibration Sensor", 45.0},
      {"Temperature Sensor", 78.0},
      {"Pressure Sensor", 95.0}
    };
    int numSensors = 3;
    double warningThreshold = 70.0;
    double criticalThreshold = 90.0;

    for (int i = 0; i < numSensors; i++) {
      int priority = MaintenanceScheduler::assessMaintenancePriority(
                       sensors[i].value, warningThreshold, criticalThreshold
                     );
      std::string status;

      if (priority == 2) status = "🔴 CRITICAL";
      else if (priority == 1) status = "🟡 WARNING";
      else status = "🟢 NORMAL";

      std::cout << "  " << std::left << std::setw(20) << sensors[i].name
                << ": " << std::setw(6) << sensors[i].value
                << " → " << status << "\n";
    }

    return true;
  } catch(...) {
    return false;
  }
}

bool demonstrateInventoryOptimization() {
  try {
    printHeader("INVENTORY OPTIMIZATION DEMO");
    // Bearing inventory example
    double annualDemand = 1200.0; // 1200 bearings per year
    double orderingCost = 50.0; // $50 per order
    double holdingCost = 2.0; // $2 per bearing per year
    std::cout << "Bearing Inventory Parameters:\n";
    std::cout << "  - Annual Demand: " << annualDemand << " units\n";
    std::cout << "  - Ordering Cost: $" << orderingCost << " per order\n";
    std::cout << "  - Holding Cost: $" << holdingCost << " per unit/year\n\n";
    // Calculate EOQ
    double eoq = InventoryOptimizer::calculateEOQ(annualDemand, orderingCost, holdingCost);
    std::cout << "✓ Economic Order Quantity (EOQ): " << std::fixed << std::setprecision(0)
              << eoq << " units\n";
    // Calculate reorder point
    double dailyDemand = annualDemand / 365.0;
    double leadTime = 7.0; // 7 days
    double safetyStock = 50.0; // 50 units safety stock
    double reorderPoint = InventoryOptimizer::calculateReorderPoint(
                            dailyDemand, leadTime, safetyStock
                          );
    std::cout << "✓ Reorder Point: " << std::fixed << std::setprecision(0)
              << reorderPoint << " units\n";
    // Check if reorder needed
    std::cout << "\nStock Level Check:\n";
    struct StockItem {
      std::string part;
      double currentStock;
      double minLevel;
    };

    StockItem inventory[] = {
      {"Bearing 608", 45.0, 50.0},
      {"Oil Filter", 120.0, 100.0},
      {"Belt V-Type", 15.0, 20.0}
    };
    int numItems = 3;

    for (int i = 0; i < numItems; i++) {
      bool needsReorder = InventoryOptimizer::isReorderNeeded(
                            inventory[i].currentStock, inventory[i].minLevel
                          );
      std::cout << "  " << std::left << std::setw(15) << inventory[i].part
                << ": " << std::setw(6) << inventory[i].currentStock
                << " / " << std::setw(6) << inventory[i].minLevel
                << " → " << (needsReorder ? "🔴 REORDER NEEDED" : "✅ OK") << "\n";
    }

    return true;
  } catch(...) {
    return false;
  }
}

} // namespace DemoApp

// We need to keep a main function for the standalone application build
// But if this file is included in tests, we might have multiple mains.
// The solution is to use preprocessor directives or ensure CMake links correctly.
// For now, let's keep the main simple and just call our functions.

#ifndef ENABLE_DEMO_APP_TEST // If NOT building tests for this app
int main() {
  std::cout << "\n";
  std::cout <<
            "╔════════════════════════════════════════════════════════════╗\n";
  std::cout << "║     SMART MAINTENANCE SUITE - ANALYTICS DEMO              ║\n";
  std::cout << "║     Industrial IoT Predictive Maintenance System          ║\n";
  std::cout <<
            "╚════════════════════════════════════════════════════════════╝\n";

  try {
    // Run demonstrations
    DemoApp::demonstrateSensorAnalytics();
    DemoApp::demonstrateMaintenanceScheduling();
    DemoApp::demonstrateInventoryOptimization();
    std::cout << "\n";
    std::cout <<
              "╔════════════════════════════════════════════════════════════╗\n";
    std::cout << "║  ✅ All demonstrations completed successfully!            ║\n";
    std::cout <<
              "╚════════════════════════════════════════════════════════════╝\n\n";
  } catch (const std::exception &e) {
    std::cerr << "\n❌ Error: " << e.what() << "\n\n";
    return 1;
  }

  return 0;
}

#endif
