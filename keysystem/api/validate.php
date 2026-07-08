<?php
require_once 'config.php';

// Validate a license key
function validateKey($licenseKey, $hwid, $ip) {
    $db = getDB();
    
    // Check if key exists
    $stmt = $db->prepare("SELECT * FROM keys WHERE license_key = ?");
    $stmt->execute([$licenseKey]);
    $key = $stmt->fetch();
    
    if (!$key) {
        return ['success' => false, 'message' => 'Invalid license key', 'code' => 'INVALID_KEY'];
    }
    
    // Check if banned
    if ($key['is_banned']) {
        return ['success' => false, 'message' => 'This key has been banned', 'code' => 'KEY_BANNED'];
    }
    
    // Check if HWID is banned
    $stmt = $db->prepare("SELECT id FROM bans WHERE hwid = ?");
    $stmt->execute([$hwid]);
    if ($stmt->fetch()) {
        return ['success' => false, 'message' => 'This hardware has been banned', 'code' => 'HWID_BANNED'];
    }
    
    // If key is not activated yet, activate it
    if (!$key['is_active']) {
        $stmt = $db->prepare("UPDATE keys SET hwid = ?, user_ip = ?, is_active = 1, activated_at = NOW(), expires_at = ? WHERE license_key = ?");
        
        $expiresAt = calculateExpiry($key['subscription_type']);
        $stmt->execute([$hwid, $ip, $expiresAt, $licenseKey]);
        
        logActivity('KEY_ACTIVATED', $licenseKey, $hwid, $ip, "Activated with subscription: {$key['subscription_type']}");
        
        return [
            'success' => true, 
            'message' => 'Key activated successfully',
            'subscription' => $key['subscription_type'],
            'expires' => $expiresAt
        ];
    }
    
    // Key is already activated - verify HWID
    if ($key['hwid'] !== $hwid) {
        logActivity('HWID_MISMATCH', $licenseKey, $hwid, $ip, "Expected: {$key['hwid']}");
        return ['success' => false, 'message' => 'Key is bound to different hardware', 'code' => 'HWID_MISMATCH'];
    }
    
    // Check expiry
    if ($key['expires_at'] && strtotime($key['expires_at']) < time()) {
        return ['success' => false, 'message' => 'Key has expired', 'code' => 'KEY_EXPIRED', 'expires' => $key['expires_at']];
    }
    
    // Update last check time
    $stmt = $db->prepare("UPDATE keys SET last_check = NOW() WHERE license_key = ?");
    $stmt->execute([$licenseKey]);
    
    logActivity('KEY_VALIDATED', $licenseKey, $hwid, $ip);
    
    return [
        'success' => true, 
        'message' => 'Key is valid',
        'subscription' => $key['subscription_type'],
        'expires' => $key['expires_at']
    ];
}

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

function logActivity($action, $key, $hwid, $ip, $details = null) {
    $db = getDB();
    $stmt = $db->prepare("INSERT INTO activity_log (action, license_key, hwid, ip_address, details) VALUES (?, ?, ?, ?, ?)");
    $stmt->execute([$action, $key, $hwid, $ip, $details]);
}

// Handle API request
if ($_SERVER['REQUEST_METHOD'] === 'POST') {
    $input = json_decode(file_get_contents('php://input'), true);
    
    $licenseKey = $input['key'] ?? '';
    $hwid = $input['hwid'] ?? '';
    $ip = $_SERVER['REMOTE_ADDR'] ?? 'unknown';
    
    if (empty($licenseKey) || empty($hwid)) {
        jsonResponse(['success' => false, 'message' => 'Missing key or hwid'], 400);
    }
    
    $result = validateKey($licenseKey, $hwid, $ip);
    jsonResponse($result);
} else {
    jsonResponse(['success' => false, 'message' => 'Method not allowed'], 405);
}
?>
