-- Migration: 007_seed_data
-- Description: Seeding the database with initial sample data

-- 1. Machines
INSERT INTO machines (name, model, location, installation_date, status) VALUES
('CNC Lathe-01', 'Mazak-X', 'Block A / Floor 1', '2023-05-15', 'operational'),
('Robot Arm-04', 'Fanuc-R2000', 'Block B / Assembly Line', '2024-01-10', 'operational'),
('Heat Treatment Furnace', 'Therm-500', 'Block C', '2022-11-20', 'maintenance');

-- 2. Sensors
INSERT INTO sensors (machine_id, sensor_type, unit, min_threshold, max_threshold) VALUES
(1, 'Temperature', 'Celsius', 20.0, 85.0),
(1, 'Vibration', 'Hz', 0.0, 50.0),
(2, 'Pressure', 'Bar', 2.0, 10.0),
(3, 'Temperature', 'Celsius', 100.0, 550.0);

-- 3. Sensor Data
INSERT INTO sensor_data (sensor_id, value, timestamp) VALUES
(1, 45.5, NOW() - INTERVAL '10 minutes'),
(1, 46.2, NOW() - INTERVAL '5 minutes'),
(2, 12.5, NOW() - INTERVAL '8 minutes'),
(3, 6.8, NOW() - INTERVAL '2 minutes');

-- 4. Maintenance Logs
INSERT INTO maintenance_logs (machine_id, performer, description, parts_replaced, cost) VALUES
(1, 'Alex Martinez', 'Routine oil change and filter check.', 'Oil Filter', 150.00);

-- 5. Alerts
INSERT INTO alerts (sensor_id, severity, message) VALUES
(1, 'Medium', 'Temperature is approaching the critical threshold (85C)!');
