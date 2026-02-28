# Lunch Remixer — Build Guide: BOM, Buy List & Schematic

## Do I Need a Microcontroller for the Battery?

**No separate battery management board is needed.**

The LILYGO T5 4.7" v2.3 has a **TP4054 LiPo charger IC built directly onto the board**. It handles:
- Safe LiPo charging via the USB-C port (4.2V CC/CV charge profile)
- Overcharge protection
- Charge status LED indicator

You just connect the LiPo to the onboard JST connector and plug in USB-C to charge. That's it.

---

## Bill of Materials (BOM)

| # | Item | Spec | Qty | ~CAD | ~USD |
|---|------|------|-----|------|------|
| 1 | **LILYGO T5 4.7" e-Paper v2.3** | ESP32-S3, 960×540 e-ink, WiFi, USB-C, TP4054 charger, JST PH 2.0mm battery port | 1 | $55 | $40 |
| 2 | **LiPo battery — 503040** | 3.7V, ~500–600 mAh, **5mm × 30mm × 40mm**, JST PH 2.0mm connector | 1 | $7 | $5 |
| 3 | **Neodymium disc magnets** | 10mm diameter × 2mm thick | 3 | $5 | $4 |
| 4 | **PLA filament** | Any brand/color, ~20g needed | — | $1 | $1 |
| 5 | **USB-C cable** | For flashing firmware & charging | 1 | (likely have one) | — |
| | **Total** | | | **~$68** | **~$50** |

> **No soldering required.** Everything is plug-and-play.

---

## Battery — Critical Notes

### Correct Battery Size for This Case

The 3D-printed case battery bay is **40mm × 30mm × 5mm thick**. In LiPo naming convention (TxWxL):

| Code | Thickness | Width | Length | Fits? |
|------|-----------|-------|--------|-------|
| **503040** | 5mm | 30mm | 40mm | ✅ **Correct** |
| 103040 | 10mm | 30mm | 40mm | ❌ Too thick |
| 803040 | 8mm | 30mm | 40mm | ❌ Too thick |
| 503450 | 5mm | 34mm | 50mm | ❌ Too wide/long |

**Buy a 503040 cell.** Search exactly: `"LiPo 503040 3.7V JST PH 2.0"` or `"503040 lithium polymer battery"`.

### Connector Type

The T5 v2.3 board uses a **JST PH 2.0mm 2-pin** battery connector (2.0mm pin pitch).
- Positive pin: closer to board edge (verify with battery documentation before connecting)
- Most 503040 cells from major suppliers ship with this connector pre-installed

> ⚠️ Double-check polarity before plugging in. If your battery came with a JST 1.25mm or other connector, do **not** force it — get the correct connector or swap it.

### Expected Battery Life

| Usage pattern | Estimated life |
|---------------|----------------|
| 1 button press/day (normal use) | **6–12 months** |
| 5 presses/day | ~2–3 months |
| WiFi editing sessions (10 min/week) | Minimal impact |

Deep sleep current draw is ~10 μA. The display is e-ink and holds its image with zero power after refresh.

---

## Where to Buy

### LILYGO T5 4.7" v2.3

| Store | Notes |
|-------|-------|
| **LILYGO Official AliExpress** | Cheapest, ~2–4 week shipping |
| **LILYGO Official website** (lilygo.cc) | Same price, slightly faster |
| **Amazon** | Often in stock, faster shipping, ~$10 markup |
| **Mouser / Digi-Key** | Available in some regions |

Search: `LILYGO T5 4.7 e-paper v2.3 ESP32-S3`

### LiPo Battery (503040)

| Store | Notes |
|-------|-------|
| **Amazon** | Search: `503040 lipo battery JST PH 2.0` |
| **AliExpress** | Cheapest, search: `503040 3.7V lithium polymer` |
| **Adafruit** | Sells 500mAh LiPos, may need connector swap |
| **SparkFun** | Similar — verify connector pitch |

### Neodymium Magnets (10mm × 2mm)

| Store | Notes |
|-------|-------|
| **Amazon** | Search: `10mm x 2mm neodymium disc magnets` — buy a pack of 10+ |
| **AliExpress** | Very cheap in bulk |
| **Local hardware store** | Likely stocked |

---

## Schematic

No custom wiring is required. The LILYGO T5 4.7" v2.3 is a fully integrated board. The only external connections are the battery and button (the button is already on the board).

### System Block Diagram

