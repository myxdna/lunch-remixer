#include "display.h"
#include "epd_driver.h"
#include "firasans.h"
#include <Arduino.h>

// Display: 960 × 540, 4-bit grayscale (0 = black, 15 = white)
#define DISPLAY_W 960
#define DISPLAY_H 540

// Layout constants — Teenage Engineering-inspired grid
#define MARGIN      20    // outer border inset from display edge
#define HEADER_H    72    // black header bar height (px)
#define CONTENT_TOP (MARGIN + HEADER_H)   // y=92: start of row content
#define CONTENT_BOT (DISPLAY_H - MARGIN - 32)  // y=488: end of row content, footer below
#define FOOTER_Y    CONTENT_BOT
#define DIV_X       252   // x: vertical rule separating label col from item col

static uint8_t *fb = nullptr;

// --- Font property helpers ---
static FontProperties propsWhite() {   // white on black (header, inverted cells)
    FontProperties p;
    p.fg_color = 15; p.bg_color = 0;
    p.fallback_glyph = true; p.flags = 0;
    return p;
}
static FontProperties propsBlack() {   // black on white (item names)
    FontProperties p;
    p.fg_color = 0; p.bg_color = 15;
    p.fallback_glyph = true; p.flags = 0;
    return p;
}
static FontProperties propsGray() {    // mid-gray on white (labels, hints)
    FontProperties p;
    p.fg_color = 6; p.bg_color = 15;
    p.fallback_glyph = true; p.flags = 0;
    return p;
}

// --- Primitive helpers ---
static void fbClear() {
    memset(fb, 0xFF, DISPLAY_W * DISPLAY_H / 2);
}

// Fill a solid axis-aligned rectangle
static void fillRect(int x, int y, int w, int h, uint8_t color) {
    for (int row = 0; row < h; row++)
        epd_draw_hline(x, y + row, w, color, fb);
}

// 2-px thick outer border rect
static void drawBorder() {
    int x = MARGIN, y = MARGIN;
    int w = DISPLAY_W - 2 * MARGIN, h = DISPLAY_H - 2 * MARGIN;
    for (int t = 0; t < 2; t++) {
        epd_draw_hline(x,     y + t,         w, 0, fb);
        epd_draw_hline(x,     y + h - 1 - t, w, 0, fb);
        epd_draw_vline(x + t,         y, h, 0, fb);
        epd_draw_vline(x + w - 1 - t, y, h, 0, fb);
    }
}

// 2-px thick horizontal rule across full content width
static void hRule(int y) {
    epd_draw_hline(MARGIN, y,     DISPLAY_W - 2 * MARGIN, 0, fb);
    epd_draw_hline(MARGIN, y + 1, DISPLAY_W - 2 * MARGIN, 0, fb);
}

// 2-px thick vertical rule
static void vRule(int x, int y_top, int y_bot) {
    epd_draw_vline(x,     y_top, y_bot - y_top, 0, fb);
    epd_draw_vline(x + 1, y_top, y_bot - y_top, 0, fb);
}

// Inverted black header bar with white title text (centered by eye for each string)
static void drawHeader(const char *title, int approxTextX) {
    fillRect(MARGIN, MARGIN, DISPLAY_W - 2 * MARGIN, HEADER_H, 0);
    FontProperties fp = propsWhite();
    int x = approxTextX;
    int y = MARGIN + HEADER_H - 16;
    write_string(&FiraSans, title, &x, &y, fb, &FiraSans, &fp);
}

static void pushToScreen() {
    Rect_t area = {0, 0, DISPLAY_W, DISPLAY_H};
    epd_poweron();
    epd_clear();
    epd_draw_grayscale_image(area, fb);
    epd_poweroff();
}

// ----------------------------------------------------------------
void displayInit() {
    epd_init();
    fb = (uint8_t *)ps_calloc(sizeof(uint8_t), DISPLAY_W * DISPLAY_H / 2);
    if (!fb) {
        Serial.println("FATAL: Failed to allocate framebuffer from PSRAM");
        while (1) delay(1000);
    }
}

// ----------------------------------------------------------------
void displaySplash() {
    fbClear();
    drawHeader("LUNCH REMIXER", 330);
    drawBorder();

    FontProperties fp = propsBlack();
    FontProperties gray = propsGray();
    int x, y;

    // Main prompt — bold feel via spacing
    x = DISPLAY_W / 2 - 115;
    y = 260;
    write_string(&FiraSans, "PRESS BUTTON.", &x, &y, fb, &FiraSans, &fp);

    x = DISPLAY_W / 2 - 115;
    y = 330;
    write_string(&FiraSans, "EAT LUNCH.", &x, &y, fb, &FiraSans, &gray);

    // Footer rule + edit mode hint
    hRule(FOOTER_Y);
    x = MARGIN + 16;
    y = DISPLAY_H - MARGIN - 8;
    write_string(&FiraSans, "HOLD 3s — WIFI EDIT MODE", &x, &y, fb, &FiraSans, &gray);

    pushToScreen();
}

