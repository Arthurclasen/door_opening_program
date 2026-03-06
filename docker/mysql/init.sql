CREATE DATABASE IF NOT EXISTS door_access;

USE door_access;

CREATE TABLE users (
    id INT AUTO_INCREMENT PRIMARY KEY,
    
    name VARCHAR(255) NOT NULL,
    
    email VARCHAR(255) UNIQUE NOT NULL,
    
    password_hash VARCHAR(255) NOT NULL,
    
    private_key TEXT NOT NULL,
    
    team VARCHAR(100) NOT NULL,
    
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE doors (
    id INT AUTO_INCREMENT PRIMARY KEY,

    door_name VARCHAR(50) NOT NULL,

    is_open BOOLEAN NOT NULL
);

INSERT INTO doors (door_name, is_open) VALUES
('RED', FALSE),
('GREEN', FALSE),
('BLUE', FALSE);

REVOKE INSERT, DELETE ON door_access.doors FROM *;

CREATE TABLE door_events (
    id INT AUTO_INCREMENT PRIMARY KEY,
    
    user_id INT NOT NULL,
    
    door_id VARCHAR(100) NOT NULL,
    
    attempted_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    
    success BOOLEAN NOT NULL,
    
    failure_reason ENUM(
        'INVALID_CREDENTIAL',
        'ACCESS_DENIED',
        'EXPIRED_CERTIFICATE',
        'UNKNOWN_USER',
        'SYSTEM_ERROR'
    ) NULL,

    certificate TEXT NULL,

    FOREIGN KEY (user_id) REFERENCES users(id),
    FOREIGN KEY (door_id) REFERENCES doors(id)
);