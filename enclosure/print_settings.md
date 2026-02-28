# Print Settings — Bambu Lab P1S

## Case Dimensions
- **~125 x 71 x 14.5mm** — about as thick as a phone in a case

## Slicer: Bambu Studio

### Both Parts
- **Material:** PLA
- **Layer height:** 0.20mm
- **Infill:** 15% grid
- **Walls:** 3 perimeters
- **Top/bottom layers:** 4
- **Supports:** None needed
- **Brim:** Optional

### Part Orientation
- **Back shell:** Print open-side up (magnet pockets on build plate)
- **Front shell:** Print flat-side down (display window facing up)

### Estimated Print Time
- Back: ~35 min
- Front: ~30 min
- **Total: ~65 min**

## Bill of Materials
| Item | Notes | ~CAD |
|------|-------|------|
| LILYGO T5 4.7" v2.3 | ESP32-S3, e-paper, USB-C, battery charging built in | $55 |
| LiPo battery 503040 | 40x30x5mm, ~500-600mAh, JST PH 2.0mm connector | $7 |
| 3x neodymium magnets | 10mm x 2mm disc | $5 |
| PLA filament | ~20g total | $1 |
| **Total** | | **~$68** |

## Assembly
1. Print both shells
2. Press-fit 3 magnets into back wall pockets (super glue if needed)
3. Plug LiPo into the T5 board's JST battery connector
4. Place LiPo flat in the back shell (corner posts hold it)
5. Set T5 board on top of the support ledges (display facing up)
6. Snap front shell onto back shell
7. Charge via USB-C (port accessible through side cutout)
8. Stick on fridge

## Battery
- Charges via USB-C through the T5's built-in TP4054 charger
- ~500-600mAh lasts **6-12 months** at ~10uA deep sleep (1 press/day)
- Charge indicator LED visible through the case gap or USB-C cutout
- No battery swapping needed — just plug in USB-C occasionally

## Updating the Food List
- **No computer needed!**
- Hold the button for 3 seconds → device enters WiFi mode
- Connect your phone to "LunchRemixer" WiFi network
- Open browser → http://192.168.4.1
- Edit categories and items → tap Save
- Press button to exit WiFi mode → shows new lunch combo

## Notes
- Magnets: 3x 10mm discs hold ~150g easily (device weighs ~60g)
- If fridge isn't magnetic, use 3M Command strips
- USB-C port is accessible for charging without opening the case
- Button hole aligns with GPIO 21 button on the T5 board
