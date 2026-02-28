// Lunch Remixer — LILYGO T5 4.7" e-Paper (ESP32-S3)
// Press button: new random lunch combo from your food list
// Hold 3s: WiFi mode to edit the food list from your phone
// Deep sleeps between presses (~10uA)

#include <Arduino.h>
#include <LittleFS.h>
#include "display.h"
#include "lunch_logic.h"
#include "wifi_editor.h"

#define BUTTON_PIN 21
#define LONG_PRESS_MS 3000

// Track boot count across deep sleep cycles
RTC_DATA_ATTR int bootCount = 0;
RTC_DATA_ATTR bool showedSplash = false;

static void goToSleep() {
    displayPowerOff();

    // Wake on button press (GPIO 21, active LOW)
    esp_sleep_enable_ext0_wakeup((gpio_num_t)BUTTON_PIN, 0);
    Serial.println("Entering deep sleep...");
    Serial.flush();
    esp_deep_sleep_start();
    // Never reaches here
}

static bool isLongPress() {
    // Button is already pressed (that's what woke us).
    // Wait to see if it's held for 3 seconds.
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    unsigned long start = millis();

    while (digitalRead(BUTTON_PIN) == LOW) {
        if (millis() - start >= LONG_PRESS_MS) {
            return true;
        }
        delay(10);
    }
    return false;
}

void setup() {
    Serial.begin(115200);
    delay(100);

    bootCount++;
    Serial.printf("Boot #%d (wake reason: %d)\n", bootCount, esp_sleep_get_wakeup_cause());

    // Init filesystem
    if (!LittleFS.begin(true)) {
        Serial.println("LittleFS mount failed");
    }

    // Seed RNG
    randomSeed(esp_random());

    // Init display
    displayInit();

    // First boot: show splash
    if (!showedSplash) {
        showedSplash = true;
        displaySplash();
        goToSleep();
        return;
    }

    // Woke from button press — check if long press (WiFi mode)
    if (esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_EXT0) {
        if (isLongPress()) {
            Serial.println("Long press — entering WiFi editor");
            startWifiEditor();
            // After WiFi editor exits, show a fresh combo
        }
    }

    // Generate and display a new lunch combo
    LunchCombo combo = generateLunch();
    if (combo.count > 0 && combo.categoryNames[0] == "Error") {
        displayError(combo.selectedItems[0].c_str());
    } else {
        displayLunch(combo);
    }

    goToSleep();
}

void loop() {
    // Never reached — we deep sleep in setup()
}
