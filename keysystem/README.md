# Key System Setup Guide

## Overview
This is a self-hosted license key system with:
- PHP backend API
- MySQL database
- Admin dashboard
- C++ client integration

## Files Structure
```
keysystem/
├── api/
│   ├── config.php        # Database config & helpers
│   ├── validate.php      # Key validation endpoint
│   ├── admin.php         # Admin API (generate, list, ban)
│   └── login.php         # Admin login
├── admin/
│   └── index.html        # Admin dashboard
└── database.sql          # MySQL schema

example_win32_directx11/
├── auth/
│   └── auth_system.h     # C++ client (replaces KeyAuth)
└── main/
    └── main.cpp          # Updated to use new auth
```

## Setup Instructions

### 1. Database Setup
1. Create a MySQL database
2. Import `database.sql` into your database
3. Update `api/config.php` with your database credentials:
   ```php
   define('DB_HOST', 'localhost');
   define('DB_NAME', 'key_system');
   define('DB_USER', 'your_user');
   define('DB_PASS', 'your_password');
   ```

### 2. Security Keys
In `api/config.php`, change these to random strings:
```php
define('ADMIN_API_KEY', 'your_random_api_key_here');
define('API_SECRET', 'your_random_secret_here');
```

### 3. Admin Password
1. Generate a password hash:
   ```php
   echo password_hash('your_password', PASSWORD_DEFAULT);
   ```
2. Update the `admins` table in MySQL with your hash

### 4. Deploy Backend
Upload the `keysystem/` folder to any PHP hosting:
- Shared hosting (cPanel, Plesk)
- VPS (DigitalOcean, Linode)
- Cloud (AWS, Google Cloud)

### 5. Update C++ Client
In `main.cpp`, update the API URL:
```cpp
std::string api_url = "http://your-domain.com/keysystem/api";
```

### 6. Add nlohmann/json
Download `json.hpp` from: https://github.com/nlohmann/json/releases
Place it in `example_win32_directx11/auth/` folder

### 7. Build
Open `imgui_examples.sln` in Visual Studio and build.

## Admin Dashboard
Access at: `http://your-domain.com/keysystem/admin/`

Features:
- Generate license keys (bulk supported)
- View all keys with status
- Ban/unban users by HWID
- View activity log
- See statistics

## Key Types
- **Trial**: 24 hours
- **1day**: 1 day
- **7day**: 7 days
- **30day**: 30 days
- **Lifetime**: Never expires

## HWID Locking
Keys are automatically locked to the first hardware that activates them.
If a different HWID tries to use the key, it will be rejected.

## API Endpoints
- `POST /validate.php` - Validate a license key
- `GET /admin.php?action=list` - List all keys
- `POST /admin.php?action=generate` - Generate new keys
- `POST /admin.php?action=ban` - Ban a HWID
- `POST /admin.php?action=unban` - Unban a HWID
- `GET /admin.php?action=stats` - Get statistics
- `GET /admin.php?action=log` - Get activity log

## GitHub Pages (Optional)
For a public-facing website, you can create a static page that links to your admin dashboard.
