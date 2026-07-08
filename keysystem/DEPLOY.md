# AI Aim Assist - Key System

## Setup

### 1. Admin Dashboard (GitHub Pages)
The admin dashboard is static HTML - enable GitHub Pages:
1. Go to repo Settings > Pages
2. Source: `main` branch, `/keysystem/admin` folder
3. Access at: `https://yourusername.github.io/repo-name/`

### 2. PHP Backend
GitHub Pages can't run PHP. Deploy to one of these free options:

**Option A: Render.com (Free)**
1. Create account at render.com
2. New > Web Service > Connect GitHub repo
3. Runtime: PHP
4. Build Command: `composer install` (if needed)
5. Start Command: `php -S 0.0.0.0:$PORT -t keysystem/api`

**Option B: Railway.app ($5 free credit)**
1. Create account at railway.app
2. New Project > Deploy from GitHub
3. Add MySQL database plugin
4. Set start command: `php -S 0.0.0.0:$PORT -t keysystem/api`

**Option C: InfinityFree (Free traditional hosting)**
1. Sign up at infinityfree.net
2. Upload `keysystem/` folder via File Manager
3. Create MySQL database in control panel

**Option D: Any cheap PHP hosting ($1-3/month)**
- Hostinger, Namecheap, Bluehost, etc.
- Upload via FTP or File Manager

### 3. Update API URL
After deploying backend, update `main.cpp`:
```cpp
std::string api_url = "https://your-backend-url/api";
```

Then update `admin/index.html`:
```javascript
const API_URL = 'https://your-backend-url/api';
```

## File Structure
```
your-repo/
├── keysystem/
│   ├── api/           # Deploy to PHP host
│   ├── admin/         # GitHub Pages
│   ├── database.sql
│   └── README.md
└── example_win32_directx11/  # Your app
```
