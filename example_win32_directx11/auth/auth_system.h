#pragma once
#include <Windows.h>
#include <string>
#include <winhttp.h>
#include <sstream>
#include <json.hpp>

#pragma comment(lib, "winhttp.lib")

using json = nlohmann::json;

class AuthSystem {
private:
    std::string m_apiUrl;
    std::string m_hwid;
    std::string m_licenseKey;
    std::string m_subscription;
    std::string m_expiresAt;
    bool m_isValid;

    std::string getHWID() {
        HW_PROFILE_INFO hwInfo;
        if (GetCurrentHwProfile(&hwInfo)) {
            return std::string(hwInfo.szHwProfileGuid);
        }
        
        // Fallback: use machine GUID from registry
        HKEY hKey;
        char buffer[256];
        DWORD bufferSize = sizeof(buffer);
        
        if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Cryptography", 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
            if (RegQueryValueExA(hKey, "MachineGuid", nullptr, nullptr, (LPBYTE)buffer, &bufferSize) == ERROR_SUCCESS) {
                RegCloseKey(hKey);
                return std::string(buffer);
            }
            RegCloseKey(hKey);
        }
        
        return "UNKNOWN_HWID";
    }

    std::string httpRequest(const std::string& url, const std::string& method, const std::string& body = "") {
        // Parse URL
        URL_COMPONENTS urlComp = { 0 };
        urlComp.dwStructSize = sizeof(urlComp);
        urlComp.dwSchemeLength = 1;
        urlComp.dwHostNameLength = 1;
        urlComp.dwUrlPathLength = 1;
        urlComp.dwExtraInfoLength = 1;
        
        // Convert string to wide string
        std::wstring wUrl(url.begin(), url.end());
        
        if (!WinHttpCrackUrl(wUrl.c_str(), 0, 0, &urlComp)) {
            return "";
        }
        
        std::wstring hostName(urlComp.lpszHostName, urlComp.dwHostNameLength);
        std::wstring urlPath(urlComp.lpszUrlPath, urlComp.dwUrlPathLength);
        std::wstring extraInfo(urlComp.lpszExtraInfo, urlComp.dwExtraInfoLength);
        
        // Open connection
        HINTERNET hSession = WinHttpOpen(L"AuthClient/1.0", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
        if (!hSession) return "";
        
        HINTERNET hConnect = WinHttpConnect(hSession, hostName.c_str(), urlComp.nPort, 0);
        if (!hConnect) {
            WinHttpCloseHandle(hSession);
            return "";
        }
        
        std::wstring fullUrl = urlPath + extraInfo;
        HINTERNET hRequest = WinHttpOpenRequest(hConnect, 
            method == "POST" ? L"POST" : L"GET", 
            fullUrl.c_str(), nullptr, WINHTTP_NO_REFERER, 
            WINHTTP_DEFAULT_ACCEPT_TYPES, 
            urlComp.nScheme == INTERNET_SCHEME_HTTPS ? WINHTTP_FLAG_SECURE : 0);
        
        if (!hRequest) {
            WinHttpCloseHandle(hConnect);
            WinHttpCloseHandle(hSession);
            return "";
        }
        
        // Send request
        BOOL result = FALSE;
        if (method == "POST" && !body.empty()) {
            const wchar_t* headers = L"Content-Type: application/json";
            result = WinHttpSendRequest(hRequest, headers, -1, (LPVOID)body.c_str(), body.size(), body.size(), 0);
        } else {
            result = WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0, 0);
        }
        
        std::string response;
        if (result && WinHttpReceiveResponse(hRequest, nullptr)) {
            DWORD bytesAvailable = 0;
            while (WinHttpQueryDataAvailable(hRequest, &bytesAvailable) && bytesAvailable > 0) {
                std::vector<char> buffer(bytesAvailable + 1, 0);
                DWORD bytesRead = 0;
                WinHttpReadData(hRequest, buffer.data(), bytesAvailable, &bytesRead);
                response.append(buffer.data(), bytesRead);
            }
        }
        
        WinHttpCloseHandle(hRequest);
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        
        return response;
    }

public:
    AuthSystem(const std::string& apiUrl) 
        : m_apiUrl(apiUrl), m_isValid(false) {
        m_hwid = getHWID();
    }

    bool validate(const std::string& licenseKey) {
        m_licenseKey = licenseKey;
        
        json request;
        request["key"] = licenseKey;
        request["hwid"] = m_hwid;
        
        std::string response = httpRequest(m_apiUrl + "/validate.php", "POST", request.dump());
        
        if (response.empty()) {
            return false;
        }
        
        try {
            json result = json::parse(response);
            m_isValid = result["success"].get<bool>();
            
            if (m_isValid) {
                m_subscription = result.value("subscription", "unknown");
                m_expiresAt = result.value("expires", "never");
            }
            
            return m_isValid;
        } catch (...) {
            return false;
        }
    }

    bool isValid() const { return m_isValid; }
    std::string getHWID() const { return m_hwid; }
    std::string getLicenseKey() const { return m_licenseKey; }
    std::string getSubscription() const { return m_subscription; }
    std::string getExpiresAt() const { return m_expiresAt; }
};
