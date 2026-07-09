<?php
// Database configuration - SQLite (no external DB needed)
define('DB_PATH', __DIR__ . '/database.sqlite');

// Security
if (!defined('ADMIN_API_KEY')) {
    define('ADMIN_API_KEY', 'x2gts-admin-key-2024');
    define('API_SECRET', 'CHANGE_THIS_TO_ANOTHER_RANDOM_SECRET');
}

// CORS headers
if (!headers_sent()) {
    header('Access-Control-Allow-Origin: *');
    header('Access-Control-Allow-Methods: GET, POST, OPTIONS');
    header('Access-Control-Allow-Headers: Content-Type, X-API-Key');
    header('Content-Type: application/json');
}

if ($_SERVER['REQUEST_METHOD'] === 'OPTIONS') {
    http_response_code(200);
    exit();
}

function getDB() {
    static $db = null;
    if ($db === null) {
        $db = new SQLite3(DB_PATH);
        $db->busyTimeout(5000);
        $db->exec('PRAGMA journal_mode = WAL');
        
        // Create tables if they don't exist
        $db->exec("CREATE TABLE IF NOT EXISTS license_keys (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            license_key TEXT NOT NULL UNIQUE,
            hwid TEXT DEFAULT NULL,
            user_ip TEXT DEFAULT NULL,
            user_id INTEGER DEFAULT NULL,
            is_active INTEGER DEFAULT 0,
            is_banned INTEGER DEFAULT 0,
            subscription_type TEXT DEFAULT 'trial' CHECK(subscription_type IN ('trial','1day','7day','30day','lifetime')),
            activated_at TEXT DEFAULT NULL,
            expires_at TEXT DEFAULT NULL,
            last_check TEXT DEFAULT NULL,
            created_at TEXT DEFAULT (datetime('now')),
            created_by TEXT DEFAULT 'system',
            FOREIGN KEY (user_id) REFERENCES users(id)
        )");
        
        $db->exec("CREATE TABLE IF NOT EXISTS admins (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            username TEXT NOT NULL UNIQUE,
            password_hash TEXT NOT NULL,
            api_key TEXT NOT NULL,
            created_at TEXT DEFAULT (datetime('now')),
            last_login TEXT DEFAULT NULL
        )");
        
        $db->exec("CREATE TABLE IF NOT EXISTS bans (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            license_key TEXT NOT NULL,
            reason TEXT DEFAULT NULL,
            banned_at TEXT DEFAULT (datetime('now')),
            banned_by TEXT DEFAULT 'system'
        )");
        
        $db->exec("CREATE TABLE IF NOT EXISTS activity_log (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            action TEXT NOT NULL,
            license_key TEXT DEFAULT NULL,
            hwid TEXT DEFAULT NULL,
            ip_address TEXT DEFAULT NULL,
            details TEXT DEFAULT NULL,
            created_at TEXT DEFAULT (datetime('now'))
        )");
        
        $db->exec("CREATE TABLE IF NOT EXISTS users (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            username TEXT NOT NULL UNIQUE,
            password_hash TEXT NOT NULL,
            email TEXT DEFAULT NULL,
            created_at TEXT DEFAULT (datetime('now')),
            last_login TEXT DEFAULT NULL
        )");
        
        // Migration: add user_id column if missing
        $hasUserId = false;
        $cols = $db->query("PRAGMA table_info(license_keys)");
        while ($col = $cols->fetchArray(SQLITE3_ASSOC)) {
            if ($col['name'] === 'user_id') { $hasUserId = true; break; }
        }
        if (!$hasUserId) {
            $db->exec("ALTER TABLE license_keys ADD COLUMN user_id INTEGER DEFAULT NULL");
        }

        // Insert default admin if not exists
        $result = $db->querySingle("SELECT COUNT(*) FROM admins");
        if ($result == 0) {
            $hash = password_hash('password', PASSWORD_DEFAULT);
            $db->exec("INSERT INTO admins (username, password_hash, api_key) VALUES ('admin', '$hash', 'x2gts-admin-key-2024')");
        }
    }
    return $db;
}

function jsonResponse($data, $code = 200) {
    http_response_code($code);
    echo json_encode($data);
    exit();
}

function verifyApiKey() {
    $apiKey = $_SERVER['HTTP_X_API_KEY'] ?? '';
    if ($apiKey !== ADMIN_API_KEY) {
        jsonResponse(['success' => false, 'message' => 'Invalid API key'], 401);
    }
}

function generateKey($prefix = 'AIM') {
    $chars = 'ABCDEFGHJKLMNPQRSTUVWXYZ23456789';
    $key = $prefix . '-';
    for ($i = 0; $i < 4; $i++) {
        for ($j = 0; $j < 4; $j++) {
            $key .= $chars[random_int(0, strlen($chars) - 1)];
        }
        if ($i < 3) $key .= '-';
    }
    return $key;
}
?>
