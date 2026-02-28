#include "display.h"
#include "epd_driver.h"
#include "firasans.h"

// Display: 960 x 540, 4-bit grayscale (0=black, 15=white)
#define DISPLAY_W 960
#define DISPLAY_H 540

static uint8_t *fb = nullptr;

// Font properties: black text on white
static FontProperties propsBlack() {
    FontProperties p;
    p.fg_color = 0;
    p.bg_color = 15;
    p.fallback_glyph = true;
    p.flags = 0;
    return p;
}

// Font properties: gray text on white (for labels)
static FontProperties propsGray() {
    FontProperties p;
    p.fg_color = 5;
    p.bg_color = 15;
    p.fallback_glyph = true;
    p.flags = 0;
    return p;
}

static void fbClear() {
    memset(fb, 0xFF, DISPLAY_W * DISPLAY_H / 2);
}

static void pushToScreen() {
    Rect_t area = {0, 0, DISPLAY_W, DISPLAY_H};
    epd_poweron();
    epd_clear();
    epd_draw_grayscale_image(area, fb);
    epd_poweroff();
}

void displayInit() {
    epd_init();
    fb = (uint8_t *)ps_calloc(sizeof(uint8_t), DISPLAY_W * DISPLAY_H / 2);
    if (!fb) {
        Serial.println("FATAL: Failed to allocate framebuffer from PSRAM");
        while (1) delay(1000);
    }
}

void displaySplash() {
    fbClear();

    FontProperties fp = propsBlack();
    int x, y;

    // Title
    x = 170;
    y = 200;
    write_string(&FiraSans, "Lunch Remixer", &x, &y, fb, &FiraSans, &fp);

    // Divider line
    epd_draw_hline(100, 230, 760, 0, fb);

    // Subtitle
    FontProperties gray = propsGray();
    x = 250;
    y = 300;
    write_string(&FiraSans, "Press button for today's lunch!", &x, &y, fb, &FiraSans, &gray);

    // Hint
    x = 220;
    y = 450;
    write_string(&FiraSans, "Hold 3s to enter WiFi edit mode", &x, &y, fb, &FiraSans, &gray);

    pushToScreen();
}

void displayLunch(const LunchCombo &combo) {
    fbClear();

    FontProperties fp = propsBlack();
    FontProperties gray = propsGray();
    int x, y;

    // Title
    x = 230;
    y = 70;
    write_string(&FiraSans, "Today's Lunch", &x, &y, fb, &FiraSans, &fp);

    // Divider
    epd_draw_hline(40, 90, 880, 0, fb);
    epd_draw_hline(40, 91, 880, 0, fb);

    // Category rows
    int rowY = 140;
    for (int i = 0; i < combo.count; i++) {
        // Category label (gray, smaller feel)
        x = 60;
        y = rowY;
        write_string(&FiraSans, combo.categoryNames[i].c_str(), &x, &y, fb, &FiraSans, &gray);

        // Selected item (black, bold feel)
        x = 280;
        y = rowY;
        write_string(&FiraSans, combo.selectedItems[i].c_str(), &x, &y, fb, &FiraSans, &fp);

        // Subtle separator line
        epd_draw_hline(60, rowY + 15, 840, 12, fb);  // light gray line

        rowY += 75;
    }

    // Bottom border
    epd_draw_hline(40, DISPLAY_H - 20, 880, 0, fb);

    pushToScreen();
}

void displayError(const char *msg) {
    fbClear();

    FontProperties fp = propsBlack();
    int x = 100, y = 200;
    write_string(&FiraSans, "Error!", &x, &y, fb, &FiraSans, &fp);

    epd_draw_hline(100, 220, 760, 0, fb);

    FontProperties gray = propsGray();
    x = 100;
    y = 300;
    write_string(&FiraSans, msg, &x, &y, fb, &FiraSans, &gray);

    pushToScreen();
}

void displayWifiMode(const char *ssid, const char *ip) {
    fbClear();

    FontProperties fp = propsBlack();
    FontProperties gray = propsGray();
    int x, y;

    x = 200;
    y = 120;
    write_string(&FiraSans, "WiFi Edit Mode", &x, &y, fb, &FiraSans, &fp);

    epd_draw_hline(100, 145, 760, 0, fb);

    x = 100;
    y = 220;
    write_string(&FiraSans, "1. Connect to WiFi:", &x, &y, fb, &FiraSans, &gray);

    x = 150;
    y = 280;
    write_string(&FiraSans, ssid, &x, &y, fb, &FiraSans, &fp);

    x = 100;
    y = 350;
    write_string(&FiraSans, "2. Open browser to:", &x, &y, fb, &FiraSans, &gray);

    char url[64];
    snprintf(url, sizeof(url), "http://%s", ip);
    x = 150;
    y = 410;
    write_string(&FiraSans, url, &x, &y, fb, &FiraSans, &fp);

    x = 100;
    y = 490;
    write_string(&FiraSans, "Press button to exit WiFi mode", &x, &y, fb, &FiraSans, &gray);

    pushToScreen();
}

void displayClear() {
    fbClear();
}

void displayPowerOff() {
    epd_poweroff();
}
