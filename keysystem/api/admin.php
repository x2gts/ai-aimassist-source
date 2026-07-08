<?php
require_once __DIR__ . '/config.php';

verifyApiKey();

$db = getDB();
$action = $_GET['action'] ?? $_POST['action'] ?? '';

switch ($action) {
    case 'generate':
        generateKeys();
        break;
    case 'list':
        listKeys();
        break;
    case 'revoke':
        revokeKey();
        break;
    case 'ban':
        banUser();
        break;
    case 'unban':
        unbanUser();
        break;
    case 'stats':
        getStats();
        break;
    case 'log':
        getActivityLog();
        break;
    default:
        jsonResponse(['success' => false, 'message' => 'Invalid action'], 400);
}

function generateKeys() {
    global $db;
    $count = min(max((int)($_POST['count'] ?? 1), 1), 100);
    $subscription = $_POST['subscription'] ?? 'trial';
    $prefix = $_POST['prefix'] ?? 'AIM';
    $createdBy = $_POST['created_by'] ?? 'admin';
    
    $validSubs = ['trial', '1day', '7day', '30day', 'lifetime'];
    if (!in_array($subscription, $validSubs)) {
        jsonResponse(['success' => false, 'message' => 'Invalid subscription type']);
    }
    
    $keys = [];
    for ($i = 0; $i < $count; $i++) {
        $key = generateKey($prefix);
        $stmt = $db->prepare("INSERT INTO license_keys (license_key, subscription_type, created_by) VALUES (:key, :sub, :by)");
        $stmt->bindValue(':key', $key, SQLITE3_TEXT);
        $stmt->bindValue(':sub', $subscription, SQLITE3_TEXT);
        $stmt->bindValue(':by', $createdBy, SQLITE3_TEXT);
        $stmt->execute();
        $keys[] = $key;
    }
    
    jsonResponse([
        'success' => true, 
        'message' => "Generated {$count} keys",
        'keys' => $keys,
        'subscription' => $subscription
    ]);
}

function listKeys() {
    global $db;
    $page = max((int)($_GET['page'] ?? 1), 1);
    $limit = min(max((int)($_GET['limit'] ?? 50), 1), 100);
    $offset = ($page - 1) * $limit;
    $search = $_GET['search'] ?? '';
    
    $where = '';
    $params = [];
    
    if (!empty($search)) {
        $where = "WHERE license_key LIKE :search1 OR hwid LIKE :search2";
        $params[':search1'] = "%{$search}%";
        $params[':search2'] = "%{$search}%";
    }
    
    $stmt = $db->prepare("SELECT COUNT(*) as total FROM license_keys {$where}");
    foreach ($params as $k => $v) $stmt->bindValue($k, $v, SQLITE3_TEXT);
    $total = $stmt->execute()->fetchArray(SQLITE3_ASSOC)['total'];
    
    $stmt = $db->prepare("SELECT * FROM license_keys {$where} ORDER BY created_at DESC LIMIT :limit OFFSET :offset");
    foreach ($params as $k => $v) $stmt->bindValue($k, $v, SQLITE3_TEXT);
    $stmt->bindValue(':limit', $limit, SQLITE3_INTEGER);
    $stmt->bindValue(':offset', $offset, SQLITE3_INTEGER);
    $keys = $stmt->execute()->fetchArray(SQLITE3_ASSOC);
    
    $allKeys = [];
    while ($keys) {
        $allKeys[] = $keys;
        $keys = $stmt->execute()->fetchArray(SQLITE3_ASSOC);
    }
    
    jsonResponse([
        'success' => true,
        'keys' => $allKeys,
        'total' => $total,
        'page' => $page,
        'pages' => ceil($total / $limit)
    ]);
}

function revokeKey() {
    global $db;
    $key = $_POST['key'] ?? '';
    
    if (empty($key)) {
        jsonResponse(['success' => false, 'message' => 'Missing key']);
    }
    
    $stmt = $db->prepare("DELETE FROM license_keys WHERE license_key = :key");
    $stmt->bindValue(':key', $key, SQLITE3_TEXT);
    $stmt->execute();
    
    jsonResponse(['success' => true, 'message' => 'Key revoked']);
}

function banUser() {
    global $db;
    $hwid = $_POST['hwid'] ?? '';
    $reason = $_POST['reason'] ?? 'No reason provided';
    
    if (empty($hwid)) {
        jsonResponse(['success' => false, 'message' => 'Missing HWID']);
    }
    
    $stmt = $db->prepare("INSERT INTO bans (hwid, reason) VALUES (:hwid, :reason)");
    $stmt->bindValue(':hwid', $hwid, SQLITE3_TEXT);
    $stmt->bindValue(':reason', $reason, SQLITE3_TEXT);
    $stmt->execute();
    
    $stmt = $db->prepare("UPDATE license_keys SET is_banned = 1 WHERE hwid = :hwid");
    $stmt->bindValue(':hwid', $hwid, SQLITE3_TEXT);
    $stmt->execute();
    
    jsonResponse(['success' => true, 'message' => 'User banned']);
}

function unbanUser() {
    global $db;
    $hwid = $_POST['hwid'] ?? '';
    
    if (empty($hwid)) {
        jsonResponse(['success' => false, 'message' => 'Missing HWID']);
    }
    
    $stmt = $db->prepare("DELETE FROM bans WHERE hwid = :hwid");
    $stmt->bindValue(':hwid', $hwid, SQLITE3_TEXT);
    $stmt->execute();
    
    $stmt = $db->prepare("UPDATE license_keys SET is_banned = 0 WHERE hwid = :hwid");
    $stmt->bindValue(':hwid', $hwid, SQLITE3_TEXT);
    $stmt->execute();
    
    jsonResponse(['success' => true, 'message' => 'User unbanned']);
}

function getStats() {
    global $db;
    
    $stats = [];
    
    $stats['total_keys'] = $db->querySingle("SELECT COUNT(*) FROM license_keys");
    $stats['active_keys'] = $db->querySingle("SELECT COUNT(*) FROM license_keys WHERE is_active = 1");
    $stats['banned_keys'] = $db->querySingle("SELECT COUNT(*) FROM license_keys WHERE is_banned = 1");
    $stats['online_24h'] = $db->querySingle("SELECT COUNT(*) FROM license_keys WHERE last_check > datetime('now', '-1 day')");
    $stats['banned_hwids'] = $db->querySingle("SELECT COUNT(*) FROM bans");
    
    jsonResponse(['success' => true, 'stats' => $stats]);
}

function getActivityLog() {
    global $db;
    $limit = min(max((int)($_GET['limit'] ?? 100), 1), 500);
    
    $stmt = $db->prepare("SELECT * FROM activity_log ORDER BY created_at DESC LIMIT :limit");
    $stmt->bindValue(':limit', $limit, SQLITE3_INTEGER);
    $result = $stmt->execute();
    
    $logs = [];
    while ($row = $result->fetchArray(SQLITE3_ASSOC)) {
        $logs[] = $row;
    }
    
    jsonResponse(['success' => true, 'logs' => $logs]);
}
?>
