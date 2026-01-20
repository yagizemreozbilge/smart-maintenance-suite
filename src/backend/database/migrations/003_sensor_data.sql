-- This file is a template for 003_sensor_data.sql. Content will be filled by yagiz on 2025-12-29.
-- 3. Sensor Data
CREATE TABLE IF NOT EXISTS sensor_data (
    id BIGSERIAL PRIMARY KEY,
    sensor_id INT REFERENCES sensors(id) ON DELETE CASCADE,
    timestamp TIMESTAMP WITH TIME ZONE DEFAULT CURRENT_TIMESTAMP,
    value FLOAT NOT NULL
);