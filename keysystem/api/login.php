<?php
require_once 'config.php';

if ($_SERVER['REQUEST_METHOD'] !== 'POST') {
    jsonResponse(['success' => false, 'message' => 'Method not allowed'], 405);
}

$input = json_decode(file_get_contents('php://input'), true);
$username = $input['username'] ?? '';
$password = $input['password'] ?? '';

if (empty($username) || empty($password)) {
    jsonResponse(['success' => false, 'message' => 'Missing credentials'], 400);
}

$db = getDB();
$stmt = $db->prepare("SELECT * FROM admins WHERE username = ?");
$stmt->execute([$username]);
$admin = $stmt->fetch();

if (!$admin || !password_verify($password, $admin['password_hash'])) {
    jsonResponse(['success' => false, 'message' => 'Invalid credentials'], 401);
}

// Update last login
$stmt = $db->prepare("UPDATE admins SET last_login = NOW() WHERE id = ?");
$stmt->execute([$admin['id']]);

// Generate session token
$token = bin2hex(random_bytes(32));

// Store token (in production, use Redis or database)
// For simplicity, we'll use the API key as the token
jsonResponse([
    'success' => true,
    'message' => 'Login successful',
    'api_key' => $admin['api_key'],
    'username' => $admin['username']
]);
?>
