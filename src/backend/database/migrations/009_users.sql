-- 009_users.sql
-- Kullanıcı hesapları ve rollerini tutan tablo

CREATE TYPE user_role AS ENUM ('admin', 'teknisyen', 'operator');

CREATE TABLE users (
    id SERIAL PRIMARY KEY,
    username VARCHAR(50) UNIQUE NOT NULL,
    password_hash VARCHAR(255) NOT NULL, -- Şifreleri açık tutmuyoruz!
    full_name VARCHAR(100),
    role user_role DEFAULT 'operator',
    last_login TIMESTAMP,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- İlk admin hesabını oluşturalım (Test için şifre: admin123)
-- Not: Normalde burada hashlenmiş şifre olur, biz şimdilik mock olarak ekliyoruz.
INSERT INTO users (username, password_hash, full_name, role) VALUES 
('admin', 'admin123', 'Sistem Yöneticisi', 'admin'),
('teknik1', 'tech123', 'Baş Teknisyen', 'teknisyen'),
('op1', 'op123', 'Makine Operatörü', 'operator');
