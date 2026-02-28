#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>
#include <vector>

struct LunchCombo {
    std::vector<String> categoryNames;
    std::vector<String> selectedItems;
    int count = 0;
};

// Load categories from LittleFS, pick one item per category, avoid repeating last combo
LunchCombo generateLunch();

// Load the food list JSON from LittleFS. Returns true on success.
bool loadFoodList(JsonDocument &doc);

// Save the food list JSON to LittleFS
bool saveFoodList(const String &json);

// Get the raw JSON string from LittleFS
String getFoodListRaw();
