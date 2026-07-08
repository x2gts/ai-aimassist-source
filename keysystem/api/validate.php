<?php
require_once __DIR__ . '/config.php';

$input = json_decode(file_get_contents('php://input'), true);

$licenseKey = $input['key'] ?? '';
$hwid = $input['hwid'] ?? '';
$ip = $_SERVER['REMOTE_ADDR'] ?? 'unknown';

if (empty($licenseKey) || empty($hwid)) {
    jsonResponse(['success' => false, 'message' => 'Missing key or hwid'], 400);
}

$db = getDB();

$stmt = $db->prepare("SELECT * FROM license_keys WHERE license_key = :key");
bindValue($stmt, ':key', $licenseKey);
$result = $stmt->execute();
$key = $result->fetchArray(SQLITE3_ASSOC);

if (!$key) {
    jsonResponse(['success' => false, 'message' => 'Invalid license key', 'code' => 'INVALID_KEY']);
}

if ($key['is_banned']) {
    jsonResponse(['success' => false, 'message' => 'This key has been banned', 'code' => 'KEY_BANNED']);
}

$stmt = $db->prepare("SELECT id FROM bans WHERE hwid = :hwid");
bindValue($stmt, ':hwid', $hwid);
$result = $stmt->execute();
if ($result->fetchArray()) {
    jsonResponse(['success' => false, 'message' => 'This hardware has been banned', 'code' => 'HWID_BANNED']);
}

if (!$key['is_active']) {
    $expiresAt = calculateExpiry($key['subscription_type']);
    $stmt = $db->prepare("UPDATE license_keys SET hwid = :hwid, user_ip = :ip, is_active = 1, activated_at = datetime('now'), expires_at = :expires WHERE license_key = :key");
    bindValue($stmt, ':hwid', $hwid);
    bindValue($stmt, ':ip', $ip);
    bindValue($stmt, ':expires', $expiresAt);
    bindValue($stmt, ':key', $licenseKey);
    $stmt->execute();
    
    jsonResponse([
        'success' => true, 
        'message' => 'Key activated successfully',
        'subscription' => $key['subscription_type'],
        'expires' => $expiresAt
    ]);
}

if ($key['hwid'] !== $hwid) {
    jsonResponse(['success' => false, 'message' => 'Key is bound to different hardware', 'code' => 'HWID_MISMATCH']);
}

if ($key['expires_at'] && strtotime($key['expires_at']) < time()) {
    jsonResponse(['success' => false, 'message' => 'Key has expired', 'code' => 'KEY_EXPIRED', 'expires' => $key['expires_at']]);
}

$stmt = $db->prepare("UPDATE license_keys SET last_check = datetime('now') WHERE license_key = :key");
bindValue($stmt, ':key', $licenseKey);
$stmt->execute();

jsonResponse([
    'success' => true, 
    'message' => 'Key is valid',
    'subscription' => $key['subscription_type'],
    'expires' => $key['expires_at']
]);

function calculateExpiry($type) {
    switch ($type) {
        case 'trial': return date('Y-m-d H:i:s', strtotime('+24 hours'));
        case '1day': return date('Y-m-d H:i:s', strtotime('+1 day'));
        case '7day': return date('Y-m-d H:i:s', strtotime('+7 days'));
        case '30day': return date('Y-m-d H:i:s', strtotime('+30 days'));
        case 'lifetime': return null;
        default: return date('Y-m-d H:i:s', strtotime('+24 hours'));
    }
}

function bindValue($stmt, $key, $value) {
    if (is_int($value)) {
        $stmt->bindValue($key, $value, SQLITE3_INTEGER);
    } else {
        $stmt->bindValue($key, $value, SQLITE3_TEXT);
    }
}
?>
