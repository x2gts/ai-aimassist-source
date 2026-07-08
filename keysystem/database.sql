-- Key System Database Schema
-- Run this in phpMyAdmin or MySQL CLI

CREATE DATABASE IF NOT EXISTS key_system;
USE key_system;

-- License keys table
CREATE TABLE IF NOT EXISTS license_keys (
    id INT AUTO_INCREMENT PRIMARY KEY,
    license_key VARCHAR(25) NOT NULL UNIQUE,
    hwid VARCHAR(255) DEFAULT NULL,
    user_ip VARCHAR(45) DEFAULT NULL,
    is_active TINYINT(1) DEFAULT 0,
    is_banned TINYINT(1) DEFAULT 0,
    subscription_type ENUM('trial', '1day', '7day', '30day', 'lifetime') DEFAULT 'trial',
    activated_at DATETIME DEFAULT NULL,
    expires_at DATETIME DEFAULT NULL,
    last_check DATETIME DEFAULT NULL,
    created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
    created_by VARCHAR(50) DEFAULT 'system',
    INDEX idx_license_key (license_key),
    INDEX idx_hwid (hwid),
    INDEX idx_is_active (is_active)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- Admin users table
CREATE TABLE IF NOT EXISTS admins (
    id INT AUTO_INCREMENT PRIMARY KEY,
    username VARCHAR(50) NOT NULL UNIQUE,
    password_hash VARCHAR(255) NOT NULL,
    api_key VARCHAR(64) NOT NULL,
    created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
    last_login DATETIME DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- Ban list table (for HWID bans)
CREATE TABLE IF NOT EXISTS bans (
    id INT AUTO_INCREMENT PRIMARY KEY,
    hwid VARCHAR(255) NOT NULL,
    reason TEXT DEFAULT NULL,
    banned_at DATETIME DEFAULT CURRENT_TIMESTAMP,
    banned_by VARCHAR(50) DEFAULT 'system',
    INDEX idx_hwid (hwid)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- Activity log
CREATE TABLE IF NOT EXISTS activity_log (
    id INT AUTO_INCREMENT PRIMARY KEY,
    action VARCHAR(50) NOT NULL,
    license_key VARCHAR(25) DEFAULT NULL,
    hwid VARCHAR(255) DEFAULT NULL,
    ip_address VARCHAR(45) DEFAULT NULL,
    details TEXT DEFAULT NULL,
    created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
    INDEX idx_license_key (license_key)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- Insert default admin (password: admin123 - CHANGE THIS!)
-- Password hash generated with: password_hash('admin123', PASSWORD_DEFAULT)
INSERT INTO admins (username, password_hash, api_key) VALUES 
('admin', '$2y$10$YourHashedPasswordHere', 'CHANGE_THIS_TO_RANDOM_API_KEY')
ON DUPLICATE KEY UPDATE username = username;
