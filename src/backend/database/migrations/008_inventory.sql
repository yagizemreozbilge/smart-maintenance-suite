-- 8. Inventory (Stok Yönetimi)
CREATE TABLE IF NOT EXISTS inventory (
    id SERIAL PRIMARY KEY,
    part_name VARCHAR(100) NOT NULL,
    sku VARCHAR(50) UNIQUE NOT NULL,
    quantity INT DEFAULT 0,
    min_stock_level INT DEFAULT 5,
    unit_cost DECIMAL(10, 2) DEFAULT 0.00,
    last_restocked_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- Seed data for inventory
INSERT INTO inventory (part_name, sku, quantity, min_stock_level, unit_cost) VALUES
('Cooling Pump X1', 'PMP-X1-001', 12, 3, 150.00),
('Air Filter Grade A', 'FLT-GR-002', 50, 10, 25.50),
('High Temp Lubricant', 'LUB-HT-003', 8, 5, 45.00),
('Precision Bearing 8mm', 'BRG-08-004', 4, 10, 12.00); -- Low stock scenario
