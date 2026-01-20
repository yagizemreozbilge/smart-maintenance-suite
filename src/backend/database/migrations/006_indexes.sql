-- Indexes for performance
CREATE INDEX idx_sensor_data_timestamp 
ON sensor_data(timestamp);
CREATE INDEX idx_sensor_data_id 
ON sensor_data(sensor_id);