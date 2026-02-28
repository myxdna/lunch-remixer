# Lunch Remixer

A fridge-mounted e-ink gadget that answers the daily question: **"what do I pack for lunch today?"**

Press the button → get a randomized lunch combo from your own food list. Hold 3 seconds → edit the list from your phone over WiFi. No apps to install, no screens to unlock. Just glance and pack.

## How It Works

1. You maintain a list of foods organized by category (main, side, fruit, snack, drink)
2. Press the button — the device picks one item from each category and displays a lunch combo
3. The e-ink screen holds the image with zero power until you press again
4. It never shows the exact same combo twice in a row

## Hardware

| Part | Details |
|------|---------|
| [LILYGO T5 4.7" e-Paper v2.3](https://lilygo.cc/en-ca/products/t5-4-7-inch-e-paper-v2-3) | ESP32-S3, 960x540 e-ink display, WiFi, USB-C charging, battery connector |
| 3.7V LiPo battery | 503040, ~500-600mAh, JST PH 2.0mm connector |
| 3x neodymium magnets | 10mm x 2mm disc — for fridge mounting |
| 3D printed case | Two-piece snap-fit, ~14.5mm thick, PLA |

**Total cost: ~$68 CAD**

## Editing the Food List

No computer needed:

1. **Hold the button for 3 seconds** — device enters WiFi mode
2. Connect your phone to the **"LunchRemixer"** WiFi network
3. Open a browser → go to **http://192.168.4.1**
4. Add/remove categories and items in the web editor
5. Tap **Save** → press the button to exit WiFi mode

The food list is stored on the device's flash memory and persists across reboots and battery changes.

## Building It

### Prerequisites

- [PlatformIO](https://platformio.org/) (VS Code extension or CLI)
- LILYGO T5 4.7" board connected via USB-C

### Flash the firmware

```bash
# Clone
git clone https://github.com/myxdna/lunch-remixer.git
cd lunch-remixer

# Build and upload firmware
pio run -t upload

# Upload the default food list to flash storage
pio run -t uploadfs
```

### Print the case

Open `enclosure/lunch_remixer_case.scad` in [OpenSCAD](https://openscad.org/), render each part separately (`back_shell` and `front_shell`), export as STL, and slice in Bambu Studio.

See [`enclosure/print_settings.md`](enclosure/print_settings.md) for recommended print settings and full assembly instructions.

## Project Structure

```
lunch-remixer/
├── src/
│   ├── main.cpp           # Boot, deep sleep, button handling
│   ├── lunch_logic.h/cpp  # Food list JSON, random selection, repeat avoidance
│   ├── display.h/cpp      # E-ink rendering (title, categories, items)
│   └── wifi_editor.h/cpp  # WiFi AP + web server for phone-based editing
├── data/
│   └── food_list.json     # Default food list (uploaded to LittleFS)
├── enclosure/
│   ├── lunch_remixer_case.scad  # OpenSCAD parametric case design
│   └── print_settings.md       # Print settings + assembly guide
└── platformio.ini         # Build configuration
```

## Power

- **Deep sleep between presses:** ~10μA
- **Display refresh:** ~4 seconds, then back to sleep
- **Battery life:** 6-12 months on a 503040 LiPo with daily use
- **Charging:** USB-C, built into the T5 board

## Food List Format

The device reads a simple JSON file. Categories and items are fully customizable:

```json
{
  "categories": [
    {
      "name": "Main",
      "items": ["Turkey Wrap", "PB&J", "Ham & Cheese", "Pasta Salad"]
    },
    {
      "name": "Side",
      "items": ["Baby Carrots", "Hummus & Celery", "Cheese Cubes"]
    }
  ]
}
```

Add or remove categories freely — the display adapts. Keep item names under ~20 characters for clean rendering.

## License

MIT
