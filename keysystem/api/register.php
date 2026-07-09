<?php
require_once __DIR__ . '/config.php';

if ($_SERVER['REQUEST_METHOD'] !== 'POST') {
    jsonResponse(['success' => false, 'message' => 'Method not allowed'], 405);
}

$input = json_decode(file_get_contents('php://input'), true);
$username = trim($input['username'] ?? '');
$password = $input['password'] ?? '';
$email = trim($input['email'] ?? '');

if (empty($username) || empty($password)) {
    jsonResponse(['success' => false, 'message' => 'Username and password required'], 400);
}

if (strlen($username) < 3) {
    jsonResponse(['success' => false, 'message' => 'Username must be at least 3 characters']);
}

if (strlen($password) < 4) {
    jsonResponse(['success' => false, 'message' => 'Password must be at least 4 characters']);
}

if ($username === 'admin') {
    jsonResponse(['success' => false, 'message' => 'This username is reserved']);
}

$db = getDB();

$stmt = $db->prepare("SELECT COUNT(*) FROM users WHERE username = :username");
$stmt->bindValue(':username', $username, SQLITE3_TEXT);
$count = $stmt->execute()->fetchArray(SQLITE3_NUM)[0];

if ($count > 0) {
    jsonResponse(['success' => false, 'message' => 'Username already exists']);
}

$hash = password_hash($password, PASSWORD_DEFAULT);

$stmt = $db->prepare("INSERT INTO users (username, password_hash, email) VALUES (:username, :hash, :email)");
$stmt->bindValue(':username', $username, SQLITE3_TEXT);
$stmt->bindValue(':hash', $hash, SQLITE3_TEXT);
$stmt->bindValue(':email', $email, SQLITE3_TEXT);
$stmt->execute();

$userId = $db->lastInsertRowID();

$licenseKey = generateKey('AIM');
$stmt = $db->prepare("INSERT INTO license_keys (license_key, user_id, subscription_type) VALUES (:key, :uid, 'trial')");
$stmt->bindValue(':key', $licenseKey, SQLITE3_TEXT);
$stmt->bindValue(':uid', $userId, SQLITE3_INTEGER);
$stmt->execute();

jsonResponse([
    'success' => true,
    'message' => 'Account created successfully',
    'username' => $username,
    'license_key' => $licenseKey
]);