```
                     ┌──────────────────────────────────────────────┐
                     │          LILYGO T5 4.7" v2.3 Board           │
                     │                                              │
  USB-C cable ──────►│ USB-C port                                   │
  (for charging /    │     │                                        │
   firmware flash)   │     ▼                                        │
                     │  TP4054 LiPo Charger IC                      │
                     │  (built-in, no extra parts needed)           │
                     │     │  - 4.2V charge voltage                 │
                     │     │  - CC/CV charge profile                │
                     │     │  - Charge LED indicator                │
                     │     │                                        │
  LiPo Battery ─────►│ JST PH 2.0mm (2-pin)                        │
  3.7V, 503040       │     │                                        │
  5mm × 30mm × 40mm  │     ▼                                        │
                     │  ESP32-S3 (runs at 3.3V via onboard LDO)     │
                     │  ┌──────────────────────────────────┐        │
                     │  │  GPIO 21 ◄─── Button (on board) │        │
                     │  │  (Active LOW, internal pull-up)  │        │
                     │  │                                  │        │
                     │  │  SPI/EPD ──► e-Paper Driver      │        │
                     │  │              (on board)          │        │
                     │  │                  │               │        │
                     │  │            4.7" e-ink display    │        │
                     │  │            960 × 540 px          │        │
                     │  │            (connected to board)  │        │
                     │  │                                  │        │
                     │  │  WiFi (2.4GHz, built-in)         │        │
                     │  │  PSRAM + 16MB Flash (on board)   │        │
                     │  └──────────────────────────────────┘        │
                     └──────────────────────────────────────────────┘
```

### Battery Connection Detail

```
LiPo Battery                     T5 Board JST PH 2.0mm socket
─────────────                    ──────────────────────────────
                                 ┌─────────┐
  [+] Red wire ─────────────────►│ PIN 1   │  (Battery positive)
  [-] Black wire ────────────────►│ PIN 2   │  (Battery negative / GND)
                                 └─────────┘
                                  2.0mm pitch
```

> ⚠️ Always verify polarity on the battery you purchase before connecting. Some suppliers reverse the pin order. If unsure, use a multimeter to check — red wire should read ~3.7V+ relative to black.

### Charging Circuit (Built-In TP4054)

```
USB-C port (5V in)
       │
       ▼
  TP4054 charger IC (on T5 board)
  ├── Charge current: ~500mA (standard USB)
  ├── Charge voltage: 4.2V (LiPo safe max)
  ├── Status: CHRG LED on board (lit while charging, off when full)
  └── Output ──► JST battery connector ──► LiPo battery
```

The TP4054 is a complete standalone LiPo charger — it handles the full charge cycle automatically.

---

## Assembly Order

```
Step 1: Flash firmware to T5 board (before enclosure assembly)
        └── Connect T5 via USB-C → run: pio run -t upload && pio run -t uploadfs

Step 2: Print case
        ├── back_shell.stl  (open-side up, ~35 min)
        └── front_shell.stl (flat-side down, ~30 min)

Step 3: Insert magnets
        └── Press 3× neodymium (10mm × 2mm) into back shell pockets
            (super glue optional for permanent hold)

Step 4: Connect battery
        └── Plug LiPo JST PH 2.0mm into T5 board battery port
            ⚠️ Verify polarity first!

Step 5: Place components in back shell
        ├── Lay LiPo flat in battery bay (corner posts hold it)
        └── Set T5 board on support ledges (display face-up)

Step 6: Close case
        └── Snap front shell onto back shell (3 snap ridge pairs)

Step 7: First charge
        └── Plug USB-C into side cutout → charge until LED goes off

Step 8: Stick on fridge
        └── Magnets hold ~150g; device weighs ~60g
```

---

## Power Budget Summary

| State | Current Draw | Duty Cycle |
|-------|-------------|------------|
| Deep sleep | ~10 μA | ~99.9% of the time |
| Display refresh (4 sec) | ~20–30 mA | ~0.005% per press |
| Active (WiFi editing) | ~50–100 mA | Occasional |
| **Effective average** | **~15–20 μA** | — |

A 503040 cell (~500 mAh) at 15 μA average = **~3.8 years theoretical**. Real-world accounting for self-discharge and WiFi use: **6–12 months per charge** is a realistic estimate.

---

## Quick Reference: Connector Types at a Glance

```
JST PH 2.0mm (correct for T5 v2.3)   JST SH 1.25mm (DO NOT USE — wrong size)
─────────────────────────────────     ─────────────────────────────────────
  ___                                   __
 |   |  2.0mm pin spacing              |  |  1.25mm pin spacing
 |___|  2-pin                          |__|  2-pin
 wider body                            smaller body

 ✅ Use this one                        ❌ Will not fit / wrong pitch
```
