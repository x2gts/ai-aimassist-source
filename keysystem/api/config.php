<?php
// Database configuration
define('DB_HOST', 'sql8.freesqldatabase.com');
define('DB_NAME', 'sql8832571');
define('DB_USER', 'sql8832571');
define('DB_PASS', 'UeSu2T8u1h');

// Security
define('ADMIN_API_KEY', 'x2gts-admin-key-2024');
define('API_SECRET', 'CHANGE_THIS_TO_ANOTHER_RANDOM_SECRET');

// CORS headers for local development
header('Access-Control-Allow-Origin: *');
header('Access-Control-Allow-Methods: GET, POST, OPTIONS');
header('Access-Control-Allow-Headers: Content-Type, X-API-Key');
header('Content-Type: application/json');

if ($_SERVER['REQUEST_METHOD'] === 'OPTIONS') {
    http_response_code(200);
    exit();
}

function getDB() {
    try {
        $pdo = new PDO(
            "mysql:host=" . DB_HOST . ";dbname=" . DB_NAME . ";charset=utf8mb4",
            DB_USER,
            DB_PASS,
            [
                PDO::ATTR_ERRMODE => PDO::ERRMODE_EXCEPTION,
                PDO::ATTR_DEFAULT_FETCH_MODE => PDO::FETCH_ASSOC
            ]
        );
        return $pdo;
    } catch (PDOException $e) {
        http_response_code(500);
        echo json_encode(['success' => false, 'message' => 'Database connection failed']);
        exit();
    }
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

function getHWID() {
    if (php_uname('s') === 'Windows NT') {
        $output = [];
        exec('wmic csproduct get uuid', $output);
        if (isset($output[1])) {
            return trim($output[1]);
        }
    }
    return php_uname('n') . '-' . sys_get_temp_dir();
}
?>
