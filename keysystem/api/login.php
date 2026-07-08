<?php
require_once __DIR__ . '/config.php';

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

$stmt = $db->prepare("SELECT * FROM admins WHERE username = :username");
$stmt->bindValue(':username', $username, SQLITE3_TEXT);
$result = $stmt->execute();
$admin = $result->fetchArray(SQLITE3_ASSOC);

if (!$admin || !password_verify($password, $admin['password_hash'])) {
    jsonResponse(['success' => false, 'message' => 'Invalid credentials'], 401);
}

$stmt = $db->prepare("UPDATE admins SET last_login = datetime('now') WHERE id = :id");
$stmt->bindValue(':id', $admin['id'], SQLITE3_INTEGER);
$stmt->execute();

jsonResponse([
    'success' => true,
    'message' => 'Login successful',
    'api_key' => $admin['api_key'],
    'username' => $admin['username']
]);
?>
