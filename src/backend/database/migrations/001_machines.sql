-- This file is a template for 001_machines.sql. Content will be filled by yagiz on 2025-12-29.
-- 1. Machines
CREATE TABLE IF NOT EXISTS machines (
    id SERIAL PRIMARY KEY,
    name VARCHAR(100) NOT NULL,
    model VARCHAR(50),
    location VARCHAR(100),
    installation_date DATE,
    status VARCHAR(20) DEFAULT 'operational'
);