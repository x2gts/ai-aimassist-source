<?php
// API Entry Point
header('Content-Type: application/json');

// Simple router
$method = $_SERVER['REQUEST_METHOD'];
$action = $_GET['action'] ?? '';

// Include config
require_once __DIR__ . '/config.php';

// Route requests
if ($action === 'validate' && $method === 'POST') {
    require __DIR__ . '/validate.php';
} elseif ($action === 'login' && $method === 'POST') {
    require __DIR__ . '/login.php';
} elseif ($action === 'register' && $method === 'POST') {
    require __DIR__ . '/register.php';
} elseif ($action === 'user_keys' && $method === 'POST') {
    require __DIR__ . '/user_keys.php';
} elseif (strpos($action, 'admin') === 0 || in_array($action, ['generate', 'list', 'revoke', 'ban', 'unban', 'stats', 'log'])) {
    require __DIR__ . '/admin.php';
} else {
    echo json_encode(['success' => false, 'message' => 'Invalid endpoint']);
}
