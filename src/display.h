#pragma once

#include "lunch_logic.h"

// Initialize the EPD display and allocate framebuffer
void displayInit();

// Show the "press button for lunch" splash screen, then power off display
void displaySplash();

// Show a lunch combo on screen, then power off display
void displayLunch(const LunchCombo &combo);

// Show an error message, then power off display
void displayError(const char *msg);

// Show "WiFi mode" screen with SSID and IP
void displayWifiMode(const char *ssid, const char *ip);

// Clear framebuffer
void displayClear();

// Power off the display (call before sleep)
void displayPowerOff();
