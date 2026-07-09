#pragma once
#include <Windows.h>
#include <opencv2/opencv.hpp>
#include <string>
#include <vector>
#include <algorithm>

class ammo_detector {
public:
    int detect() {
        HWND hWnd = FindWindowA(nullptr, "Rainbow Six");
        if (!hWnd) hWnd = FindWindowA(nullptr, "Tom Clancy's Rainbow Six");
        if (!hWnd) hWnd = FindWindowA("R6Game", nullptr);

        bool useWindow = (hWnd != nullptr);
        RECT rc;
        int winW, winH;

        if (useWindow && GetClientRect(hWnd, &rc)) {
            winW = rc.right - rc.left;
            winH = rc.bottom - rc.top;
        } else {
            useWindow = false;
            winW = GetSystemMetrics(SM_CXSCREEN);
            winH = GetSystemMetrics(SM_CYSCREEN);
            hWnd = GetDesktopWindow();
        }

        int ammoX = (int)(winW * 0.60f);
        int ammoY = (int)(winH * 0.85f);
        int ammoW = (int)(winW * 0.35f);
        int ammoH = (int)(winH * 0.12f);

        HDC hWinDC = GetDC(hWnd);
        if (!hWinDC) return -1;

        HDC hMemDC = CreateCompatibleDC(hWinDC);
        HBITMAP hBitmap = CreateCompatibleBitmap(hWinDC, ammoW, ammoH);
        SelectObject(hMemDC, hBitmap);

        BitBlt(hMemDC, 0, 0, ammoW, ammoH, hWinDC, ammoX, ammoY, SRCCOPY);

        BITMAPINFOHEADER bi = {};
        bi.biSize = sizeof(BITMAPINFOHEADER);
        bi.biWidth = ammoW;
        bi.biHeight = -ammoH;
        bi.biPlanes = 1;
        bi.biBitCount = 32;
        bi.biCompression = BI_RGB;

        cv::Mat bgra(ammoH, ammoW, CV_8UC4);
        GetDIBits(hMemDC, hBitmap, 0, ammoH, bgra.data, (BITMAPINFO*)&bi, DIB_RGB_COLORS);

        DeleteObject(hBitmap);
        DeleteDC(hMemDC);
        ReleaseDC(hWnd, hWinDC);

        static int debugFrame = 0;
        if (debugFrame++ % 300 == 0)
            cv::imwrite("ammo_debug.png", bgra);

        cv::Mat gray;
        cv::cvtColor(bgra, gray, cv::COLOR_BGRA2GRAY);

        cv::Mat thresh;
        cv::threshold(gray, thresh, 150, 255, cv::THRESH_BINARY_INV);

        if (debugFrame % 300 == 1)
            cv::imwrite("ammo_thresh.png", thresh);

        cv::Mat labels, stats, centroids;
        int nLabels = cv::connectedComponentsWithStats(thresh, labels, stats, centroids, 8);

        struct Blob { int idx, x, y, w, h, area; };
        std::vector<Blob> blobs;
        for (int i = 1; i < nLabels; i++) {
            int area = stats.at<int>(i, cv::CC_STAT_AREA);
            int cw = stats.at<int>(i, cv::CC_STAT_WIDTH);
            int ch = stats.at<int>(i, cv::CC_STAT_HEIGHT);
            if (area < 10 || area > 1500) continue;
            if (ch < 8 || ch > 45) continue;
            if (cw < 2 || cw > 30) continue;
            blobs.push_back({i, stats.at<int>(i, cv::CC_STAT_LEFT), stats.at<int>(i, cv::CC_STAT_TOP), cw, ch, area});
        }

        std::sort(blobs.begin(), blobs.end(), [](const Blob& a, const Blob& b) { return a.x < b.x; });

        int digitCount = 0;
        int digits[4] = {0};
        for (auto& b : blobs) {
            if (digitCount >= 4) break;
            digits[digitCount] = classifyDigit(thresh, stats, b.idx);
            digitCount++;
        }

        if (digitCount == 0) return -1;

        int result = 0;
        for (int i = 0; i < digitCount; i++) {
            if (digits[i] < 0) return -1;
            result = result * 10 + digits[i];
        }

        return result;
    }

private:
    int classifyDigit(const cv::Mat& thresh, const cv::Mat& stats, int label) {
        int x = stats.at<int>(label, cv::CC_STAT_LEFT);
        int y = stats.at<int>(label, cv::CC_STAT_TOP);
        int w = stats.at<int>(label, cv::CC_STAT_WIDTH);
        int h = stats.at<int>(label, cv::CC_STAT_HEIGHT);

        cv::Mat roi = thresh(cv::Rect(x, y, w, h)).clone();
        cv::Mat resized;
        cv::resize(roi, resized, cv::Size(10, 14));

        int totalPixels = 0;
        int topHalf = 0, botHalf = 0;
        int leftHalf = 0, rightHalf = 0;
        int topRow = 0, botRow = 0;
        int midRow = 0, midLeft = 0, midRight = 0;
        int midTop = 0;
        int rowPixels[14] = {};

        for (int r = 0; r < 14; r++) {
            for (int c = 0; c < 10; c++) {
                if (resized.at<uchar>(r, c) > 128) {
                    totalPixels++;
                    rowPixels[r]++;
                    if (r < 7) topHalf++; else botHalf++;
                    if (c < 5) leftHalf++; else rightHalf++;
                    if (r >= 4 && r < 10) midRow++;
                    if (r == 0) topRow++;
                    if (r == 13) botRow++;
                    if (r >= 5 && r <= 8) midTop++;
                }
            }
        }

        float density = (float)totalPixels / 140.0f;
        float vertRatio = (float)(topHalf + 1) / (botHalf + 1);
        float horizRatio = (float)(leftHalf + 1) / (rightHalf + 1);

        if (density < 0.10f) return 1;

        int filledRows = 0;
        for (int r = 0; r < 14; r++) {
            if (rowPixels[r] >= 5) filledRows++;
        }

        bool topBar = rowPixels[0] >= 5;
        bool botBar = rowPixels[13] >= 5;
        bool midBar = midRow > 12;

        if (density > 0.40f && filledRows >= 10) return 8;

        if (topBar && midBar && !botBar && vertRatio < 0.9f) return 2;
        if (topBar && botBar && midBar && horizRatio < 0.75f) return 2;

        if (topBar && botBar && !midBar) return 3;

        if (!topBar && botBar && density > 0.30f) return 6;
        if (topBar && !botBar && density > 0.30f) return 9;

        if (horizRatio < 0.65f && density > 0.20f) return 7;

        if (vertRatio > 1.3f && density > 0.30f) return 5;

        if (density > 0.20f) return 4;

        if (density > 0.35f) return 0;

        return -1;
    }
};
