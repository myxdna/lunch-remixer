#include "lunch_logic.h"
#include <LittleFS.h>

static const char *FOOD_FILE = "/food_list.json";
static const char *LAST_FILE = "/last_combo.json";

bool loadFoodList(JsonDocument &doc) {
    File f = LittleFS.open(FOOD_FILE, "r");
    if (!f) return false;
    DeserializationError err = deserializeJson(doc, f);
    f.close();
    return err == DeserializationError::Ok;
}

bool saveFoodList(const String &json) {
    // Validate JSON before saving
    JsonDocument test;
    if (deserializeJson(test, json) != DeserializationError::Ok) return false;
    if (!test["categories"].is<JsonArray>()) return false;

    File f = LittleFS.open(FOOD_FILE, "w");
    if (!f) return false;
    f.print(json);
    f.close();
    return true;
}

String getFoodListRaw() {
    File f = LittleFS.open(FOOD_FILE, "r");
    if (!f) return "{}";
    String s = f.readString();
    f.close();
    return s;
}

// Load last combo from flash
static LunchCombo loadLastCombo() {
    LunchCombo last;
    File f = LittleFS.open(LAST_FILE, "r");
    if (!f) return last;

    JsonDocument doc;
    if (deserializeJson(doc, f) == DeserializationError::Ok) {
        JsonObject obj = doc.as<JsonObject>();
        for (JsonPair kv : obj) {
            last.categoryNames.push_back(String(kv.key().c_str()));
            last.selectedItems.push_back(kv.value().as<String>());
            last.count++;
        }
    }
    f.close();
    return last;
}

// Save current combo
static void saveLastCombo(const LunchCombo &combo) {
    JsonDocument doc;
    for (int i = 0; i < combo.count; i++) {
        doc[combo.categoryNames[i]] = combo.selectedItems[i];
    }
    File f = LittleFS.open(LAST_FILE, "w");
    if (f) {
        serializeJson(doc, f);
        f.close();
    }
}

LunchCombo generateLunch() {
    JsonDocument doc;
    LunchCombo combo;

    if (!loadFoodList(doc)) {
        combo.categoryNames.push_back("Error");
        combo.selectedItems.push_back("Can't read food_list.json");
        combo.count = 1;
        return combo;
    }

    JsonArray categories = doc["categories"].as<JsonArray>();
    if (categories.size() == 0) {
        combo.categoryNames.push_back("Error");
        combo.selectedItems.push_back("No categories found");
        combo.count = 1;
        return combo;
    }

    LunchCombo last = loadLastCombo();

    // Try up to 10 times to get a different combo
    for (int attempt = 0; attempt < 10; attempt++) {
        combo.categoryNames.clear();
        combo.selectedItems.clear();
        combo.count = 0;

        for (JsonVariant cat : categories) {
            String name = cat["name"].as<String>();
            JsonArray items = cat["items"].as<JsonArray>();
            if (items.size() == 0) continue;

            int idx = random(0, items.size());
            String item = items[idx].as<String>();

            combo.categoryNames.push_back(name);
            combo.selectedItems.push_back(item);
            combo.count++;
        }

        // Check if different from last time
        if (last.count == 0) break;  // no last combo, accept anything

        bool different = false;
        for (int i = 0; i < combo.count && i < last.count; i++) {
            if (combo.selectedItems[i] != last.selectedItems[i]) {
                different = true;
                break;
            }
        }
        if (different) break;
    }

    saveLastCombo(combo);
    return combo;
}
