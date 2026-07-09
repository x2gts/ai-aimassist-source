<?php
require_once __DIR__ . '/config.php';

if ($_SERVER['REQUEST_METHOD'] !== 'POST') {
    jsonResponse(['success' => false, 'message' => 'Method not allowed'], 405);
}

$input = json_decode(file_get_contents('php://input'), true);
$username = $input['username'] ?? '';

if (empty($username)) {
    jsonResponse(['success' => false, 'message' => 'Missing username'], 400);
}

$db = getDB();

$stmt = $db->prepare("SELECT id FROM users WHERE username = :username");
$stmt->bindValue(':username', $username, SQLITE3_TEXT);
$result = $stmt->execute();
$user = $result->fetchArray(SQLITE3_ASSOC);

if (!$user) {
    jsonResponse(['success' => false, 'message' => 'User not found'], 404);
}

$userId = $user['id'];

$stmt = $db->prepare("SELECT license_key, subscription_type, is_active, is_banned, activated_at, expires_at, created_at FROM license_keys WHERE user_id = :uid ORDER BY created_at DESC");
$stmt->bindValue(':uid', $userId, SQLITE3_INTEGER);
$result = $stmt->execute();

$keys = [];
while ($row = $result->fetchArray(SQLITE3_ASSOC)) {
    $keys[] = $row;
}

jsonResponse([
    'success' => true,
    'keys' => $keys
]);
?>
