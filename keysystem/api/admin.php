<?php
require_once __DIR__ . '/config.php';

// Admin API for key management
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
    $db = getDB();
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
        $stmt = $db->prepare("INSERT INTO license_keys (license_key, subscription_type, created_by) VALUES (?, ?, ?)");
        $stmt->execute([$key, $subscription, $createdBy]);
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
    $db = getDB();
    $page = max((int)($_GET['page'] ?? 1), 1);
    $limit = min(max((int)($_GET['limit'] ?? 50), 1), 100);
    $offset = ($page - 1) * $limit;
    $search = $_GET['search'] ?? '';
    
    $where = '';
    $params = [];
    
    if (!empty($search)) {
        $where = "WHERE license_key LIKE ? OR hwid LIKE ?";
        $params[] = "%{$search}%";
        $params[] = "%{$search}%";
    }
    
    $stmt = $db->prepare("SELECT COUNT(*) as total FROM license_keys {$where}");
    $stmt->execute($params);
    $total = $stmt->fetch()['total'];
    
    $stmt = $db->prepare("SELECT * FROM license_keys {$where} ORDER BY created_at DESC LIMIT ? OFFSET ?");
    $params[] = $limit;
    $params[] = $offset;
    $stmt->execute($params);
    $keys = $stmt->fetchAll();
    
    jsonResponse([
        'success' => true,
        'keys' => $keys,
        'total' => $total,
        'page' => $page,
        'pages' => ceil($total / $limit)
    ]);
}

function revokeKey() {
    $db = getDB();
    $key = $_POST['key'] ?? '';
    
    if (empty($key)) {
        jsonResponse(['success' => false, 'message' => 'Missing key']);
    }
    
    $stmt = $db->prepare("DELETE FROM license_keys WHERE license_key = ?");
    $stmt->execute([$key]);
    
    if ($stmt->rowCount() > 0) {
        jsonResponse(['success' => true, 'message' => 'Key revoked']);
    } else {
        jsonResponse(['success' => false, 'message' => 'Key not found']);
    }
}

function banUser() {
    $db = getDB();
    $hwid = $_POST['hwid'] ?? '';
    $reason = $_POST['reason'] ?? 'No reason provided';
    
    if (empty($hwid)) {
        jsonResponse(['success' => false, 'message' => 'Missing HWID']);
    }
    
    $stmt = $db->prepare("SELECT id FROM bans WHERE hwid = ?");
    $stmt->execute([$hwid]);
    if ($stmt->fetch()) {
        jsonResponse(['success' => false, 'message' => 'HWID already banned']);
    }
    
    $stmt = $db->prepare("INSERT INTO bans (hwid, reason) VALUES (?, ?)");
    $stmt->execute([$hwid, $reason]);
    
    $stmt = $db->prepare("UPDATE license_keys SET is_banned = 1 WHERE hwid = ?");
    $stmt->execute([$hwid]);
    
    jsonResponse(['success' => true, 'message' => 'User banned']);
}

function unbanUser() {
    $db = getDB();
    $hwid = $_POST['hwid'] ?? '';
    
    if (empty($hwid)) {
        jsonResponse(['success' => false, 'message' => 'Missing HWID']);
    }
    
    $stmt = $db->prepare("DELETE FROM bans WHERE hwid = ?");
    $stmt->execute([$hwid]);
    
    $stmt = $db->prepare("UPDATE license_keys SET is_banned = 0 WHERE hwid = ?");
    $stmt->execute([$hwid]);
    
    jsonResponse(['success' => true, 'message' => 'User unbanned']);
}

function getStats() {
    $db = getDB();
    
    $stats = [];
    
    $stmt = $db->query("SELECT COUNT(*) as total FROM license_keys");
    $stats['total_keys'] = $stmt->fetch()['total'];
    
    $stmt = $db->query("SELECT COUNT(*) as total FROM license_keys WHERE is_active = 1");
    $stats['active_keys'] = $stmt->fetch()['total'];
    
    $stmt = $db->query("SELECT COUNT(*) as total FROM license_keys WHERE is_banned = 1");
    $stats['banned_keys'] = $stmt->fetch()['total'];
    
    $stmt = $db->query("SELECT COUNT(*) as total FROM license_keys WHERE last_check > DATE_SUB(NOW(), INTERVAL 24 HOUR)");
    $stats['online_24h'] = $stmt->fetch()['total'];
    
    $stmt = $db->query("SELECT COUNT(*) as total FROM bans");
    $stats['banned_hwids'] = $stmt->fetch()['total'];
    
    $stmt = $db->query("SELECT subscription_type, COUNT(*) as count FROM license_keys GROUP BY subscription_type");
    $stats['by_subscription'] = $stmt->fetchAll();
    
    jsonResponse(['success' => true, 'stats' => $stats]);
}

function getActivityLog() {
    $db = getDB();
    $limit = min(max((int)($_GET['limit'] ?? 100), 1), 500);
    
    $stmt = $db->prepare("SELECT * FROM activity_log ORDER BY created_at DESC LIMIT ?");
    $stmt->execute([$limit]);
    $logs = $stmt->fetchAll();
    
    jsonResponse(['success' => true, 'logs' => $logs]);
}
?>
