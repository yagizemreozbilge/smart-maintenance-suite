-- This file is a template for 004_maintenance_logs.sql. Content will be filled by yagiz on 2025-12-29.-- 4. Maintenance Logs

-- 4. Maintenance Logs
CREATE TABLE IF NOT EXISTS maintenance_logs (
    id SERIAL PRIMARY KEY,
    machine_id INT REFERENCES machines(id) ON DELETE CASCADE,
    ts TIMESTAMP WITH TIME ZONE DEFAULT CURRENT_TIMESTAMP,
    performer VARCHAR(100),
    description TEXT,
    parts_replaced TEXT,
    cost DECIMAL(10, 2)
);
