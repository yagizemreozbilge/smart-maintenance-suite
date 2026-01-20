-- This file is a template for 002_sensors.sql. Content will be filled by yagiz on 2025-12-29.
-- 2. Sensors
CREATE TABLE IF NOT EXISTS sensors (
    id SERIAL PRIMARY KEY,
    machine_id INT REFERENCES machines(id) ON DELETE CASCADE,
    sensor_type VARCHAR(50) NOT NULL,
    unit VARCHAR(10),
    min_threshold FLOAT,
    max_threshold FLOAT
);