// ----------------------------------------------------------------
void displayLunch(const LunchCombo &combo) {
    fbClear();
    drawHeader("LUNCH REMIXER", 330);
    drawBorder();

    FontProperties fp = propsBlack();
    FontProperties gray = propsGray();

    int rowH = (CONTENT_BOT - CONTENT_TOP) / combo.count;

    // Vertical divider — label | item
    vRule(DIV_X, CONTENT_TOP, CONTENT_BOT);

    for (int i = 0; i < combo.count; i++) {
        int rowTop  = CONTENT_TOP + i * rowH;
        int textY   = rowTop + rowH / 2 + 9;   // baseline centered in row

        // Category label — ALL CAPS, gray, left column
        String label = combo.categoryNames[i];
        label.toUpperCase();
        int x = MARGIN + 14;
        int y = textY;
        write_string(&FiraSans, label.c_str(), &x, &y, fb, &FiraSans, &gray);

        // Selected item — black, right column
        x = DIV_X + 18;
        y = textY;
        write_string(&FiraSans, combo.selectedItems[i].c_str(), &x, &y, fb, &FiraSans, &fp);

        // Row divider (between rows only — not after the last row)
        if (i < combo.count - 1)
            hRule(rowTop + rowH);
    }

    // Footer rule + hint
    hRule(FOOTER_Y);
    int x = MARGIN + 14;
    int y = DISPLAY_H - MARGIN - 8;
    write_string(&FiraSans, "HOLD 3s — WIFI EDIT", &x, &y, fb, &FiraSans, &gray);

    pushToScreen();
}

// ----------------------------------------------------------------
void displayError(const char *msg) {
    fbClear();
    drawHeader("ERROR", 400);
    drawBorder();

    FontProperties fp = propsBlack();
    int x = MARGIN + 30;
    int y = 300;
    write_string(&FiraSans, msg, &x, &y, fb, &FiraSans, &fp);

    pushToScreen();
}

// ----------------------------------------------------------------
void displayWifiMode(const char *ssid, const char *ip) {
    fbClear();
    drawHeader("WIFI EDIT MODE", 310);
    drawBorder();

    FontProperties fp = propsBlack();
    FontProperties gray = propsGray();

    // Two-column table: label | value, matching the lunch screen's DIV_X
    vRule(DIV_X, CONTENT_TOP, CONTENT_BOT);

    // Row 1 — Network
    int row1Y = CONTENT_TOP + 85;
    int x = MARGIN + 14;
    int y = row1Y;
    write_string(&FiraSans, "NETWORK", &x, &y, fb, &FiraSans, &gray);
    x = DIV_X + 18; y = row1Y;
    write_string(&FiraSans, ssid, &x, &y, fb, &FiraSans, &fp);

    hRule(CONTENT_TOP + 110);

    // Row 2 — URL
    int row2Y = CONTENT_TOP + 200;
    char url[64];
    snprintf(url, sizeof(url), "http://%s", ip);
    x = MARGIN + 14; y = row2Y;
    write_string(&FiraSans, "URL", &x, &y, fb, &FiraSans, &gray);
    x = DIV_X + 18; y = row2Y;
    write_string(&FiraSans, url, &x, &y, fb, &FiraSans, &fp);

    hRule(CONTENT_TOP + 220);

    // Instruction row
    x = MARGIN + 14;
    y = CONTENT_TOP + 310;
    write_string(&FiraSans, "Connect phone to network,", &x, &y, fb, &FiraSans, &gray);
    x = MARGIN + 14; y = CONTENT_TOP + 360;
    write_string(&FiraSans, "then open browser to URL.", &x, &y, fb, &FiraSans, &gray);

    // Footer rule + exit hint
    hRule(FOOTER_Y);
    x = MARGIN + 14;
    y = DISPLAY_H - MARGIN - 8;
    write_string(&FiraSans, "PRESS BUTTON TO EXIT", &x, &y, fb, &FiraSans, &gray);

    pushToScreen();
}

// ----------------------------------------------------------------
void displayClear() {
    fbClear();
}

void displayPowerOff() {
    epd_poweroff();
}
