-- This file is a template for 005_alerts.sql. Content will be filled by yagiz on 2025-12-29.
-- 5. Alerts
CREATE TABLE IF NOT EXISTS alerts (
    id SERIAL PRIMARY KEY,
    sensor_id INT REFERENCES sensors(id) ON DELETE CASCADE,
    alert_time TIMESTAMP WITH TIME ZONE DEFAULT CURRENT_TIMESTAMP,
    severity VARCHAR(20),
    message TEXT,
    resolved BOOLEAN DEFAULT FALSE
);